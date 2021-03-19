package Report::TestSessionReportBuilder;

use warnings;
use strict;

use Report::Presentation::Suite;
use Report::Presentation::Test;
use File::Spec::Functions qw (catfile catdir);
use SummaryBuilder;
use FileUtils;
use Dirs;
use File::Spec;
use Commands;

sub buildData
{
    my %args = @_;
    my @suiteResults = @{ $args{suiteResults} };
    my $artifactsRoot = $args{artifactsRoot};

    my %data;
    my @suitesP = _getSuiteResultsData($artifactsRoot, \@suiteResults, $args{commandLine});

    $data{utrPrefix} = Commands::getUtrPrefix();
    $data{suites} = \@suitesP;
    my %summary = _summary(@suiteResults);
    $data{summary} = \%summary;
    if ($args{collectors})
    {
        my @collectors = @{ $args{collectors} };
        foreach my $c (@collectors)
        {
            $data{ $c->name() } = $c->data();
        }
    }
    return %data;
}

sub generateHtml
{
    my ($jsonText, $destinationFile) = @_;
    my $utrRoot = Dirs::UTR_root();
    my $reportRoot = catdir($utrRoot, 'TestReportTemplate');
    my $templateFile = catfile($reportRoot, 'TestReport.html');

    my @lines = FileUtils::readAllLines($templateFile);
    my @results;
    for (my $i = 0; $i < scalar(@lines); $i++)
    {
        if ($lines[$i] =~ /TEST_EXECUTION_DATA/)
        {
            $lines[$i] = $jsonText;
            push(@results, $jsonText);
            next;
        }

        my ($ngTemplateFile) = ($lines[$i] =~ /NG_TEMPLATE_(.*)/);
        if ($ngTemplateFile)
        {
            $ngTemplateFile =~ s/\R//g;
            $ngTemplateFile = catfile($reportRoot, 'TestReport.files', 'templates', $ngTemplateFile);
            push(@results, FileUtils::readAllLines($ngTemplateFile));
            next;
        }

        push(@results, $lines[$i]);
    }

    FileUtils::writeAllLines($destinationFile, @results);
}

sub _getSuiteResultsData
{
    my ($artifactsRoot, $suiteResults, $commandLine, $overridedOptions) = @_;
    my @result;
    foreach my $sr (@$suiteResults)
    {
        my $suiteName = $sr->getName();
        my $suiteDescription = $sr->getDescription();
        my $platform = $sr->getPlatform();
        my @artifacts = $sr->getArtifacts();
        @artifacts = _cleanupArtifactPathes($artifactsRoot, @artifacts);
        my @overridedOptions = $sr->getOverridedOptions();
        my @testsData = _getTestData($sr->getAllResults());
        my @failureReasons = $sr->getFailureReasons();
        my %summary = _summary($sr);
        my @minimalCommandLine = $sr->getMinimalCommandLine();
        my $suiteP = new Report::Presentation::Suite(
            name => $suiteName,
            description => $suiteDescription,
            platform => $platform,
            artifacts => \@artifacts,
            tests => \@testsData,
            summary => \%summary,
            commandLine => $commandLine,
            overridedOptions => \@overridedOptions,
            data => $sr->getData(),
            testresultsxml => $sr->getTestResultXmlFilePath(),
            forciblyFailed => $sr->isForcibilyFailed(),
            failureReasons => \@failureReasons,
            details => $sr->getDetails(),
            partitionIndex => $sr->getPartitionIndex()
        );
        push(@result, $suiteP->data());
    }
    return @result;
}

sub _cleanupArtifactPathes
{
    my ($artifacsRoot, @artifacts) = @_;
    my @result = map { FileUtils::removeSubPath($_, $artifacsRoot) } @artifacts;
    return @result;
}

sub _getTestData
{
    my @testResults = @_;
    my @result;
    foreach my $tr (@testResults)
    {
        my $testPresentation = new Report::Presentation::Test($tr);
        if ($tr->{errors})
        {
            $testPresentation->{errors} = $tr->{errors};
        }
        push(@result, $testPresentation->data());
    }

    return @result;
}

sub _summary
{
    my @testSuites = @_;
    return SummaryBuilder::build(@testSuites);
}

1;
