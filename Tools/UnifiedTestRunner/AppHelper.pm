package AppHelper;

use warnings;
use strict;

use GlobalOptions;
use Options;
use SuiteRunners;
use ProfileArgsReader;
use Environments::MonoBleedingEdge;
use Environments::Mono;
use Environments::Win;
use File::Spec::Functions qw (catfile catdir);
use RunContext;
use SingleSuiteRunnerArtifactsNameBuilder;
use Dirs;
use lib Dirs::external_root();
use JSON;
use Carp qw(croak);
use JSON;

sub parseOptions
{
    my (@commadLine) = @_;
    my @globalOptions = GlobalOptions::getOptions();
    my $globalOptions = new Options(options => \@globalOptions);
    my $opts = new Options(options => \@globalOptions);

    my @expandedCommandLine = @commadLine;
    $opts->parse(@expandedCommandLine);

    my $playerLoadPath = $opts->getOptionValue('player-load-path');
    if ($playerLoadPath)
    {
        @expandedCommandLine = processPlayerLoadPathArgs($playerLoadPath, $opts, \@expandedCommandLine);
    }
    my $profile = $opts->getOptionValue('profile');
    if ($profile)
    {
        @expandedCommandLine = proccessProfileArgs($profile, \@expandedCommandLine);
    }

    my $hiddenArgs = $ENV{'UTR_HIDDEN_ARGS'};
    if ($hiddenArgs)
    {
        my $hiddenArgs = jsonToObj($hiddenArgs);
        push(@expandedCommandLine, @{$hiddenArgs});
    }

    $opts->parse(@expandedCommandLine);

    my @selectedSuiteNames = getSelectedSuiteNames($opts);
    if (scalar(@selectedSuiteNames) == 0)
    {
        my $result = {
            options => $opts,
            expandedCommandLine => \@expandedCommandLine
        };
        return $result;
    }

    my @runners = createRunners($opts);
    my @allOpts = (GlobalOptions::getOptions());
    foreach my $runner (@runners)
    {
        my $suiteOptions = $runner->getSuiteOptions();
        push(@allOpts, @{$suiteOptions});
    }

    $opts = new Options(options => \@allOpts);
    $opts->parse(@expandedCommandLine);

    # TODO: remove this hack, after options system refactoring,
    # so that run context is not needed to resolve options
    my $tmpContext = RunContext::new($opts);
    @allOpts = (GlobalOptions::getOptions());
    foreach my $runner (@runners)
    {
        $runner->setRunContext($tmpContext);
        my $suiteOptions = $runner->getOptions();
        push(@allOpts, @{$suiteOptions});
    }

    $opts = new Options(options => \@allOpts);
    $opts->parse(@expandedCommandLine);
    my $result = {
        options => $opts,
        expandedCommandLine => \@expandedCommandLine
    };

    return $result;
}

sub getSelectedSuiteNames
{
    my ($opts) = @_;
    my $selectedSuiteNames = $opts->getOptionValue('suite');
    if (not $selectedSuiteNames)
    {
        return ();
    }
    return @{$selectedSuiteNames};
}

sub createRunners
{
    my ($options) = @_;
    my @selectedSuiteNames = getSelectedSuiteNames($options);
    my @activeRunners = getRunners(@selectedSuiteNames);
    my $runContext = RunContext::new($options);
    my $environment = createEnvironment($options);
    foreach my $runner (@activeRunners)
    {
        $runner->setRunContext($runContext);
        $runner->setEnvironment($environment, $runContext);
        $runner->setArtifactsNameBuilder(new SingleSuiteRunnerArtifactsNameBuilder($runner));
        $runner->setRunContext($runContext);
    }
    return @activeRunners;
}

sub getRunners
{
    my (@names) = @_;
    my @runners = ();
    foreach my $suite (@names)
    {
        my $runner = SuiteRunners::getRunner($suite);
        if (not defined($runner))
        {
            Logger::error("Unknown name of test suite: '$suite'");
        }
        else
        {
            push(@runners, $runner);
        }
    }
    return @runners;
}

sub proccessProfileArgs
{
    my ($profile, $cmdArgs) = @_;
    my @args = getArgsForProfile($profile);
    @args = grep { $_ !~ qr/profile=/ } @args;
    @{$cmdArgs} = grep { $_ !~ qr/profile=/ } @{$cmdArgs};

    push(@{$cmdArgs}, @args);
    return @{$cmdArgs};
}

sub loadSuiteInfoPlayerLoadPath
{
    my ($fileName) = @_;
    if (not -e $fileName)
    {
        return {};
    }
    my $text = join(q (), FileUtils::readAllLines($fileName));
    return jsonToObj($text);
}

sub getArgsForProfile
{
    my ($profile) = @_;
    my $fullProfilePath = ProfileArgsReader::resolveProfilePath($profile);
    open(my $fh, '<', $fullProfilePath);
    my @args = ProfileArgsReader::read($fh);

    close($fh);
    return @args;
}

sub processPlayerLoadPathArgs
{
    my ($playerLoadPath, $options, $commadLine) = @_;
    my @expandedCommandLine = @{$commadLine};
    my $suiteInfoFile = catfile($playerLoadPath, ".test-run-info");
    my $suiteInfo = loadSuiteInfoPlayerLoadPath($suiteInfoFile);
    if (not $suiteInfo->{suite})
    {
        croak("File $suiteInfoFile does not contain information about test suite. Exiting.");
        return;
    }

    my $suite = $options->getOptionValue('suite');
    if (not defined($suite))
    {    # --suite is not specified in the command line - add it explicitly
        push(@expandedCommandLine, "--suite=$suiteInfo->{suite}");
    }

    my $platform = $options->getOptionValue('platform');
    if (not defined($platform))
    {
        push(@expandedCommandLine, "--platform=$suiteInfo->{platform}");
    }
    return @expandedCommandLine;
}

sub createEnvironment
{
    my ($options) = @_;
    my $env = undef;
    my $buildEngine = undef;
    my $envName = $options->getOptionValue('environment');
    if (not defined($envName))
    {
        $envName = 'monobe';
    }

    if ($envName eq 'monobe')
    {
        $env = Environments::MonoBleedingEdge->new();
    }
    elsif ($envName eq 'mono')
    {
        $env = Environments::Mono->new();
    }
    elsif ($^O eq 'MSWin32' and $envName eq 'win')
    {
        $env = Environments::Win->new();
    }
    else
    {
        croak("$envName is not supported for $^O");
    }

    return $env;
}

1;
