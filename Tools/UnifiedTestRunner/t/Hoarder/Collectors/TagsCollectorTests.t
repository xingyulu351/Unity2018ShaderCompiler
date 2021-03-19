use warnings;
use strict;

use Test::More;

BEGIN { use_ok('Hoarder::Collectors::Tags') }

Creation:
{
    can_ok('Hoarder::Collectors::Tags', 'new');
    my $tc = makeCollector();
    isa_ok($tc, 'Hoarder::Collectors::Tags');
}

Name_RetunsValidName:
{
    my $tc = makeCollector();
    is($tc->name(), 'tags');
}

Data_RetunsExpectedTags:
{
    push(@ARGV, '--tag=abv');
    push(@ARGV, '--tag=Nightly');

    my $tc = makeCollector();
    my @tags = @{ $tc->data() };
    is($tags[0], 'abv');
    is($tags[1], 'Nightly');
}

done_testing();

sub makeCollector
{
    return new Hoarder::Collectors::Tags();
}
