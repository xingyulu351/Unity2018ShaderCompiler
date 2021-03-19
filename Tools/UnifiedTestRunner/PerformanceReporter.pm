package PerformanceReporter;

use warnings;
use strict;

sub ShouldReportPerformanceData
{
    my ($suiteName, $configName, $category) = @_;

    if (defined($suiteName) and defined($configName) and defined($category))
    {
        if (grep { $_ =~ /^performance$/i } @$category)
        {
            return 1;
        }
        if (lc($suiteName) eq "native" and defined($configName))
        {
            return 1;
        }
    }
    return;
}

sub report
{
    my ($env, %args) = @_;
    my @reporterArgs;
    if (defined($args{configName}))
    {
        push(@reporterArgs, "--configName=$args{configName}");
    }
    push(@reporterArgs, "--platform=$args{platform}");
    push(@reporterArgs, "--testsuite=$args{suiteName}");
    push(@reporterArgs, "--testresultsxml=$args{testResultsXml}");
    if (defined($args{testResultsJson}))
    {
        push(@reporterArgs, "--testresultsjson=$args{testResultsJson}");
    }
    push(@reporterArgs, "--outputjson=$args{outputJson}");
    if (defined($args{csvResultsFile}))
    {
        push(@reporterArgs, "--testresultscsv=$args{csvResultsFile}");
    }
    if (defined($args{forceBaselineUpdate}))
    {
        push(@reporterArgs, "--forcebaselineupdate");
    }
    push(@reporterArgs, "1>$args{stdOut}");
    push(@reporterArgs, "2>$args{stdErr}");
    return $env->runDotNetProgram(
        program => $args{reporterPath},
        programArgs => \@reporterArgs
    );
}

1;
