package Environments::Mono;

use parent Environment;

use warnings;
use strict;

use Dirs;
use SystemCall;
use File::Spec::Functions qw (canonpath);
use File::Basename;
use NUnitCommandLine;
use FileUtils qw (getDirName);
use lib Dirs::external_root();
use Array::Utils qw (unique);
use CSProj;
use VSSolution;
use FileUtils;
use Try::Tiny;
use English qw(-no_match_vars);

my $root = Dirs::getRoot();
my $monoPrefix = canonpath("$root/External/Mono/builds/monodistribution");
my $utrRoot = Dirs::UTR_root();
my $mono = canonpath("$utrRoot/mono");

sub new
{
    my ($pkg) = @_;
    my $instance = $pkg->SUPER::new();
    return bless($instance, $pkg);
}

sub getName
{
    my ($this) = @_;
    return 'mono';
}

sub runDotNetProgram
{
    my ($this, %args) = @_;
    try
    {
        my $program = $args{program};
        _initMonoVars(getDirName($program));
        my @monoArgs = _getMonoArgs(%args);
        my @programArgs = _getProgramArgs(%args);
        return $this->{systemCall}->executeAndWatchFiles(
            command => [$mono, @monoArgs, $program, @programArgs],
            filesToWatch => $args{filesToWatch}
        );
    }
    finally
    {
        _resetMonoVars();
    };
}

sub runNUnitProject
{
    my ($this, %args) = @_;
    try
    {
        my @dlls;
        if ($args{dlls})
        {
            @dlls = @{ $args{dlls} };
        }

        _initMonoVars(_getDirs(@dlls));

        my @monoArgs = _getMonoArgs(%args);
        my @nunitArgs = _getNUnitArgs(%args);

        $args{directRunnerArgs} = \@nunitArgs;
        my @nunitcommandLine = NUnitCommandLine::build(%args);

        return $this->{systemCall}->executeAndWatchFiles(
            command => [$mono, @monoArgs, @nunitcommandLine],
            filesToWatch => $args{filesToWatch}
        );
    }
    finally
    {
        _resetMonoVars();
    };
}

sub _initMonoVars
{
    ## no critic (RequireLocalizedPunctuationVars)
    my (@dirs) = @_;
    $ENV{'MONO_PATH'} = _buildMonoPath(@dirs);
    $ENV{'MONO_CFG_DIR'} = canonpath("$monoPrefix/etc");
    $ENV{'LD_LIBRARY_PATH'} = canonpath("$monoPrefix/lib");
}

sub _resetMonoVars
{
    delete $ENV{'MONO_PATH'};
    delete $ENV{'MONO_CFG_DIR'};
    delete $ENV{'LD_LIBRARY_PATH'};
}

sub _buildMonoPath
{
    my @dirs = @_;
    my @pathes;
    if (@dirs)
    {
        push(@pathes, @dirs);
    }
    push(@pathes, canonpath("$monoPrefix/lib/mono/2.0"));
    my $path_separator = _getMonoPathSepartor();
    return join($path_separator, @pathes);
}

sub _getMonoPathSepartor
{
    my $path_separator = ':';
    if ($OSNAME eq 'MSWin32')
    {
        $path_separator = ';';
    }
    return $path_separator;
}

sub _getMonoArgs
{
    my (%args) = @_;
    my @monoArgs = ('--debug');
    if ($args{monoArgs})
    {
        @monoArgs = @{ $args{monoArgs} };
    }
    return @monoArgs;
}

sub _getProgramArgs
{
    my (%args) = @_;
    my @programArgs;
    if ($args{programArgs})
    {
        @programArgs = @{ $args{programArgs} };
    }
    return @programArgs;
}

sub _getNUnitArgs
{
    my (%args) = @_;
    my @directRunnerArgs = ('-domain=None');
    if (defined($args{directRunnerArgs}))
    {
        @directRunnerArgs = @{ $args{directRunnerArgs} };
    }
    return @directRunnerArgs;
}

sub _getDirs
{
    my (@filenames) = @_;
    my @results;
    foreach my $file (@filenames)
    {
        if ($file =~ m/csproj$/is)
        {
            my $csproj = new CSProj($file);
            my $dllPath = $csproj->getAssemblyFullPath();
            if (defined($dllPath))
            {
                push(@results, FileUtils::getDirName($dllPath));
            }
        }

        if ($file =~ m/dll$/s)
        {
            my $path = getDirName($file);
            push(@results, $path);
        }

        if ($file =~ m/sln$/is)
        {
            my $sln = new VSSolution($file);
            my @dlls = $sln->getAllClassLibraries();
            my @pathes = map { FileUtils::getDirName($_->getAssemblyFullPath()) } @dlls;
            push(@results, @pathes);
        }
    }
    my %seen;
    my @unique = grep { !$seen{$_}++ } @results;
    return @unique;
}

1;
