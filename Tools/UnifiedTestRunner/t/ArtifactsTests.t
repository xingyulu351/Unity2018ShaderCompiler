use Test::More;

use warnings;
use strict;

BEGIN { use_ok('Artifacts') }

CREATION:
{
    can_ok('Artifacts', 'new');
    my $artifacts = Artifacts->new();
    isa_ok($artifacts, 'Artifacts');
    can_ok('Artifacts', 'register');
    can_ok('Artifacts', 'get');
}

REGISTER_ARTIFACTS_FOR_SUITE:
{
    my $artifacts = Artifacts->new();
    $artifacts->register('native', '/tmp/some_artifact');
    my @artifacts = $artifacts->get('native');

    is(scalar(@artifacts), 1);
    is($artifacts[0], '/tmp/some_artifact');

    $artifacts->register('native', '/tmp/another_artifact');
    @artifacts = $artifacts->get('native');
    is(scalar(@artifacts), 2);
    is($artifacts[0], '/tmp/some_artifact');
    is($artifacts[1], '/tmp/another_artifact');
    @artifacts = $artifacts->get('XXX');
    is(scalar(@artifacts), 0, 'Returns empty set no artifacts for given suite');
}

done_testing();
