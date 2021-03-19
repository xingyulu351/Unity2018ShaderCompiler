package iOSSupport;

use strict;
use warnings;
use Cwd;
use File::Path qw (mkpath);
use File::Basename qw(dirname);
use File::Spec;
use File::Path;
use lib File::Spec->rel2abs(dirname(__FILE__) . "/../../..") . '/External/Perl/lib';
use lib File::Spec->rel2abs(dirname(__FILE__) . "/..");
use MacTools qw (Cleanup SetDeviceIdEnvironmentVariableIfNotSet);
use Tools qw (UCopy UMove Jam JamRaw AmRunningOnBuildServer ProgressMessage WriteFile);
use Tools qw (PackageAndSendResultsToBuildServer);

use lib File::Spec->rel2abs(dirname(__FILE__) . "/../../SDKDownloader");
use SDKDownloader;

## Get build options
use BuildConfig;
our $unityVersion = $BuildConfig::unityVersion;
our $copyright = $BuildConfig::copyright;

sub Register
{
    Main::RegisterTarget(
        "iOSSupport",
        {
            macbuildfunction => \&BuildiOSSupport,
            macpreparefunction => \&PrepareiOSSupport,
            disposefunction => \&DisposeiOSSupport,
            testsettingfunction => \&GetTestSettings,
            windowsbuildfunction => \&BuildiOSSupport,
            linuxbuildfunction => \&BuildiOSSupport,
            options => {
                "codegen" => ["debug", "release"],
                "incremental" => [0, 1],
                "developmentPlayer" => [0, 1],
                "scriptingBackend" => ["default", "il2cpp"],
                "abvsubset" => [0, 1],
                "staticLib" => [0, 1]
            }
        }
    );
}

sub BuildiOSSupport
{
    my ($root, $targetPath, $options) = @_;

    if ($^O eq 'linux')
    {
        if (AmRunningOnBuildServer() || exists $ENV{'UNITY_USE_LINUX_SDK'})
        {
            ProgressMessage('Setting up the Linux SDK');
            SDKDownloader::PrepareSDK('linux-sdk', '20180406', "$root/artifacts");
        }
    }

    my $isDevPlayer = $options->{developmentPlayer};    # controls dev player (shipped) parts like profiler/debugger/etc
    my $isAbvSubset = $options->{abvsubset};
    my $isIL2CPP = $options->{scriptingBackend} eq "il2cpp";
    my $isDynamic = $isIL2CPP && $isDevPlayer && !$options->{staticLib};

    {
        ProgressMessage("Building iOS support (V2) on: $^O");

        my $prefix = 'iOSPlayer';
        my @args = ();
        if (!$options->{incremental})
        {
            push(@args, "-a");
        }
        my $codegen = lc($options->{codegen});
        push(@args, "-sCONFIG=$codegen");
        if ($isDynamic)
        {
            push(@args, "-sBUILD_IOS_DYNAMIC_PLAYER=1");
        }

        if ($^O eq 'darwin')
        {
            if (!$options->{incremental})
            {
                Cleanup();
            }

            BuildPrepareiOSSupport($root, $targetPath, $options);

            if (!AmRunningOnBuildServer())
            {
                # local builds: build just basic things (one scripting backend; development or
                # non-development; no simulator)
                my $suffix = '';
                if ($isIL2CPP)
                {
                    $suffix .= 'IL2CPP';
                }
                if (!$isDevPlayer && $codegen eq 'release')
                {
                    $suffix .= 'NoDevelopment';
                }
                push(@args, $prefix . $suffix);
                if ($isIL2CPP)
                {
                    push(@args, $prefix . '64' . $suffix);
                }
                JamRaw($root, @args);
            }
            else
            {
                if (!$isAbvSubset)
                {
                    # simulators
                    push(@args, $prefix . 'x86');
                    push(@args, $prefix . 'x86_64IL2CPP');

                    # mono
                    push(@args, $prefix);
                    push(@args, $prefix . 'NoDevelopment');

                    # il2cpp
                    push(@args, $prefix . 'IL2CPP');
                    push(@args, $prefix . 'IL2CPPNoDevelopment');
                    push(@args, $prefix . '64IL2CPP');
                    push(@args, $prefix . '64IL2CPPNoDevelopment');
                    JamRaw($root, @args);
                }
                else
                {
                    # il2cpp: 32, 64, simulator
                    push(@args, $prefix . 'IL2CPP');
                    push(@args, $prefix . '64IL2CPP');
                    push(@args, $prefix . 'x86_64IL2CPP');
                    push(@args, "-sASSERTS_ENABLED=1");    # always enable asserts in ABVSubset
                    JamRaw($root, @args);
                }

                ProgressMessage("Validating trampoline for developer il2cpp simulator.");
                TestDevIl2cppSimulatorTrampolineBuild_WithDynamicPlayerLib($root, $targetPath, $options);
                ProgressMessage("Validating trampoline for developer il2cpp device.");
                TestDevIl2cppTrampolineBuild_WithStaticPlayerLib($root, $targetPath, $options);
            }

            if ($options->{buildAutomation})
            {
                Jam($root, 'iPhoneAutomation', '', '', $options->{incremental});
            }
        }
        else
        {
            # on non-Mac just build a base player; this will only end up building the editor extension
            Jam($root, $prefix, $options->{codegen}, $^O eq 'linux' ? 'linux64' : 'win64', $options->{incremental}, @args);
        }
    }

    PackageAndSendResultsToBuildServer($targetPath, "iOSSupport");
}

sub ReplaceInPlace
{
    my ($destination, $code) = @_;
    open(IN, '<', $destination) or die "Couldn't open file `$destination`.";
    my $list = '';
    while (<IN>)
    {
        $code->($_);
        $list .= $_;
    }
    close(IN);
    open(OUT, '>', $destination) or die "Couldn't open file `$destination`.";
    print OUT $list;
    close(OUT);
}

sub TestDevIl2cppTrampolineBuild_WithStaticPlayerLib
{
    my ($root, $targetPath, $options, $invoked_by_iosbuild) = @_;

    chdir("$targetPath/Trampoline");

    my $libiPhoneMono2x = "Libraries/libiPhone-lib.a";
    my $libiPhoneIl2cpp = "Libraries/libiPhone-lib-il2cpp-dev.a";
    my $libiPhoneLibIl2cpp = "Libraries/libil2cpp-dev.a";
    my $projectPbxprj = "Unity-iPhone.xcodeproj/project.pbxproj";
    my $libtool =
        "$root/PlatformDependent/iPhonePlayer/External/NonRedistributable/BuildEnvironment/builds/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/libtool";
    my $buildCommand =
        "/Applications/Xcode7.1.app/Contents/Developer/usr/bin/xcodebuild -project Unity-iPhone.xcodeproj -target Unity-iPhone -sdk iphoneos PRODUCT_NAME=noname -arch arm64";

    # Making backups of files we need to patch

    UCopy($projectPbxprj, $projectPbxprj . ".bak");
    if (-e $libiPhoneMono2x)
    {    # mono non-dev lib will be called libiPhone-lib.a.
        UCopy($libiPhoneMono2x, $libiPhoneMono2x . ".bak");
    }

    # Making sure library with expected name exists

    system("$libtool -static -o $libiPhoneMono2x $libiPhoneIl2cpp $libiPhoneLibIl2cpp") eq 0 or die("Failed to combine Unity and libil2cpp static libraries.");

    # Disable code signing
    ReplaceInPlace(
        $projectPbxprj,
        sub
        {
            $_[0] =~ s/\"CODE_SIGN_IDENTITY\[sdk\=iphoneos\*\]\" \= \"iPhone Developer\"\;/CODE_SIGNING_REQUIRED = NO;/g;
        }
    );

    # Actually trying to build it

    system("$buildCommand clean");
    system("$buildCommand") eq 0 or die("Failed to build iOS trampoline application.");
    system("$buildCommand clean");

    # Remove temporary modified files

    unlink($projectPbxprj);

    # Restoring backups

    UMove($projectPbxprj . ".bak", $projectPbxprj);
    if (-e $libiPhoneMono2x . ".bak")
    {
        UMove($libiPhoneMono2x . ".bak", $libiPhoneMono2x);
    }
    else
    {
        unlink($libiPhoneMono2x);
    }
}

sub TestDevIl2cppTrampolineBuild_WithDynamicPlayerLib
{
    my ($root, $targetPath, $options, $invoked_by_iosbuild) = @_;

    chdir("$targetPath/Trampoline");

    # [ Select dev/il2cpp variation libs]
    # Player
    my $playerLibOrigin = "Libraries/libiPhone-lib-il2cpp-dev.dylib";
    my $playerLibXcode = "Libraries/libiPhone-lib.dylib";
    UCopy($playerLibOrigin, $playerLibXcode);

    # Il2cpp
    my $il2cppLibOrigin = "Libraries/libil2cpp-dev.a";
    my $il2cppLibXcode = "Libraries/libil2cpp.a";
    if (-e $il2cppLibXcode)
    {
        UMove($il2cppLibXcode, $il2cppLibXcode . ".orig");
    }
    UCopy($il2cppLibOrigin, $il2cppLibXcode);

    # [ Modify Xcode Project ]
    my $projectPbxprj = "Unity-iPhone.xcodeproj/project.pbxproj";
    UCopy($projectPbxprj, $projectPbxprj . ".orig");

    ReplaceInPlace(
        $projectPbxprj,
        sub
        {
            # Disable code signing
            $_[0] =~ s/\"CODE_SIGN_IDENTITY\[sdk\=iphoneos\*\]\" \= \"iPhone Developer\"\;/CODE_SIGNING_REQUIRED = NO;/g;

            # Disable bitcode
            $_[0] =~ s/ENABLE_BITCODE = YES;/ENABLE_BITCODE = NO;/g;

            # Change deployment target 7.0 -> 8.0
            $_[0] =~ s/IPHONEOS_DEPLOYMENT_TARGET = 7.0;/IPHONEOS_DEPLOYMENT_TARGET = 8.0;/g;
        }
    );

    # [ Actually trying to build it ]
    my $buildCommand =
        "/Applications/Xcode7.1.app/Contents/Developer/usr/bin/xcodebuild -project Unity-iPhone.xcodeproj -target Unity-iPhone -sdk iphoneos PRODUCT_NAME=noname -arch arm64";
    system("$buildCommand clean");
    system("$buildCommand") eq 0
        or die("Failed to build dev/il2cpp iOS application from raw trampoline.\nNote: modifications to trampoline were not reverted.");
    system("$buildCommand clean");

    # [ Revert to original trampoline ]
    unlink($playerLibXcode);

    unlink($il2cppLibXcode);
    if (-e $il2cppLibXcode . ".orig")
    {
        UMove($il2cppLibXcode . ".orig", $il2cppLibXcode);
    }

    unlink($projectPbxprj);
    UMove($projectPbxprj . ".orig", $projectPbxprj);
}

sub TestDevIl2cppSimulatorTrampolineBuild_WithDynamicPlayerLib
{
    my ($root, $targetPath, $options, $invoked_by_iosbuild) = @_;

    chdir("$targetPath/Trampoline");

    # [ Select dev/il2cpp simulator dylib variation libs]
    # Player
    my $playerLibOrigin = "Libraries/libiPhone-lib-amd64-il2cpp.dylib";
    my $playerLibXcode = "Libraries/libiPhone-lib.dylib";
    UCopy($playerLibOrigin, $playerLibXcode);

    # Il2cpp
    my $il2cppLibOrigin = "Libraries/libil2cpp-amd64.a";
    my $il2cppLibXcode = "Libraries/libil2cpp.a";
    if (-e $il2cppLibXcode)
    {
        UMove($il2cppLibXcode, $il2cppLibXcode . ".orig");
    }
    UCopy($il2cppLibOrigin, $il2cppLibXcode);

    # [ Modify Xcode Project ]
    my $projectPbxprj = "Unity-iPhone.xcodeproj/project.pbxproj";
    UCopy($projectPbxprj, $projectPbxprj . ".orig");

    ReplaceInPlace(
        $projectPbxprj,
        sub
        {
            # Disable code signing
            $_[0] =~ s/\"CODE_SIGN_IDENTITY\[sdk\=iphoneos\*\]\" \= \"iPhone Developer\"\;/CODE_SIGNING_REQUIRED = NO;/g;

            # Disable bitcode
            $_[0] =~ s/ENABLE_BITCODE = YES;/ENABLE_BITCODE = NO;/g;

            # Change deployment target 7.0 -> 8.0
            $_[0] =~ s/IPHONEOS_DEPLOYMENT_TARGET = 7.0;/IPHONEOS_DEPLOYMENT_TARGET = 8.0;/g;

            # Rename libiPhone-lib.a to libiPhone-lib.dylib
            $_[0] =~ s/libiPhone-lib.a/libiPhone-lib.dylib/g;
        }
    );

    # [ Modify Preprocessor.h ]
    my $preprocessorOrigin = "Classes/Preprocessor.h";
    UCopy($preprocessorOrigin, $preprocessorOrigin . ".orig");
    ReplaceInPlace(
        $preprocessorOrigin,
        sub
        {
            # Enable dylib
            $_[0] =~ s/#define UNITY_USES_DYNAMIC_PLAYER_LIB 0/#define UNITY_USES_DYNAMIC_PLAYER_LIB 1/g;
        }
    );

    # [ Modify main.mm ]
    my $mainOrigin = "Classes/main.mm";
    UCopy($mainOrigin, $mainOrigin . ".orig");
    open(OUT, '>>', $mainOrigin) or die "Couldn't open file `$mainOrigin`.";
    print OUT "\nextern \"C\" void RegisterStaticallyLinkedModulesGranular() {}\n#include \"DynamicLibEngineAPI.mm\"\n";
    close(OUT);

    # [ Actually trying to build it ]
    my $buildCommand =
        "/Applications/Xcode7.1.app/Contents/Developer/usr/bin/xcodebuild -project Unity-iPhone.xcodeproj -target Unity-iPhone -sdk iphonesimulator PRODUCT_NAME=noname -arch x86_64 GCC_PREPROCESSOR_DEFINITIONS=\"INIT_SCRIPTING_BACKEND=1\"";
    system("$buildCommand clean");
    system("$buildCommand") eq 0
        or die("Failed to build dev/il2cpp iOS application from raw trampoline.\nNote: modifications to trampoline were not reverted.");
    system("$buildCommand clean");

    # [ Revert to original trampoline ]
    unlink($playerLibXcode);

    unlink($il2cppLibXcode);
    if (-e $il2cppLibXcode . ".orig")
    {
        UMove($il2cppLibXcode . ".orig", $il2cppLibXcode);
    }

    unlink($projectPbxprj);
    UMove($projectPbxprj . ".orig", $projectPbxprj);

    unlink($preprocessorOrigin);
    UMove($preprocessorOrigin . ".orig", $preprocessorOrigin);

    unlink($mainOrigin);
    UMove($mainOrigin . ".orig", $mainOrigin);
}

sub BuildPrepareiOSSupport
{
    my ($root, $targetPath, $options) = @_;

    if (!$options->{incremental})
    {
        # Cleanup previous stuff
        rmtree("$targetPath/Trampoline");
        mkpath("$targetPath/Trampoline/Libraries");
    }

    GenerateUnityTrampolineConfigure($root);
}

sub PrepareiOSSupport
{
    my ($root, $targetPath, $options) = @_;
    SetDeviceIdEnvironmentVariableIfNotSet($root);
}

sub GetTestSettings
{
    my ($root, $targetPath, $options) = @_;
    my %testSettings;
    my $iOSManualProvisioningProfileID = $ENV{'UNITY_IOSPROVISIONINGUUID'};
    my $appleDeveloperTeamID = $ENV{'UNITY_APPLEDEVELOPMENTTEAM'};
    if (defined $iOSManualProvisioningProfileID)
    {
        $testSettings{'appleEnableAutomaticSigning'} = 'false';
        $testSettings{'iOSManualProvisioningProfileType'} = 'Development';
        $testSettings{'iOSManualProvisioningProfileID'} = $iOSManualProvisioningProfileID;
    }
    if (defined $appleDeveloperTeamID)
    {
        $testSettings{'appleDeveloperTeamID'} = $appleDeveloperTeamID;
    }
    return \%testSettings;
}

sub DisposeiOSSupport
{
    my $exitCode = system("killall Xcode");
    $exitCode >>= 8;
    if ($exitCode != 0)
    {
        print STDERR "Could not kill Xcode!\n";
    }
}

sub GenerateUnityTrampolineConfigure
{
    my ($root) = @_;

    my ($ver_version, $ver_major, $ver_minor) = $BuildConfig::unityVersion =~ /^(\d+)\.(\d+)\.(\d+)/;
    my $cur_ver_define = "#define UNITY_" . "$ver_version" . "_" . $ver_major . "_" . $ver_minor . " $ver_version$ver_major$ver_minor\n";
    my $cur_ver_regex = "#define UNITY_" . "$ver_version" . "_" . $ver_major . "_" . $ver_minor . " $ver_version$ver_major$ver_minor\\s*";

    my $trampoline_configure_fname = "$root/PlatformDependent/iPhonePlayer/Trampoline/Classes/UnityTrampolineConfigure.h";
    my $trampoline_configure;

    my $need_append_version = 1;
    my $contents = "";

    open($trampoline_configure, $trampoline_configure_fname) or die $!;
    while (my $line = <$trampoline_configure>)
    {
        if ($line =~ $cur_ver_regex)
        {
            $need_append_version = 0;
        }

        if ($line =~ "#define UNITY_VERSION [\\d]+[\\s]*")
        {
            $contents = $contents . "#define UNITY_VERSION $ver_version$ver_major$ver_minor\n";
        }
        else
        {
            $contents = $contents . $line;
        }
    }
    close($trampoline_configure);

    if ($need_append_version)
    {
        $contents = $contents . $cur_ver_define;
    }

    WriteFile($trampoline_configure_fname, $contents);
}

1;
