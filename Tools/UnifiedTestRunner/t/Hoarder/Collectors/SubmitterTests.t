use warnings;
use strict;

use Test::More;

BEGIN { use_ok('Hoarder::Collectors::Submitter') }

Creation:
{
    can_ok('Hoarder::Collectors::Submitter', 'new');
    my $c = makeCollector();
    isa_ok($c, 'Hoarder::Collectors::Submitter');
}

Name_Returns_Sumbitter:
{
    my $c = makeCollector();
    can_ok($c, 'name');
    my $name = $c->name();
    is($name, 'submitter');
}

Data_ReturnsCorrectSubmitterValue:
{
    push(@ARGV, '-submitter=a');
    my $c = makeCollector();
    my $data = $c->data();
    can_ok($c, 'data');
    is($data, 'a');
}

Data_ReturnsUndefIfSumiterIsNotSpecified:
{
    push(@ARGV, '-submitter=');
    my $c = makeCollector();
    my $data = $c->data();
    ok(not defined($data));
}

done_testing();

sub makeCollector
{
    return new Hoarder::Collectors::Submitter();
}
