package SuiteTools::Inspxe;

use parent Plugin;

use TargetResolver;
use File::Basename;

use warnings;
use strict;
use SuiteRunnerArtifactsNameBuilder;

my $suiteOptions = [
    [
        'analysis-type=s',
        'Analysis type',
        {
            allowedValues => [
                ['mi1', 'Detect Leaks'],
                ['mi2', 'Detect Memory Problems'],
                ['mi3', 'Locate Memory Problems'],
                ['ti1', 'Detect Deadlocks'],
                ['ti2', 'Detect Deadlocks and Data Races'],
                ['ti3', 'Locate Deadlocks and Data Races']
            ],
        }
    ],
    ['suppression-file:s', 'Text file contains definitions of suppression rules'],
    ['result-dir:s', 'Identify an alternative location for finding/storing result data'],
];

sub new
{
    my ($pkg) = @_;
    return $pkg->SUPER::new(suiteOptions => $suiteOptions);
}

sub usage
{
    my ($this) = @_;
    return (
        prerequisites => '1. Intel Inspector XE is installed. 2. SET INSPXEBIN=<PATH_TO_INTEL_INSPECTOR>\\bin32\\',
        description => "Intel Inspector XE Tool wrapper",
        url =>
            'https://confluence.hq.unity3d.com/display/DEV/Finding+Threading+and+Memory+Errors+in+Unity#FindingThreadingandMemoryErrorsinUnity-IntelInspectorXE',
        examples => [
            {
                description => 'Run all all native tests with memory issues detection level 2',
                command => '--suite=native --tool=inspxe --analysis-type=mi2 (Detect Memory Problems)'
            },
        ]
    );
}

sub getName
{
    return 'inspxe';
}

sub decorate
{
    my ($this, @args) = @_;
    my $analysis_type = $this->getOptionValue('analysis-type');
    my $result_dir = $this->getResultDir();
    my @result = ();
    my $inspxe = $ENV{'INSPXEBIN'};
    push(@result, "\"${inspxe}inspxe-cl\"");
    push(@result, '-collect');
    push(@result, $analysis_type);
    my $suppression_file = $this->getOptionValue('suppression-file');
    push(@result, "-s-f=$suppression_file") if ($suppression_file);
    push(@result, "-result-dir=$result_dir");
    push(@result, '-return-app-exitcode');

    my $platform = $this->getOptionValue('platform');

    if (defined($platform))
    {
        my $target = TargetResolver::resolve($platform);
        $target = fileparse($target);
        push(@result, "-executable-of-interest=$target");
    }

    push(@result, '--');
    push(@result, @args);
    return @result;
}

sub getResultDir
{
    my ($this) = @_;
    my $result_dir = $this->getOptionValue('result-dir');
    if (not defined($result_dir))
    {
        $result_dir = $this->{runContext}->getArtifactsPath();
    }
    return $result_dir;
}

1;
