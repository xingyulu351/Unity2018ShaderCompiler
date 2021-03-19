use warnings;
use strict;

use Dirs;
use lib Dirs::external_root();

use File::Spec;
use FindBin qw ($Bin);
use lib $Bin;

use Getopt::Long qw (GetOptionsFromArray);

use Carp qw (croak);
use JSON;
use Hoarder::Collectors;
use Hoarder::TestSessionSubmissionRequestBuilder;
use SuiteResult;

use SubmitTestDataRequestSender;

sub main
{
    my ($testresultsxml, $suite, $platform, $hoarderUri, $retries, $timeout) = _readOptions();
    my $jsonText = _buildRequestAsJson($testresultsxml, $suite, $platform);
    my $sendResult = SubmitTestDataRequestSender::send(
        uri => $hoarderUri . "?format=json_header/refrenced_data_v1",
        header => $jsonText,
        files => [$testresultsxml],
        retries => $retries,
        timeout => $timeout,
        logger => sub { print(@_); }
    );

    if ($sendResult)
    {
        exit(0);
    }

    exit(1);
}

sub _buildRequestAsJson
{
    my ($testresultsxml, $suite, $platform) = @_;

    my @collectors = Hoarder::Collectors::getAllCollectors();
    my $requestBuilder = new Hoarder::TestSessionSubmissionRequestBuilder();
    my (@suiteResults) = new SuiteResult(name => $suite, platform => $platform, testresultsxml => $testresultsxml);
    my $request = $requestBuilder->build(collectors => \@collectors, suite_results => \@suiteResults);

    my %data = %{ $request->data() };
    my $json = JSON->new(utf8 => 1, convblessed => 1);
    my $jsonText = $json->to_json(\%data);
    return $jsonText;
}

sub _readOptions
{
    my ($testresultsxml, $suite, $platform, $hoarderUri, $retries, $timeout);
    my @args = @ARGV;

    my $resGetOpt = GetOptionsFromArray(
        \@args,
        "testresultsxml=s" => \$testresultsxml,
        "suite=s" => \$suite,
        "platform=s" => \$platform,
        "hoarder-uri:s" => \$hoarderUri,
        "retries:s" => \$retries,
        "timeout:s" => \$timeout
    );

    if (not defined($hoarderUri))
    {
        $hoarderUri = "http://qa.hq.unity3d.com/hoarder/api/utr";
    }

    if (not defined($retries))
    {
        $retries = 5;
    }

    if (not defined($timeout))
    {
        $timeout = 10;    # 10 secs
    }

    return ($testresultsxml, $suite, $platform, $hoarderUri, $retries, $timeout);
}

main();
