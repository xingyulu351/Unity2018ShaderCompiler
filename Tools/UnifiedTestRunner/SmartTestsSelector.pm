package SmartTestsSelector;

use warnings;
use strict;

use Dirs;
use lib Dirs::external_root();

use Carp qw (croak);
use LWP::UserAgent;

use TestPlan;
use TestPlanItem;
use JSON;
use Logger;
use ParamUtils qw (verifyParamExistsOrCroak);

sub queryTests
{
    if (@_ == 0)
    {
        return undef;
    }
    my %args = @_;

    verifyParamExistsOrCroak('uri', %args);
    verifyParamExistsOrCroak('revision', %args);
    verifyParamExistsOrCroak('branch', %args);
    verifyParamExistsOrCroak('submitter', %args);
    verifyParamExistsOrCroak('configId', %args);
    verifyParamExistsOrCroak('testPlan', %args);

    my $timeout = 15;
    if (defined($args{timeout}))
    {
        $timeout = $args{timeout};
    }

    my $response = Request::send(
        request => _createRequest(%args),
        timeout => $timeout
    );

    if ($response->is_error())
    {
        Logger::error("Can not query tests statistics due to the error: " . $response->as_string());
        return undef;
    }

    my $result = jsonToObj($response->content());

    my $isArray = ref($result) eq 'ARRAY';
    if ($isArray)
    {
        return _createTestPlan($result);
    }
    else
    {
        my ($testRun, $skippedTests) = _createTestPlan($result->{Resolutions});
        return ($testRun, $skippedTests, $result->{SmartSelectSessionId});
    }

    return _createTestPlan($result);
}

sub _createTestItems
{
    my ($testPlan) = @_;
    my @tests;
    my @testItems = $testPlan->getAllTestItems();
    foreach my $t (@testItems)
    {
        push(@tests, { Name => $t->getName() });
    }
    return \@tests;
}

sub _createTestPlan
{
    Logger::minimal('Excluding the following tests according to the statistical data:');

    my ($resolutions) = @_;
    my $testPlan = new TestPlan();
    my $excludedTests = [];
    foreach my $item (@{$resolutions})
    {
        if ($item->{Resolution} == 0)
        {    # 0 is Run, 1 is Skip;
            my $testItem = new TestPlanItem(name => $item->{Name}, testCaseCount => 1);
            $testPlan->registerTest($testItem);
        }
        else
        {
            Logger::minimal("\t" . $item->{Name});
            push(@$excludedTests, $item->{Name});
        }
    }

    Logger::minimal("Excluded tests count: " . scalar(@{$excludedTests}));
    return ($testPlan, $excludedTests);
}

sub _createRequest
{
    my (%args) = @_;
    my $headers = HTTP::Headers->new(
        Content_type => 'application/json',
        Accept => 'application/json',
    );
    return HTTP::Request->new(
        POST => $args{uri},
        $headers,
        _getJsonContentFromRequest(%args),
    );
}

sub _getJsonContentFromRequest
{
    my %args = @_;
    my $requestDataObj = {
        Branch => $args{branch},
        Submitter => $args{submitter},
        ConfigId => $args{configId},
        Tests => _createTestItems($args{testPlan}),
        Revision => $args{revision},
        BuildNumber => $args{build},
        Capabilities => $args{capabilities},
    };

    return objToJson($requestDataObj);
}

1;
