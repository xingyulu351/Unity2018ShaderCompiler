package Tools;

use warnings;
use strict;
use File::Basename qw (dirname basename fileparse);
use File::Spec;
use File::Path qw (mkpath rmtree);
use File::Copy qw (move);
use File::Temp qw (tempfile);
use lib File::Spec->rel2abs(dirname(__FILE__)) . '/../../External/Perl/lib';
use lib File::Spec->rel2abs(dirname(__FILE__)) . '/../../Configuration';
use File::chdir;
use Cwd qw (realpath getcwd);
use Carp qw (croak carp confess);
use File::stat;
use Crypt::CBC;
use MIME::Base64;
use IPC::System::Simple qw(capture);
use Capture::Tiny qw(tee_merged capture_merged);
use Path::Class;

require Exporter;
our @ISA = qw (Exporter);

our @EXPORT_OK = qw (SetupPlaybackEngines UCopy GenerateUnityConfigure XBuild XBuild4 Jam JamRaw
    NewXBuild ReadFile EncryptFile DecryptFile SignJar RmtreeSleepy MkpathSleepy SetupMono AmRunningOnBuildServer IsWindows8OrNewer
    DeleteRecursive ProgressMessage
    TeamCityOnlyProgressMessage UMove ReplaceText GenerateUnityEvents
    TakeFirstElementIfPrefixMatch TakeFirstElementIfAnyPrefixMatches HasAnyElementThatIsPrefixMatch ConvertToLowercaseAndMatchByPrefix
    OpenBrowser LaunchExecutableAndExit
    WriteFile DumpBinaryToByteArray PackageAndSendResultsToBuildServer
    extractArtifact
    SetBuildEnvironment
    SetLogPath
    BuildBundleFileList
    GetMono64Exe
);

my ($modulePath, $root);

BEGIN
{
    $modulePath = realpath(File::Spec->rel2abs(dirname(__FILE__)) . '/../../External/Perl/lib');
    $root = realpath(File::Spec->rel2abs(dirname(__FILE__)) . '/../..');

}
use lib "$modulePath/../../Configuration";
use BuildConfig;
use lib "$modulePath/../../External/Perl/lib";
use OTEE::BuildUtils qw(svnGetRevision);
use OTEE::BuildUtils qw(gitGetRevision);

our $baseLogPath = "$root/build/log";
our $buildenv = "dev";

sub SetLogPath
{
    my ($logPath) = @_;
    $baseLogPath = File::Spec->catdir($root, $logPath);
}

# Define build environment via command line options
sub SetBuildEnvironment
{
    my ($newEnvironment) = @_;
    if ($newEnvironment =~ /dev|farm/i)
    {
        $buildenv = lc($newEnvironment);
    }
    else
    {
        die("Invalid build environment specified.");
    }
}

sub GenerateUnityEvents($$$)
{
    my $unityEventTypeCount = shift;
    my $unityEventTemplate = shift;
    my $unityEventOutputPath = shift;

    print("Generating $unityEventTypeCount UnityEvents from $unityEventTemplate\n");

    open(TEMPLATE, $unityEventTemplate) || croak("Could not open $unityEventTemplate");
    my $templateContents = join('', <TEMPLATE>);

    for (my $i = 0; $i < $unityEventTypeCount; ++$i)
    {
        my $templateParams = '';
        my $argumentParams = '';
        my $arguments = '';
        my $types = '';
        my $invokeArraySetup = '';

        for (my $param = 0; $param < $i; ++$param)
        {
            if ($param == 0)
            {
                $templateParams = '<';
            }
            else
            {
                $templateParams .= ', ';
                $argumentParams .= ', ';
                $arguments .= ', ';
                $types .= ', ';
            }
            my $Tparam = "T$param";
            my $argparam = "arg$param";
            $templateParams .= $Tparam;
            $argumentParams .= "$Tparam $argparam";
            $arguments .= $argparam;
            $types .= "typeof($Tparam)";
            $invokeArraySetup .= "m_InvokeArray[$param] = $argparam; ";
        }
        if ($i > 0)
        {
            $templateParams .= '>';
        }

        my $classAccess = 'abstract';
        if ($i == 0)
        {
            $classAccess = '';
        }

        my $result = $templateContents;
        $result =~ s/\$\{TEMPLATEPARAMS}/$templateParams/g;
        $result =~ s/\$\{ARGUMENTPARAMS}/$argumentParams/g;
        $result =~ s/\$\{ARGUMENTS}/$arguments/g;
        $result =~ s/\$\{TYPES}/$types/g;
        $result =~ s/\$\{CLASSACCESS}/$classAccess/g;
        $result =~ s/\$\{INVOKEARRAYSETUP}/$invokeArraySetup/g;
        $result =~ s/\$\{ARGUMENTCOUNT}/$i/g;

        mkpath($unityEventOutputPath);
        open(UNITYEVENT, '>', "$unityEventOutputPath/UnityEvent_$i.cs") || die("Could not open $unityEventOutputPath/UnityEvent_$i.cs");
        print(UNITYEVENT $result);
        close(UNITYEVENT);
    }
}

sub GenerateUnityConfigure
{
    my $versionOverride = shift;

    if (!defined($versionOverride))
    {
        $versionOverride = "";
    }

    if ($versionOverride ne "")
    {
        $BuildConfig::unityVersion = $versionOverride;
    }
    chdir("$root");

    # parse version into version.minor.major numbers
    my ($ver_version, $ver_major, $ver_minor, $ver_suffix) = $BuildConfig::unityVersion =~ /^(\d+)\.(\d+)\.(\d+)(.+)/;

    # Get svn revision
    my %svn_info = svnGetRevision();
    my $ver_revision = $svn_info{Revision};
    my $ver_numericRevision = $svn_info{NumericRevision};
    my $ver_date = $svn_info{Date};
    my $ver_url = $svn_info{URL};
    my $ver_branch = $svn_info{Branch};
    my $git_revision = gitGetRevision();

    # TODO: Move this into svnGetRevision
    if (!$ver_revision)
    {
        $ver_revision = 0;
        print("Failed to retrieve build revision number from source control, using dummy value.\n");
    }
    my $ver_string = $BuildConfig::unityVersion;

    my $ver_string_full = $ver_string . '_' . $ver_revision;
    my $ver_string_full_nice = $ver_string . ' (' . $ver_revision . ')';
    my $unity_build_info = $git_revision . ' | ' . $BuildConfig::year . '-' . ($BuildConfig::mon+1) . '-' . $BuildConfig::mday;

    # The branch url is only included for betas, and only the two last parts of the path
    my $ver_string_branch_url = "";
    if ($BuildConfig::unityIsBeta == 1)
    {
        $ver_string_branch_url = $ver_url;
        $ver_string_branch_url =~ s{^.*/([^/]+/[^/]+)/*$}{$1};
        $ver_string_branch_url =~ s{\\}{\\\\}g;    # escape the backslashes, this will happen when repo was cloned locally
    }

    # Construct file contents strings
    my $f = '';
    $f = $f . "#pragma once\n";
    $f = $f . "// Generated by build script, do not edit!\n";
    $f = $f . "#define UNITY_COMPANY \"$BuildConfig::company\"\n";
    $f = $f . "#define UNITY_COPYRIGHT \"$BuildConfig::copyright\"\n";

    $f = $f . "#define UNITY_ENABLE_HWSTATS_REPORT $BuildConfig::enableHWStats\n";

    $f = $f . "#define UNITY_BUILD_ROOT \"$root\"\n";

    my $fver = '';
    $fver = $fver . "#pragma once\n";
    $fver = $fver . "// Generated by build script, do not edit!\n";
    $fver = $fver . "#define UNITY_VERSION \"$ver_string\"\n";
    $fver = $fver . "#define UNITY_VERSION_DIGITS \"$ver_version.$ver_major.$ver_minor\"\n";
    $fver = $fver . "#define UNITY_VERSION_SUFFIX \"$ver_suffix\"\n";
    $fver = $fver . "#define UNITY_VERSION_VER $ver_version\n";
    $fver = $fver . "#define UNITY_VERSION_MAJ $ver_major\n";
    $fver = $fver . "#define UNITY_VERSION_MIN $ver_minor\n";
    $fver = $fver . "#define UNITY_IS_BETA $BuildConfig::unityIsBeta\n";
    $fver = $fver . "#define UNITY_BUILD_INFO \"$unity_build_info\"\n";
	
    my ($lengine_version_main, $lengine_version_sub, $lengine_version_build) = $BuildConfig::lengineVersion =~ /^(\d+)\.(\d+)\.(\d+)/;
    $fver = $fver . "// Add LEngine defines.\n";
	$fver = $fver . "#define LENGINE_VERSION \"$BuildConfig::lengineVersion\"\n";
    $fver = $fver . "#define LENGINE_VERSION_MAIN $lengine_version_main\n";
    $fver = $fver . "#define LENGINE_VERSION_SUB $lengine_version_sub\n";
    $fver = $fver . "#define LENGINE_VERSION_BUILD $lengine_version_build\n";    

    my $frev = '';
    $frev = $frev . "#pragma once\n";
    $frev = $frev . "// Generated by build script, do not edit!\n";
    $frev = $frev . "#define UNITY_VERSION_FULL \"$ver_string_full\"\n";
    $frev = $frev . "#define UNITY_VERSION_FULL_NICE \"$ver_string_full_nice\"\n";
    $frev = $frev . "#define UNITY_VERSION_BLD $ver_numericRevision\n";
    $frev = $frev . "#define UNITY_VERSION_DATE $ver_date\n";
    $frev = $frev . "#define UNITY_VERSION_WIN \"$ver_version.$ver_major.$ver_minor.$ver_numericRevision\"\n";
    $frev = $frev . "#define UNITY_VERSION_BRANCH \"$ver_branch\"\n";

    my $csver = '';
    $csver = $csver . "using UnityEngine;\n";
    $csver = $csver . "// Generated by build script, do not edit!\n";
    $csver = $csver . "[assembly:UnityAPICompatibilityVersion(\"$ver_string\")]\n";

    # Write out files
    mkpath("$root/artifacts/generated/Configuration");

    WriteFile("$root/artifacts/generated/Configuration/UnityConfigureOther.gen.h", $f);
    WriteFile("$root/artifacts/generated/Configuration/UnityConfigureVersion.gen.h", $fver);
    WriteFile("$root/artifacts/generated/Configuration/UnityConfigureRevision.gen.h", $frev);
    WriteFile("$root/artifacts/generated/Configuration/UnityAPICompatibilityVersion.cs", $csver);

    WriteConfigurationProperty('Copyright', $BuildConfig::copyright);
    WriteConfigurationProperty('Version/Revision', $ver_revision);
}

sub WriteConfigurationProperty
{
    my ($property, $value) = @_;
    my $path = "artifacts/generated/Configuration/Property/$property";
    mkpath(dirname($path));
    WriteFile($path, $value);
}

# Writes the file (but only if different from existing)
sub WriteFile
{
    my ($fname, $text) = @_;

    my $isFileClosed = 1;
    open(my $result, '<', $fname) or $isFileClosed = 0;

    unless ($isFileClosed and (join '', <$result>) eq $text)
    {
        close($result);
        open(my $fout, '>', $fname) or confess("FAILED: $!");

        # Do not do line endings conversion on windows
        if ($^O eq 'MSWin32') { binmode($fout, ":raw"); }
        print($fout $text);
        print("Generated $fname\n");
    }
}

sub XBuild
{
    my ($root, $sln, $config, $targetdir, @producedfiles) = @_;
    my $xbuild = "$root/External/Mono/builds/monodistribution/bin/xbuild";
    my $slndir = dirname($sln);

    if ($ENV{Platform} || $ENV{PLATFORM})
    {
        die(
            "the environment variable 'Platform' should not be set when building Unity. This is a problem with some hp-branded windows PCs which have the variable set by default. Remove it in your system properties."
        );
    }

    system($xbuild, $sln, "/property:Configuration=$config") eq 0 or croak("Failing xbuilding $sln");

    if (defined $targetdir)
    {
        for my $file (@producedfiles)
        {
            print "trying to copy $slndir/$file to $targetdir\n";
            UCopy("$slndir/$file", $targetdir);
        }
    }
}

sub NewXBuild
{
    my ($root, $sln, $config, $targetdir, @producedfiles) = @_;
    my $xbuild = "$root/External/MonoBleedingEdge/builds/monodistribution/bin/xbuild";
    my $slndir = dirname($sln);
    my $mcs = "mcs";

    if ($^O eq 'MSWin32')
    {
        $mcs = "mcs.bat";
    }

    if ($ENV{Platform} || $ENV{PLATFORM})
    {
        die(
            "the environment variable 'Platform' should not be set when building Unity. This is a problem with some hp-branded windows PCs which have the variable set by default. Remove it in your system properties."
        );
    }

    system($xbuild, $sln, "/t:Rebuild", "/property:Configuration=$config", "/verbosity:quiet", "/property:CscToolExe=$mcs") eq 0
        or croak("Failing xbuilding $sln");

    if (defined $targetdir)
    {
        for my $file (@producedfiles)
        {
            print "trying to copy $slndir/$file to $targetdir\n";
            UCopy("$slndir/$file", $targetdir);
        }
    }
}

sub XBuild4
{
    my ($root, $sln, $config, $targetdir, $platform) = @_;
    my $xbuild = "$root/External/MonoBleedingEdge/builds/monodistribution/bin/cli";
    my $xbuildExe = "$root/External/MonoBleedingEdge/builds/monodistribution/lib/mono/4.5/xbuild.exe";
    my $mcs = "mcs";

    if ($^O eq 'MSWin32')
    {
        $mcs = "mcs.bat";
    }

    if ($ENV{Platform} || $ENV{PLATFORM})
    {
        die(
            "the environment variable 'Platform' should not be set when building Unity. This is a problem with some hp-branded windows PCs which have the variable set by default. Remove it in your system properties."
        );
    }
    system($xbuild, $xbuildExe, $sln, "/t:Build", "/property:Configuration=$config", "/verbosity:quiet", "/property:CscToolExe=$mcs") eq 0
        or croak("Failing xbuilding $sln");
}

sub JamRaw
{
    my ($root, @args) = @_;
    my $jam = $root . "/jam.pl";
    system("perl", $jam, @args) eq 0 or croak("Failed to jam '@args'.");
}

sub Jam
{
    my $root = shift();
    my $targetname = shift();
    my $codegen = shift();
    my $platform = shift();
    my $incremental = shift();
    my @additionalargs = grep { /\S/ } @_;

    my $config = lc($codegen);

    my $jam = "$root/jam.pl";

    my @args = ("-q");
    if (length $baseLogPath > 0)
    {
        push(@args, "--logpath=$baseLogPath");
    }
    if ($config ne "")
    {
        push(@args, "-sCONFIG=$config");
    }
    if ($platform ne "")
    {

        if ($platform eq '<default>')
        {
            if ($^O eq 'MSWin32')
            {
                $platform = "win64";
            }
            elsif ($^O eq 'darwin')
            {
                $platform = "macosx64";
            }
            elsif ($^O eq 'linux')
            {
                $platform = "linux64";
            }
            else
            {
                die "Unsupported platform: $^O";
            }
        }

        push(@args, "-sPLATFORM=$platform");
    }

    if (ShouldRunNativeTests())
    {
        push(@args, "-sUNITY_RUN_NATIVE_TESTS_DURING_BUILD=1");
    }

    @args = (@args, @additionalargs);

    if ($incremental eq 0)
    {
        unshift(@args, '-a');
    }
    elsif ($incremental ne 1)
    {
        confess "Jam command can not be used without explicit incremental flag. It's very important to be correct here!\nincremental: $incremental\n";
    }

    if ($targetname ne "")
    {
        push(@args, $targetname);
    }

    system("perl", $jam, @args) eq 0 or die("Failed to jam '$targetname' using '$jam'");
}

# The one universal copy method for all platform which actually freaking works (tm)
sub UCopy
{
    my ($source, $destination) = @_;

    if ($^O eq "darwin" || $^O eq "linux")
    {
        if ($source =~ /\*$/)
        {
            my @files = ();
            foreach (glob($source))
            {
                push @files, $_;
            }
            if (@files == 0)
            {
                croak("No source files found for '$source'\n");
            }
            my ($name, $path, $suffix) = fileparse($destination);
            if (not -d $path)
            {
                mkpath($path);
            }
            foreach (@files)
            {
                system("cp", "-LRfp", $_, $destination) && croak("Failed to copy $_ to $destination");
            }
        }
        else
        {
            # Handle the special case where the target file is a hard link of the source file
            if (realpath(File::Spec->rel2abs($source)) ne realpath(File::Spec->rel2abs($destination)))
            {
                my $sourceStat = stat($source);
                my $destinationStat = stat($destination);
                if (defined($sourceStat) && defined($destinationStat) && $sourceStat->ino == $destinationStat->ino)
                {
                    unlink $destination;
                }
            }
            my ($name, $path, $suffix) = fileparse($destination);
            if (not -d $path)
            {
                mkpath($path);
            }
            system("cp", "-LRfp", $source, $destination) && croak("Failed to copy $source to $destination");
        }
    }
    elsif ($^O eq "MSWin32")
    {
        # Replace slashes with backslashes.
        $source =~ s/\//\\/g;
        $destination =~ s/\//\\/g;

        # If we're copying a file rather than a directory, create the
        # target path.
        if (not -d $source)
        {
            my ($name, $path, $suffix) = fileparse($destination);
            if (not -d $path)
            {
                $path =~ s/\\$//g;
                mkpath($path);
            }
        }
        else
        {
            mkpath($destination);
        }

        if (-d $source)
        {
            my $strippedSource = $source;
            $strippedSource =~ s/\\$//g;
            $destination =~ s/\\?$//g;
            system("robocopy", "/E", "/NJH", "/NJS", "/NDL", "/NC", "/NS", "/NP", "/NFL", $strippedSource, $destination)
                or system("xcopy", "/y", "/E", $source, $destination)
                or croak("Failed to copy directory $source to $destination");
        }
        else
        {
            system("copy", "/y", $source, $destination) && croak("Failed to copy $source to $destination");
        }
    }
    else
    {
        croak("Unsupported platform");
    }
    return $?;
}

sub UMove
{
    my ($source, $destination) = @_;
    if ($^O eq "MSWin32")
    {
        $source =~ s/\//\\/g;
        $destination =~ s/\//\\/g;
    }
    if (not -d $source)
    {
        my ($name, $path, $suffix) = fileparse($destination);
        if (not -d $path)
        {
            $path =~ s/\\$//g;
            mkpath($path);
        }
    }
    else
    {
        mkpath($destination);
    }
    move($source, $destination) or croak("Failed to move $source to $destination");
    return 1;
}

sub SetupPlaybackEngines
{
    my ($root, $options, $playbackEnginePath) = @_;

    my $playbackEngines = $options->{playbackengines};

    print "PlaybackEngines: @$playbackEngines\n";

    mkdir $playbackEnginePath;

    for my $engineName (@$playbackEngines)
    {
        my ($enginePath, $engineNameCorrectCasing);
        $enginePath = "$root/build/$engineName";
        unless (-d $enginePath)
        {
            die("Did not find required playback engine $engineName at $enginePath.\n");
        }
        $engineNameCorrectCasing = basename($enginePath);

        ProgressMessage("Adding $engineNameCorrectCasing");
        print("Copying $enginePath to $playbackEnginePath/$engineNameCorrectCasing...\n");
        if (-d "$playbackEnginePath/$engineNameCorrectCasing")
        {
            rmtree("$playbackEnginePath/$engineNameCorrectCasing");
        }
        UCopy "$enginePath", "$playbackEnginePath/$engineNameCorrectCasing";
    }

    # If no playback engines are found give NSIS a message file to place instead
    # if ($noEngines)
#   {
#       warn "No playback engines found" if @$playbackEngines;
#       open my $messageFile, ">", "$playbackEnginePath/NoEnginesHere" or die ("Failed to create testfile1: $!");
#       print $messageFile "PlaybackEngines: @$playbackEngines";
#       close $messageFile;
#   }
}

# Recursive delete with retry and sleeping on failure
sub RmtreeSleepy
{
    my $targetPath = $_[0];

    # Windows7 machines are too slow for Perl; try/repeat a few times before failing..
    for (my $i = 0; -e $targetPath; $i = $i + 1)
    {
        if ($i == 5)
        {
            return 0;
        }
        print "erasing: $targetPath (try $i) \n";
        rmtree($targetPath, { error => \my $err });
        if (@$err)
        {
            sleep $i;
        }
    }
    return 1;
}

sub MkpathSleepy
{
    my $targetPath = $_[0];

    # Windows7 machines are too slow for Perl; try/repeat a few times before failing..
    for (my $i = 0; not -e $targetPath; $i = $i + 1)
    {
        if ($i == 5)
        {
            return 0;
        }
        print "mkpath: $targetPath (try $i) \n";
        mkpath($targetPath);
        sleep $i;
    }
    return 1;
}

# Read file into string.
sub ReadFile
{
    my ($path) = @_;

    local $/;
    open(FILE, $path) or return "";
    my $contents = <FILE>;
    close(FILE);

    return $contents;
}

sub EncryptFile
{
    my ($sourceFile, $destFile) = @_;
    EncryptOrDecrypt($sourceFile, $destFile, 1);
}

sub DecryptFile
{
    my ($sourceFile, $destFile) = @_;
    EncryptOrDecrypt($sourceFile, $destFile, 0);
}

sub DecryptAndReadFile
{
    my $sourceFile = shift();
    my (undef, $destFile) = tempfile();

    EncryptOrDecrypt($sourceFile, $destFile, 0);

    local $/;
    open("inputfile", "<" . $destFile) or croak("Could not read from decrypted file $destFile");
    my $content = <inputfile>;
    close "inputfile";

    unlink $destFile;
    return $content;
}

sub EncryptOrDecrypt
{
    my ($sourceFile, $destFile, $encrypt) = @_;

    my $key = $ENV{UNITY_KEY};

    if (!defined($key))
    {
        warn "No key found, files can't be encrypted/decrypted\n ";

        if (AmRunningOnBuildServer())
        {
            die "On a build machine code decryption must be performed";
        }

        return;
    }

    my $cipher = new Crypt::CBC($key, 'Rijndael_PP');

    open(my $sourceHandle, $sourceFile) or die("Failed to open $sourceFile for reading");

    my $processedData;
    if ($encrypt)
    {
        my $fileSize = -s $sourceFile;
        my $encodedData;
        while (read($sourceHandle, my $buffer, 60 * 57))
        {
            $encodedData .= encode_base64($buffer, "");
        }
        $processedData = $cipher->encrypt($encodedData);
    }
    else
    {
        my $data;
        while (read($sourceHandle, my $buffer, 60 * 16))
        {
            $data .= $buffer;
        }
        my $decryptedData = $cipher->decrypt($data);
        $processedData = decode_base64($decryptedData);
    }
    open(my $destHandle, ">$destFile") or die("Failed to open $destFile for writing");
    binmode($destHandle, ":raw");
    print $destHandle $processedData;

    close $sourceHandle;
    close $destHandle;
}

sub GetCertificateFilesPath()
{
    my $commonpath = "agent-files/common/certs";
    if ($^O eq 'MSWin32')
    {
        return "$ENV{USERPROFILE}/$commonpath";
    }
    else
    {
        return "$ENV{HOME}/$commonpath";
    }
}

sub SignJar($)
{
    my $file = shift();

    my $certpath = GetCertificateFilesPath();
    my $p12 = "$certpath/unitycert.p12";

    if (defined($ENV{UNITY_KEY}) and $ENV{UNITY_KEY} eq "")
    {
        warn "Skipping jar signing as no encryption key was found\n";
        return;
    }

    my $jarsigner = "jarsigner";
    my $java = $ENV{"JAVA_HOME"};
    $jarsigner = "$java/bin/jarsigner.exe" if ($^O eq "MSWin32");

    DecryptFile("${p12}_encrypted", $p12);
    my $unitycertpassword = DecryptAndReadFile("$certpath/unitycert.pwd_encrypted");
    my $result =
        system($jarsigner, "-storetype", "pkcs12", "-keystore", $p12, "-storepass", $unitycertpassword, '-tsa', 'http://timestamp.comodoca.com/rfc3161',
        $file, "UnityTechnologiesApS");
    unlink $p12;

    croak("Error while signing '$file': $?") if $result != 0;
}

# Make sure we're using the correct mono for the current build system
sub SetupMono($)
{
    my $platform = shift();
    if ($platform =~ /^linux(32|64|-armel)$/)
    {
        # TODO: Fix builds.zip layout
        for my $variant ('Mono', 'MonoBleedingEdge')
        {
            # Continuation of ugly hack to workaround jam not being able to copy first one directory, then another
            system("cp", "-R", "External/$variant/builds/monodistribution/bin/cli", "External/$variant/builds/monodistribution/bin-$platform/");
            system("cp", "-R", "External/$variant/builds/monodistribution/bin/mono-env", "External/$variant/builds/monodistribution/bin-$platform/");
            system("cp", "-R", "External/$variant/builds/monodistribution/bin/monobin-env", "External/$variant/builds/monodistribution/bin-$platform/");

            system("chmod", "-R", "755", "External/$variant/builds/monodistribution/bin-$platform");
            system("cp", "-R", "External/$variant/builds/monodistribution/bin-$platform/mono", "External/$variant/builds/monodistribution/bin/");
            system("cp", "-R", "External/$variant/builds/monodistribution/bin-$platform/pedump", "External/$variant/builds/monodistribution/bin/");
            system("cp", "-R", "External/$variant/builds/monodistribution/etc-$platform/mono/config", "External/$variant/builds/monodistribution/etc/mono/");
        }
    }
}

sub IsWindows8OrNewer
{
    my $win8orNewer = 0;
    my @version = Win32::GetOSVersion();
    my $major = $version[1];
    my $minor = $version[2];
    $win8orNewer = 1 if (($major > 6) || (($major == 6) && ($minor >= 2)));
    return $win8orNewer;
}

sub IsVisualStudio2015Available
{
    if ($^O eq 'MSWin32')
    {
        my $vs2015Reg32;
        my $vs2015Reg64;
        $::HKEY_LOCAL_MACHINE->Open('SOFTWARE\\Microsoft\\Microsoft SDKs\\Windows\\v10.0', $vs2015Reg32);
        $::HKEY_LOCAL_MACHINE->Open('SOFTWARE\\Wow6432Node\\Microsoft\\Microsoft SDKs\\Windows\\v10.0', $vs2015Reg64);
        if ((not $vs2015Reg32) and (not $vs2015Reg64))
        {
            return 0;
        }
        else
        {
            return 1;
        }
    }
    else
    {
        return 0;
    }
}

sub AmRunningOnBuildServer
{
    if ($buildenv eq 'farm')
    {
        return 1;
    }
    return $ENV{UNITY_THISISABUILDMACHINE};
}

sub ShouldRunNativeTests
{
    if (not exists $ENV{'UNITY_RUN_NATIVE_TESTS_DURING_BUILD'})
    {
        # We don't run unit tests during builds anymore as we now have dedicated configs. The run-during-builds
        # only works for some select platforms only anyway.
        return 0;
    }

    # Note: The UnifiedTestRunner revision_verifier.pl script uses the
    # presence of the word "ShouldRunNativeTests" in this file as a signal
    # that we support the UNITY_RUN_NATIVE_TESTS_DURING_BUILD environment
    # variable. If the word is missing, meaning we're working with an old
    # revision, this file is instead patched in the working copy to disable
    # the automatic tests.

    return $ENV{'UNITY_RUN_NATIVE_TESTS_DURING_BUILD'} != 0;
}

sub DeleteRecursive
{
    my ($dir, $pattern, $processFiles, $processDirs) = @_;

    local *DIR;
    opendir(DIR, $dir) or croak("Could not open directory $dir: $!");

    # Pattern consists of a potential list of patterns, separated by spaces.
    my @patterns = split(/\s/, $pattern);
    foreach my $glob (@patterns)
    {
        # Iterate through the resulting list of files
        foreach my $file (File::Glob::glob("$dir/$glob"))
        {
            # Skip directories if $processDirs is not set
            my $exists = -e $file;
            my $isDir = -d $file;
            my $isFile = ($exists && !$isDir);
            if ($exists && ($isDir && $processDirs) || ($isFile && $processFiles))
            {
                if ($isDir)
                {
                    rmtree($file) or croak("Could not delete $file: $!");
                }
                else
                {
                    unlink($file) or croak("Could not delete $file: $!");
                }
            }
        }
    }

    # Now, recursively go down into subdirectories
    while (defined(my $file = readdir(DIR)))
    {
        # Only recurse on directories, which do not start with '.', and skip symbolic links
        if (-d "$dir/$file" && !(-l "$dir/$file") && ($file !~ /^\.{1,2}$/))
        {
            DeleteRecursive("$dir/$file", $pattern, $processFiles, $processDirs);
        }
    }
}

sub ProgressMessage
{
    my $message = shift();
    TeamCityOnlyProgressMessage($message);
    print("\n==== $message ====\n\n");
}

sub TeamCityOnlyProgressMessage
{
    my $message = shift();
    if (AmRunningOnBuildServer())
    {
        print("##teamcity[progressMessage '" . EscapeForTeamCity($message) . "']");
    }
}

sub PackageAndSendResultsToBuildServer
{
    my ($directory, $package, $exclude) = @_;

    if (not AmRunningOnBuildServer())
    {
        return;
    }

    local $CWD = $directory;

    my $packagefile;
    my $options = "";

    # Place log in ReportedArtifacts to automatically get picked up by katana
    my $logPath = "$root/build/ReportedArtifacts";
    mkpath($logPath) if (not -d $logPath);
    my $merged_fh = IO::File->new(File::Spec->catfile($logPath, "zip_$package.log"), "w+");

    ProgressMessage("Zipping up build artifacts from $directory");
    if ($^O eq 'MSWin32')
    {
        $packagefile = "ZippedForTeamCity.zip";
        if ((defined($exclude)) and ($exclude ne ""))
        {
            $options .= "-x!*$exclude";
        }
    }
    else
    {
        $packagefile = "ZippedForTeamCity.tar.gz";
        if ((defined($exclude)) and ($exclude ne ""))
        {
            $options .= "--exclude=$exclude";
        }
    }

    if (-e "$directory/$packagefile")
    {
        ProgressMessage("Removing pre-existing $directory/$packagefile");
        unlink("$directory/$packagefile");
    }

    # Syntax from Tiny.pm regarding capture_merged with trailing options
    capture_merged
    {
        if ($^O eq 'MSWin32')
        {
            system("$root/External/7z/win32/7za.exe", "a", "-r", $packagefile, "*") == 0 or croak("Failed to zip up artifacts");
        }
        else
        {
            system("cd \"$directory\" && tar -pczf $packagefile $options *") == 0 or croak("Failed to zip up artifacts");
        }
    }
    stdout => $merged_fh, stderr => $merged_fh;
    close $merged_fh;

    print("##\[publishArtifacts '" . "$directory/$packagefile" . "'\]\n");
}

sub EscapeForTeamCity
{
    my $input = shift();
    $input =~ s/\|/!TEMPMARKER!/g;
    $input =~ s/'/\|'/g;
    $input =~ s/]/\|]/g;
    $input =~ s/\n/\|n/g;
    $input =~ s/\r/\|r/g;
    $input =~ s/!TEMPMARKER!/\|\|/g;
    return $input;
}

sub ReplaceText
{
    my ($filein, $fileout, %fields) = @_;

    my $data;
    {
        open(my $FH, $filein) or croak("FAILED: unable to open $filein for reading!");
        $data = join '', <$FH>;
    }
    my $re = join('|', reverse(sort (keys(%fields))));

    # Protect against anyone providing us with a hash that has un-initialized elements
    foreach (keys(%fields))
    {
        if (not defined($fields{$_}))
        {
            $fields{$_} = "";
        }
    }

    $data =~ s|($re)|$fields{$1}|ge;

    open(my $FH, ">$fileout") or croak("FAILED: unable to open $fileout for writing!");
    print($FH $data);
}

sub ConvertToLowercaseAndMatchByPrefix
{
    my ($prefix, $expected) = @_;

    # Convert to lowercase.
    $prefix = lc($prefix);

    # Construct pattern to match by prefix.
    my $pattern = qr/^$prefix/;

    return ($expected =~ $pattern);
}

sub TakeFirstElementIfPrefixMatch
{
    my ($refArgs, $expected) = @_;

    if (scalar(@{$refArgs}) == 0)
    {
        return 0;
    }

    if (!ConvertToLowercaseAndMatchByPrefix(${$refArgs}[0], $expected))
    {
        return 0;
    }

    shift(@{$refArgs});
    return 1;
}

sub TakeFirstElementIfAnyPrefixMatches
{
    my ($refArgs, $refExpected) = @_;

    foreach (@{$refExpected})
    {
        if (TakeFirstElementIfPrefixMatch($refArgs, $_))
        {
            return $_;
        }
    }

    return 0;
}

sub HasAnyElementThatIsPrefixMatch
{
    my ($refArray, $expected) = @_;

    foreach (@{$refArray})
    {
        if (ConvertToLowercaseAndMatchByPrefix($_, $expected))
        {
            return 1;
        }
    }

    return 0;
}

sub OpenBrowser
{
    my $url = shift;
    my $platform = $^O;
    my $cmd;

    if ($platform eq 'darwin') { $cmd = "open \"$url\""; }    # Mac OS X
    elsif ($platform eq 'linux') { $cmd = "xdg-open \"$url\""; }    # Linux
    elsif ($platform eq 'MSWin32') { $cmd = "start explorer \"$url\""; }    # Win95..Win7
    if (defined $cmd)
    {
        system($cmd);
    }
    else
    {
        die "Can't locate default browser";
    }
}

sub LaunchExecutableAndExit
{
    my ($executablePath) = @_;

    if ($^O eq 'darwin')
    {
        exec("open $executablePath");
    }
    else
    {
        exec("$executablePath");
    }
}

# Dumps a binary file /foo/bar/baz.bin to /foo/bar/baz.h as:
# #pragma once
# const unsigned char kbaz [] = { 1, 1, 2, 3, 5, 8, ... };
# where each element in kbaz is a byte from baz.bin
#
# This is primarily for embedding public keys for plugin validation.
#
# The passed parameter is the binary file to headerize.
sub DumpBinaryToByteArray
{
    my $file = shift;

    my $basename = $file;
    $basename =~ s/\.[^\.]+$//;    # Strip file extension
    my $header = "$basename.h";

    $basename =~ s/[^\w\d\/\\]/_/g;    # Replace non-alphanumeric with underscore
    my $variable = $basename;
    $variable =~ s/.*[\/\\]//;         # Get base filename

    local $/ = undef;                  # Read EVERYTHING

    # Read input
    open(INFILE, '<:raw', $file) or croak("Error opening $file:$!");
    my $binaryContent = <INFILE>;
    close(INFILE);

    # Write prettified output
    open(OUTFILE, '>', $header) or croak("Error opening $header:$!");
    print(OUTFILE "//THIS IS A GENERATED FILE; DO NOT MODIFY\n#pragma once\nconst unsigned char k$variable [] = { ",
        join(', ', unpack('C*', $binaryContent)), '};');
    close(OUTFILE);
}

# extractArtifact function
# - takes in a string representing file path to the artifact we want to extract and the destination we want to
#   extract it to, also takes an optional argument that specifies if the function returns the content of the
#   supplied artifact as a string
sub extractArtifact
{
    my ($filePath, $outputPath, $returnContent) = @_;

    my $fileList;
    my @suffixes = ('.zip', '.tar.gz');

    my ($name, $dirs, $suffix) = fileparse("$filePath", @suffixes);

    my @options = ("yes", "silent");

    if ($suffix eq ".zip" and $^O eq "MSWin32")
    {
        $fileList = WinTools::extractArtifactOnWindows($filePath, "content_listing");
        WinTools::extractArtifactOnWindows($filePath, "extract", "$outputPath", \@options);
    }
    elsif ($suffix eq ".tar.gz" and $^O eq "MSWin32")
    {
        WinTools::extractArtifactOnWindows($filePath, "extract", "$outputPath", \@options);
        my $filePathTemp = substr($filePath, 0, index($filePath, ".gz"));
        $fileList = WinTools::extractArtifactOnWindows($filePathTemp, "content_listing");
        WinTools::extractArtifactOnWindows($filePathTemp, "extract", "$outputPath", \@options);

        # this removes the temporary tar created
        unlink($filePathTemp);
    }
    elsif ($suffix eq ".zip" and $^O ne "MSWin32")
    {
        $fileList = `unzip -l $filePath`;
        system("unzip -o -q $filePath -d $outputPath") and die("Unable to extract $filePath to $outputPath");
    }
    else
    {
        $fileList = `tar -tvf $filePath`;
        system("tar -zxf $filePath -C $outputPath") and die("Unable to extract $filePath to $outputPath");
    }

    if ($returnContent == 1)
    {
        return $fileList;
    }
}

# Generate bundle file report (CSV)
# permissions, size, name, etc
sub BuildBundleFileList($$)
{
    my ($bundle_path, $filelist_path) = @_;

    open(my $fh, ">$filelist_path") or die("Failed to open $filelist_path for writing\n");

    $fh->print("Permissions,Size,Name\n");
    my $dir = Path::Class::Dir->new($bundle_path);
    $dir->traverse(
        sub
        {
            my ($child, $cont, $indent) = @_;

            $indent //= 0;

            my $prn = $child;
            $prn =~ s/^\Q$bundle_path\E\/?//;

            if ($prn eq '')
            {
                $cont->($indent + 1);
                return;
            }

            my @st = CORE::stat($child);
            $prn =~ s/"/""/g;
            if ($prn =~ /[, ]/)
            {
                $prn = "\"$prn\"";
            }
            $fh->printf("%o,%d,$prn\n", $st[2], $st[7]);

            $cont->($indent + 1);
        }
    );

    close($fh) or die("Failed to close filehandle of $filelist_path\n");
}

sub GetMono64Exe
{
    if ($^O eq 'MSWin32')
    {
        # Use 64-mono on windows to prevent running out of memory. On Mac, MonoBleedingEdge defaults to 64-bit already.
        return "$root/External/MonoBleedingEdge/builds/monodistribution/bin-x64/mono";
    }
    else
    {
        return "$root/External/MonoBleedingEdge/builds/monodistribution/bin/cli";
    }
}

1;
