package Hoarder::TestSessionSubmissionRequestBuilder;

use warnings;
use strict;

use Hoarder::Presentation::SumbitTestResultRequest;

sub new
{
    my $instance = {};
    return bless($instance, __PACKAGE__);
}

sub build
{
    my ($this, %params) = @_;
    my $collectors = $params{collectors};
    my $request = new Hoarder::Presentation::SumbitTestResultRequest();

    foreach my $collector (@{$collectors})
    {
        my $data = $collector->data();
        if (defined($data))
        {
            $request->addCollectorData($collector->name(), $data);
        }
    }
    my @suiteResults = @{ $params{suite_results} };
    $request->addSuiteResults(@suiteResults);
    $request->setSmartSelectSessionId($params{smartSelectSessionId});
    return $request;
}

1;
