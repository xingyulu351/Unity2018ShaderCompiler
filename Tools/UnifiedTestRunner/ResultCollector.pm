package ResultCollector;

use warnings;
use strict;

use TestResult;
use SuiteResult;

sub new
{
    return bless({ suite_results => [] }, __PACKAGE__);
}

sub addSuiteResult
{
    my ($this, $result) = @_;
    push(@{ $this->{suite_results} }, $result);
}

sub getAllSuiteResults
{
    my ($this) = @_;
    return @{ $this->{suite_results} };
}

sub getAllResults
{
    my ($this) = @_;
    my (@allResults);
    foreach my $sr (@{ $this->{suite_results} })
    {
        my @suiteResults = $sr->getAllResults();
        push(@allResults, @suiteResults);
    }
    return @allResults;
}

1;
