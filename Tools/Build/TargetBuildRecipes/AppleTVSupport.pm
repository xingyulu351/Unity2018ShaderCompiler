package AppleTVSupport;

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

sub Register
{
    Main::RegisterTarget(
        "AppleTVSupport",
        {
            macbuildfunction => \&BuildAppleTVSupport,
            macpreparefunction => \&PrepareAppleTVSupport,
            disposefunction => \&DisposetvOSSupport,
            testsettingfunction => \&GetTestSettings,
            windowsbuildfunction => \&BuildAppleTVSupport,
            options => {
                "codegen" => ["debug", "release"],
                "incremental" => [0, 1],
                "developmentPlayer" => [0, 1],
                "scriptingBackend" => ["il2cpp"],
                "abvsubset" => [0, 1],
                "staticLib" => [0, 1]
            }
        }
    );
}

my $v2enabledFile = 'Tools/Unity.BuildSystem/Unity.BuildSystem/enable_v2_ios_by_default';

sub NewBuildCodeDisabled
{
    my $useroverride = 'UserOverride.jam';
    my $found = 0;
    if (-e $useroverride)
    {
        open(FILE, $useroverride);
        if (grep { /^USENEWBUILDCODE_IOS = 0 ;$/ } <FILE>)
        {
            $found = 1;
        }
        close(FILE);
    }
    return $found;
}

sub BuildAppleTVSupport
{
    my ($root, $targetPath, $options) = @_;

    my $isDevPlayer = $options->{developmentPlayer};    # controls dev player (shipped) parts like profiler/debugger/etc
    my $isAbvSubset = $options->{abvsubset};
    my $isDynamic = $options->{scriptingBackend} eq "il2cpp" && $isDevPlayer && !$options->{staticLib};

    if (-e $v2enabledFile && !NewBuildCodeDisabled())
    {
        ProgressMessage("Building Apple TV support (V2) on: $^O");

        my $prefix = 'tvOSPlayer';
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

            BuildPrepareAppleTVSupport($root, $targetPath, $options);

            if (!AmRunningOnBuildServer())
            {
                # local builds: build just basic, no simulator
                my $suffix = '';
                if (!$isDevPlayer && $codegen eq 'release')
                {
                    $suffix .= 'NoDevelopment';
                }
                push(@args, $prefix . $suffix);
                JamRaw($root, @args);
            }
            else
            {
                push(@args, $prefix);    # regular dev build
                push(@args, $prefix . 'x86_64');    # simulator
                if (!$isAbvSubset)
                {
                    push(@args, $prefix . 'NoDevelopment');    # non-dev build
                }
                JamRaw($root, @args);

                ProgressMessage("Test Build Development Il2cpp Apple TV Device Trampoline.");
                TestDevIl2cppTrampolineBuild_WithStaticPlayerLib($root, $targetPath, $options);
                ProgressMessage("Test Build Development Il2cpp Apple TV Simulator Trampoline.");
                TestDevIl2cppSimulatorTrampolineBuild_WithDynamicPlayerLib($root, $targetPath, $options);
            }
        }
        else
        {
            # on non-Mac just build a base player; this will only end up building the editor extension
            Jam($root, $prefix, $options->{codegen}, $^O eq 'linux' ? 'linux64' : 'win64', $options->{incremental}, @args);
        }
    }
    else
    {
        my @args = ("-sDEVELOPMENT_PLAYER=$isDevPlayer");
        if ($isDynamic)
        {
            push(@args, "-sBUILD_IOS_DYNAMIC_PLAYER=1");
        }

        ProgressMessage("Building Apple TV support on: $^O");
        if ($^O eq 'darwin')
        {
            Jam($root, 'AppleTVSupportMac', $options->{codegen}, 'macosx64', $options->{incremental}, @args);
            BuildAppleTVPlayerV1($root, $targetPath, $options);
        }
        else
        {
            Jam($root, 'AppleTVSupportWin', $options->{codegen}, 'win64', $options->{incremental}, @args);
        }
    }

    PackageAndSendResultsToBuildServer($targetPath, "AppleTVSupport");
}

sub AppendToFile
{
    my ($destination, $content) = @_;
    open(my $fh, '>>', $destination) or die "Couldn't open file `$destination`.";
    say $fh $content;
    close($fh);
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
    my ($root, $targetPath, $options) = @_;

    chdir("$targetPath/Trampoline");

    my $libiPhoneMono2x = "Libraries/libiPhone-lib.a";
    my $libAppleTVIl2cpp = "Libraries/libunity-appletv-il2cpp-dev.a";
    my $libAppleTVLibIl2cpp = "Libraries/libil2cpp-appletv-dev.a";
    my $projectPbxprj = "Unity-iPhone.xcodeproj/project.pbxproj";
    my $libtool =
        "$root/PlatformDependent/iPhonePlayer/External/NonRedistributable/BuildEnvironment/builds/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/libtool";
    my $buildCommand =
        "/Applications/Xcode7.1.app/Contents/Developer/usr/bin/xcodebuild -project Unity-iPhone.xcodeproj -target Unity-iPhone -sdk appletvos CODE_SIGN_IDENTITY=\"\" CODE_SIGNING_REQUIRED=\"NO\" CODE_SIGN_ENTITLEMENTS=\"\" CODE_SIGNING_ALLOWED=\"NO\" PRODUCT_NAME=noname -arch arm64";

    # Making sure library with expected name exists

    system("$libtool -static -o $libiPhoneMono2x $libAppleTVIl2cpp $libAppleTVLibIl2cpp") eq 0
        or die("Failed to combine Unity and libil2cpp static libraries.");

    # Making backups of files we need to patch

    UCopy($projectPbxprj, $projectPbxprj . ".bak");

    # Hacking the project to pieces, as Mantas put it, "walking around our failures"
    #   * Disable code signing, since there is a problem with AppleTV provisioning profiles at the moment
    #   * Replace CoreMotion with GameKit (it seems AppleTV platform renamed this framework)
    #   * Remove -weak-lSystem , it's some sort of legacy framework for all i can gather and it's not present anymore

    ReplaceInPlace(
        $projectPbxprj,
        sub
        {
            $_[0] =~ s/\"CODE_SIGN_IDENTITY\[sdk\=iphoneos\*\]\" \= \"iPhone Developer\"\;/CODE_SIGNING_REQUIRED = NO;/g;
            $_[0] =~ s/CoreMotion/GameKit/g;
            $_[0] =~ s/\"\-weak\-lSystem\",//g;
        }
    );

    # Actually trying to build it

    system("$buildCommand clean");
    system("$buildCommand") eq 0 or die("Failed to build tvOS trampoline application.");
    system("$buildCommand clean");

    # Remove temporary modified files

    unlink($libiPhoneMono2x);
    unlink($projectPbxprj);

    # Restoring backups

    UMove($projectPbxprj . ".bak", $projectPbxprj);
}

sub TestDevIl2cppSimulatorTrampolineBuild_WithDynamicPlayerLib
{
    my ($root, $targetPath, $options, $invoked_by_iosbuild) = @_;

    chdir("$targetPath/Trampoline");

    # [ Select dev/il2cpp simulator dylib variation libs]
    # Player
    my $playerLibOrigin = "Libraries/libunity-appletv-amd64-il2cpp.dylib";
    my $playerLibXcode = "Libraries/libiPhone-lib.dylib";
    UCopy($playerLibOrigin, $playerLibXcode);

    # Il2cpp
    my $il2cppLibOrigin = "Libraries/libil2cpp-appletv-amd64.a";
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

            # Rename libiPhone-lib.a to libiPhone-lib.dylib
            $_[0] =~ s/libiPhone-lib.a/libiPhone-lib.dylib/g;

            $_[0] =~ s/CoreMotion/GameKit/g;

            $_[0] =~ s/\"\-weak\-lSystem\",//g;
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
        "/Applications/Xcode7.1.app/Contents/Developer/usr/bin/xcodebuild -project Unity-iPhone.xcodeproj -target Unity-iPhone -sdk appletvsimulator CODE_SIGN_IDENTITY=\"\" CODE_SIGNING_REQUIRED=\"NO\" CODE_SIGN_ENTITLEMENTS=\"\" CODE_SIGNING_ALLOWED=\"NO\" PRODUCT_NAME=noname -arch x86_64 GCC_PREPROCESSOR_DEFINITIONS=\"INIT_SCRIPTING_BACKEND=1\"";
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

sub BuildAppleTVPlayerV1
{
    my ($root, $targetPath, $options) = @_;
    my $isAbvSubset = $options->{abvsubset};
    my $isStaticLib = $options->{staticLib};

    if (!$options->{incremental})
    {
        Cleanup();
    }

    my $isDevPlayer = $options->{developmentPlayer};    # controls dev player (shipped) parts like profiler/debugger/etc

    BuildPrepareAppleTVSupport($root, $targetPath, $options);

    ProgressMessage("Building Apple TV player.");
    if (!AmRunningOnBuildServer())
    {
        my @args_local = ("-sDEVELOPMENT_PLAYER=$isDevPlayer");
        push(@args_local, "-sSCRIPTING_BACKEND=il2cpp");
        if ($isDevPlayer && !$isStaticLib)
        {
            push(@args_local, "-sBUILD_IOS_DYNAMIC_PLAYER=1");
        }

        Jam($root, 'AppleTVPlayer', $options->{codegen}, 'iphone', $options->{incremental}, @args_local);
    }
    else
    {
        my @args_dev = ("-sDEVELOPMENT_PLAYER=1");

        my @args_ndev = ("-sDEVELOPMENT_PLAYER=0");

        # IL2CPP version
        push(@args_dev, "-sSCRIPTING_BACKEND=il2cpp");
        push(@args_ndev, "-sSCRIPTING_BACKEND=il2cpp");

        # AppleTV version
        if (!$isAbvSubset)
        {
            Jam($root, 'AppleTVPlayer', $options->{codegen}, 'iphone', $options->{incremental}, @args_ndev);
        }

        Jam($root, 'AppleTVPlayer', $options->{codegen}, 'iphone', $options->{incremental}, @args_dev);
        Jam($root, 'AppleTVSimulatorPlayer', $options->{codegen}, 'iphonesimulator', $options->{incremental}, @args_dev);

        ProgressMessage("Test Build Development Il2cpp Apple TV Device Trampoline.");
        TestDevIl2cppTrampolineBuild_WithStaticPlayerLib($root, $targetPath, $options);

        ProgressMessage("Test Build Development Il2cpp Apple TV Simulator Trampoline.");
        TestDevIl2cppSimulatorTrampolineBuild_WithDynamicPlayerLib($root, $targetPath, $options);
    }
}

sub BuildPrepareAppleTVSupport
{
    my ($root, $targetPath, $options) = @_;

    # Cleanup previous stuff
    rmtree("$targetPath/Trampoline");
    mkpath("$targetPath/Trampoline/Libraries");
}

sub PrepareAppleTVSupport
{
    my ($root, $targetPath, $options) = @_;
    SetDeviceIdEnvironmentVariableIfNotSet($root);
}

sub GetTestSettings
{
    my ($root, $targetPath, $options) = @_;
    my %testSettings;
    my $tvOSManualProvisioningProfileID = $ENV{'UNITY_TVOSPROVISIONINGUUID'};
    my $appleDeveloperTeamID = $ENV{'UNITY_APPLEDEVELOPMENTTEAM'};
    if (defined $tvOSManualProvisioningProfileID)
    {
        $testSettings{'appleEnableAutomaticSigning'} = 'false';
        $testSettings{'tvOSManualProvisioningProfileType'} = 'Development';
        $testSettings{'tvOSManualProvisioningProfileID'} = $tvOSManualProvisioningProfileID;
    }
    if (defined $appleDeveloperTeamID)
    {
        $testSettings{'appleDeveloperTeamID'} = $appleDeveloperTeamID;
    }
    return \%testSettings;
}

sub DisposetvOSSupport
{
    my $exitCode = system("killall Xcode");
    $exitCode >>= 8;
    if ($exitCode != 0)
    {
        print STDERR "Could not kill Xcode!\n";
    }
}

1;
