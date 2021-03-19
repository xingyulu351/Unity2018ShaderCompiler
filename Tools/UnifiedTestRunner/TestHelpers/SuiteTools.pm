package TestHelpers::SuiteTools;

use warnings;
use strict;

use Options;
use GlobalOptions;
use RunContext;

sub createRunContext
{
    my ($tool, @args) = @_;
    my @toolOptions = @{ $tool->getOptions() };
    my @allOptions = (@toolOptions, GlobalOptions::getOptions());
    my $options = new Options(options => \@allOptions);
    $options->parse(@args);
    my $runContext = RunContext::new($options);
    return $runContext;
}

1;
