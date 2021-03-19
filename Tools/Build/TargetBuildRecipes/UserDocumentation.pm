package UserDocumentation;

use strict;
use warnings;
use File::Basename qw (dirname basename fileparse);
use File::Copy;
use File::Spec;
use File::Glob;
use File::Find;
use File::Path;
use Carp qw (croak carp);
use File::chdir;
use lib File::Spec->rel2abs(dirname($0) . "/..");
use MacTools qw (LinkFileOrDirectory);
use Tools qw (AmRunningOnBuildServer ProgressMessage UCopy OpenBrowser);
use Tools qw (PackageAndSendResultsToBuildServer);
use Tools qw (UCopy XBuild NewXBuild XBuild4);
use Tools qw (Jam JamRaw);
use Tools qw (GetMono64Exe);
use Repositories qw (ReposFetch);
use Cwd;
use Cwd 'abs_path';
use feature qw (say);
use base qw (Exporter);

#used from PlatformDependent/.../$platform.pm
@UserDocumentation::EXPORT = qw (BuildUserDocumentation);

my $root = File::Spec->rel2abs(dirname($0));
my $docToolsDir = "$root/Tools/DocTools";
my $documentationRepoFolder = "$root/Repositories/documentation";

#turn the following off for debugging
my $processManual = 1;
my $processScriptRef = 1;
my $processExamples = 1;

sub Register
{
    Main::RegisterTarget(
        "UserDocumentation",
        {
            windowsbuildfunction => \&BuildUserDocumentation_Default,
            macbuildfunction => \&BuildUserDocumentation_Default,
            options => {
                incremental => [0, 1],
                platform => [],
            },
            dependencies => [],
        }
    );

    Main::RegisterTarget(
        "DocsBuildVerification",
        {
            windowsbuildfunction => \&DocsBuildVerification,
            macbuildfunction => \&DocsBuildVerification,
            options => {},
            dependencies => ["UserDocumentation"]
        }
    );
}

sub UnTGZPackages
{
    my ($unityAssemblyPath) = @_;

    my $SevenZip;
    if ($^O eq 'MSWin32')
    {
        $SevenZip = "$root/External/7z/win32/7za.exe";
    }
    elsif ($^O eq "darwin")
    {
        $SevenZip = "$root/External/7z/osx/7za";
    }
    elsif ($^O eq 'linux')
    {
        $SevenZip = "$root/External/7z/linux64/7za";
    }
    else
    {
        die("Unsupported platform: $^O\n");
    }

    my @files;

    my $wanted = sub
    {
        my $filename = $File::Find::name;
        $filename =~ tr/\\/\//;
        if ($filename =~ /Resources\/PackageManager\/Editor\/.*\.tgz$/)
        {
            push @files, $filename;
        }
    };

    find({ wanted => $wanted }, $unityAssemblyPath ? $unityAssemblyPath : "$root/build");

    my $packagetemp;
    foreach my $filename (@files)
    {
        my $dir = dirname($filename);
        my $tarname = $filename;
        $tarname =~ s/\.tgz$/\.tar/;
        my $filebasename = basename($filename);
        my $outputdir = "$root/build/temp/docs_untgz/$filebasename";
        if (!$packagetemp)
        {
            $packagetemp = File::Spec->rel2abs(dirname($outputdir));
        }

        $outputdir =~ s/\.tgz$//;

        print("Extracting to: $outputdir\n");
        mkpath($outputdir);
        my @cmdUnGzip = ($SevenZip, 'x', $filename, "-o$dir");
        map { print "$_ " } (@cmdUnGzip, "\n");
        system(@cmdUnGzip) and die("Failed to ungzip $filename\n");

        my @cmdUnTar = ($SevenZip, 'x', '-y', '-ttar', $tarname, "-o$outputdir");
        map { print "$_ " } (@cmdUnTar, "\n");
        system(@cmdUnTar) and die("Failed to untar $tarname\n");

        unlink($tarname);
    }

    return $packagetemp;
}

sub BuildUserDocumentation_MonoDoc
{
    my ($unityAssemblyPath) = @_;

    my $packagetemp = UnTGZPackages($unityAssemblyPath);

    Jam($root, "DocGen", "", "", 1);
    local $CWD = $docToolsDir;
    my $cli = GetMono64Exe();
    my $exe = "APIDocumentationGenerator/bin/APIDocumentationGenerator.exe";
    my $failureMessage = 'Failed generating MonoDoc XML files';

    if ($^O eq 'linux')
    {
        # DocTools forces cecil to hold file handles for each scanned assembly - currently there are 1122
        system("ulimit -Sn 4096; \"$cli\" \"$exe\" -monodoc -folder \"$unityAssemblyPath\"") and croak($failureMessage);
    }
    else
    {
        my @monoargs = ($cli, $exe, "-monodoc", "-folder", "$unityAssemblyPath");
        system(@monoargs) == 0 or croak($failureMessage);
    }

    if (-d $packagetemp)
    {
        rmtree($packagetemp) or die("Unable to remove $packagetemp (another process has a lock on it?)\n");
    }
}

sub BuildUserDocumentation_Default
{
    my ($root, $targetPath, $options) = @_;
    BuildUserDocumentation($root, $targetPath, $options, "default", "default", "doctools.conf,doctools-platform-default.conf");
}

sub BuildUserDocumentation
{
    my ($root, $targetPath, $options, $platformName, $playbackEngineName, $userDocsConfig) = @_;

    if ($processManual)
    {
        rmtree($targetPath) or die("Unable to remove $targetPath (another process has a lock on it?)\n");
        mkpath($targetPath);

        BuildUserDocsManual($root, $targetPath, $options, $playbackEngineName, $userDocsConfig);
    }
    if ($processScriptRef)
    {
        BuildUserDocsScriptingReference($root, $targetPath, $options, $platformName, $playbackEngineName);
    }

    # DocCombiner
    if ($^O eq 'MSWin32')
    {
        ProgressMessage("Making DocCombiner");
        my $incremental = ($options->{incremental} or 1);
        Jam($root, 'DocCombiner', $options->{codegen}, 'win64', $incremental);
        my $doccombiner = "$root/build/WindowsDocCombiner/DocCombiner.exe";

        system($doccombiner, "-verify $targetPath/Documentation") and die("DocCombiner json verification failed\n");
    }

    # Switch links generation
    chdir($docToolsDir) or die "Could not switch directory to $docToolsDir";

    if ($platformName eq "default" and $processScriptRef and $processManual)
    {
        system("perl", "GenerateDocSwitchLinks.pl") && croak("GenerateDocSwitchLinks failed");
        system("perl", "ValidateSwitchLinks.pl") && croak("Switch link validation failed");
    }

    # Wrap up
    copy("$root/Tools/WikiDocs/Documentation.html", "$targetPath/ActualDocumentation/Documentation.html");

    PackageAndSendResultsToBuildServer($targetPath, "UserDocumentation");
    chdir $root;
}

sub BuildUserDocsManual
{
    my ($root, $targetPath, $options, $playbackEngineName, $userDocsConfig) = @_;

    if (!-e "$documentationRepoFolder")
    {
        die("Documentation tools must be checked out into $documentationRepoFolder\n");
    }

    chdir("$documentationRepoFolder/tools/doctools");
    rmtree("temp");

    my $fast = '';
    if (!AmRunningOnBuildServer())
    {
        $fast = '--no-optimize-images';
    }

    my $args = $userDocsConfig;
    if (defined $args && length($args))
    {
        print("Calling unity-docs build.pl with args:\n$args\n");
        system("perl", "build.pl", "--config=$args", $fast) && die("Failed to run docs build 'build.pl --config=$args'");
    }
    else
    {
        system("perl", "build.pl", $fast) && die("Failed to run docs build 'build.pl'");
    }

    my $outputHtmlFolder = "$documentationRepoFolder/tools/doctools/temp/$playbackEngineName/Documentation";

    my $targetHtmlFolder = "$targetPath/Documentation";
    if ($^O eq "darwin" || $^O eq "linux")
    {
        $targetHtmlFolder = "$targetPath/";
    }

    print("Copying output HTML from $outputHtmlFolder to $targetHtmlFolder\n");
    mkpath($targetHtmlFolder);
    UCopy($outputHtmlFolder, $targetHtmlFolder);
}

sub BuildUserDocsScriptingReference
{
    my $packagetemp = UnTGZPackages();

    my ($root, $targetPath, $options, $platformName) = @_;

    Jam($root, "DocGen", "", "", 1);
    (chdir "$docToolsDir") == 1 or die "Could not change directory to $docToolsDir";
    my $cli = GetMono64Exe();
    my $exe = "APIDocumentationGenerator/bin/APIDocumentationGenerator.exe";

    if ($platformName ne "default")
    {
        system($cli, $exe, "-noscandlls", "-platform", "$platformName") == 0 or croak("Failed running mono $exe");
    }
    else
    {
        if ($processExamples)
        {
            system($cli, $exe) == 0 or croak("Failed running mono $exe");
        }
        else
        {
            system($cli, $exe, "-noexamples") == 0 or croak("Failed running mono $exe");
        }
    }

    if (-d $packagetemp)
    {
        rmtree($packagetemp) or die("Unable to remove $packagetemp (another process has a lock on it?)\n");
    }
}

sub DocsBuildVerification
{
    my ($root, $targetPath, $options) = @_;
    {
        local $CWD = "$documentationRepoFolder/tools/tests";
        system("perl", "ADocsVerification.pl") && die("Failed to run 'ADocsVerification.pl'");
    }

    my $dbvDir = "$root/build/DocsBuildVerification";
    rmtree($dbvDir) or die "Cannot rmtree '$dbvDir' : $!";

    # Copy documentation files (Katana prefers to pick up artifacts from one folder)
    my $artifacts_folder = "$root/build/UserDocumentation/Documentation/";    # ending backslash is required for UCopy to work on both Mac and Windows
    my $output_folder = "$dbvDir/Documentation/";
    print("Copying docs from '$artifacts_folder' to '$output_folder'...\n");
    UCopy($artifacts_folder, "$output_folder");
    WriteDocumentationHTML($dbvDir);

    # Run link validation
    (chdir $docToolsDir) == 1 or die "Could not switch directory to $docToolsDir";
    my $output_folder_relative = "../../build/DocsBuildVerification/Linklint";
    my $output_folder_absolute = "$dbvDir/Linklint";
    $artifacts_folder = "$dbvDir/Documentation";
    mkdir($output_folder_absolute);
    my @args = (
        "perl", "linklint/linklint-2.3.5.pl", "\@linklint/ADocsVerification.args",
        "-quiet", "/@", "-doc", $output_folder_relative, "-root", "$artifacts_folder/en", "-docbase", "../Documentation/en"
    );
    if ($^O eq "MSWin32")
    {
        push(@args, "-case");
    }
    print("Running '@args'...\n");

    my $result = system(@args);

    if ($result != 0)
    {
        if (AmRunningOnBuildServer())
        {
            say("See the artifact DocsBuildVerification, path /DocsBuildVerification/Linklint/errorX.html");
            warn("Link verification failed\n");
        }
        else
        {
            OpenBrowser("file://$output_folder_absolute/index.html");
            croak("Link verification failed");
        }
    }
    else
    {
        print "Link verification succeeded\n";
    }
}

sub WriteDocumentationHTML
{
    my $unityAssemblyPath = shift;

    my $rootdoc = <<END;
<!DOCTYPE html><html lang="en" class="no-js">
<head>
<script type="text/javascript">
window.location = "Documentation/en/Manual/index.html"
</script>
</head>
<body></body>
</html>
END

    my $filepath = "$unityAssemblyPath/Documentation.html";
    open(my $rootdocfile, ">$filepath") or die "Failed to open $filepath\n";
    print $rootdocfile ($rootdoc);
    close($rootdocfile);
}

1;
