use Test::More;
use Hoarder::Collectors::TestSessionInfo;
use Hoarder::Presentation::Suite;

use warnings;
use strict;

BEGIN { use_ok('Hoarder::Collectors::TestSessionInfo') }

BASIC:
{
    can_ok('Hoarder::Collectors::TestSessionInfo', 'new');
    my $ai = Hoarder::Collectors::TestSessionInfo->new();
    isa_ok($ai, 'Hoarder::Collectors::TestSessionInfo');
}

RECEIVE_DATA:
{
    my $si = Hoarder::Collectors::TestSessionInfo->new();
    is($si->name(), 'run-test-session-data');

    my %data = %{ $si->data() };
    my @args = @{ $data{args} };
    is(scalar(keys(%data)), 1);
    ok(eq_array(\@args, \@ARGV));
}

done_testing();
