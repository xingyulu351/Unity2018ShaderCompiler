package BuildEngines::XBuild;

use parent Builder;

use warnings;
use strict;

use Carp qw (croak);
use File::Spec::Functions qw (canonpath);
use SystemCall;
use Dirs;
use English qw( -no_match_vars );
use Logger;

my $root = Dirs::getRoot();

sub new
{
    my ($pkg) = @_;
    my $instance = $pkg->SUPER::new();
    return bless($instance, $pkg);
}

sub getName
{
    return 'xbuild';
}

sub clean
{
    my ($this, $project) = @_;
    Logger::minimal("Cleaning $project");
    my $xbuild = $this->getXBuildPath();
    my @args = ();
    push(@args, $xbuild);
    push(@args, canonpath($project));
    push(@args, "/target:Clean");
    push(@args, "/noconsolelogger");
    push(@args, "/nologo");
    return $this->executeAndWatchFiles(command => \@args);
}

sub build
{
    my ($this, %args) = @_;
    my $config = $args{'config'};
    my $rebuild = $args{'rebuild'};
    my $logfile = $args{'logfile'};
    my $project = canonpath($args{'project'});
    my $projectArgs = $args{'project_args'};
    my $framework = $args{'framework'};
    my $xbuild = $this->getXBuildPath($framework);
    my $outputPath = $args{'outputPath'};

    Logger::minimal("Building $project");
    my @args;
    push(@args, $xbuild);
    push(@args, $project);
    if ($rebuild)
    {
        push(@args, "/target:Rebuild");
    }
    else
    {
        push(@args, "/target:Build");
    }
    push(@args, "/property:Configuration=$config");
    push(@args, "/verbosity:quiet");
    push(@args, "/nologo");
    if ($^O eq 'MSWin32')
    {
        push(@args, "/property:CscToolExe=mcs.bat");
    }
    else
    {
        push(@args, "/property:CscToolExe=mcs");
    }
    push(@args, "/property:BuildInParallel=false");

    if (defined($projectArgs))
    {
        push(@args, $projectArgs);
    }

    if (defined($framework))
    {
        push(@args, "/property:TargetFrameworkVersion=$framework");
    }

    if (defined($outputPath))
    {
        push(@args, "/property:OutputPath=$outputPath");
        my $intermPath = canonpath("$outputPath/Intermediate") . canonpath("/");
        push(@args, "/property:BaseIntermediateOutputPath=$intermPath");
    }

    my $filesToWatch = [];
    if ($logfile)
    {
        $filesToWatch = [{ file => $logfile, callback => \&Plugin::printLogMessage, callbackArg => $this }];
        push(@args, "1>$logfile");
    }

    my $exitCode = $this->executeAndWatchFiles(command => \@args, filesToWatch => $filesToWatch);
    if ($exitCode != 0)
    {
        $this->dumpBuildLogAndCroak($project, $logfile);
    }
}

sub getXBuildPath
{
    my ($this, $framework) = @_;
    if (defined($framework) and $framework eq '4.5')
    {
        return canonpath("$root/External/MonoBleedingEdge/builds/monodistribution/lib/mono/4.5/xbuild.exe");
    }
    return canonpath("$root/External/MonoBleedingEdge/builds/monodistribution/bin/xbuild");
}

1;
