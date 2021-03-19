use warnings;
use strict;

use Test::More;

BEGIN
{
    use_ok('UnityTestProtocol::FilterExpressionBuilder');
}

is(_conv('$.f1'), '$msg->{f1}');

is(_conv('$.f1==0'), '$msg->{f1} eq 0');

is(_conv('$.f1=="foo"'), '$msg->{f1} eq "foo"');

is(_conv('$.f1==\'foo\''), '$msg->{f1} eq \'foo\'');

is(_conv('$.f1.f11'), '$msg->{f1}->{f11}');

is(_conv('$.f1.f11==0'), '$msg->{f1}->{f11} eq 0');

is(_conv('$.f1.f11 > 0'), '$msg->{f1}->{f11} > 0');

is(_conv('$.f1 && $.f2'), '$msg->{f1} && $msg->{f2}');

is(_conv('$.f1 || $.f2'), '$msg->{f1} || $msg->{f2}');

done_testing();

sub _conv
{
    my ($expression) = @_;
    my $varName = 'msg';
    return UnityTestProtocol::FilterExpressionBuilder::convert($expression, $varName);
}
