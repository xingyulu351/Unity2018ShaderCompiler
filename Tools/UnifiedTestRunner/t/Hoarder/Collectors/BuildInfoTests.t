use warnings;
use strict;

use Test::More;

BEGIN { use_ok('Hoarder::Collectors::BuildInfo') }

Creation:
{
    can_ok('Hoarder::Collectors::BuildInfo', 'new');
    my $c = makeCollector();
    isa_ok($c, 'Hoarder::Collectors::BuildInfo');
}

Name_RetunsValidName:
{
    my $c = makeCollector();
    is($c->name(), 'build-info');
}

Data_RetunsExpectedParams:
{
    push(@ARGV, '--config=Test-ABC');
    push(@ARGV, '--config-id=abc');
    push(@ARGV, '--build-number=1');

    my $c = makeCollector();
    my %data = %{ $c->data() };
    is($data{config}, 'Test-ABC');
    is($data{config_id}, 'abc');
    is($data{build_number}, '1');
    is(scalar(keys(%data)), 3);
}

Data_RetunsNullIfBuildInfoConfigIsMissed:
{
    push(@ARGV, '--config=');
    push(@ARGV, '--config-id=abc');
    push(@ARGV, '--build-number=1');

    my $c = makeCollector();
    ok(not defined($c->data()));
}

Data_RetunsNullIfBuildInfoConfigIdIsMissed:
{
    push(@ARGV, '--config=a');
    push(@ARGV, '--config-id=');
    push(@ARGV, '--build-number=1');

    my $c = makeCollector();
    ok(not defined($c->data()));
}

Data_RetunsNullIfBuildNumberIsMissed:
{
    push(@ARGV, '--config=a');
    push(@ARGV, '--config-id=');
    push(@ARGV, '--build-number=');

    my $c = makeCollector();
    ok(not defined($c->data()));
}

Data_RetunsNullIfOneOfTheParametersIsMissed:
{
    push(@ARGV, '--config=a');
    push(@ARGV, '--build-number=12');

    my $c = makeCollector();
    ok(not defined($c->data()));
}

done_testing();

sub makeCollector
{
    return new Hoarder::Collectors::BuildInfo();
}
