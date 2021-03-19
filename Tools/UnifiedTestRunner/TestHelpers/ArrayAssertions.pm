package TestHelpers::ArrayAssertions;

use warnings;
use strict;

use Test::More;

use Dirs;
use lib Dirs::external_root();
use Array::Utils qw (array_diff);

use experimental 'smartmatch';

use Exporter 'import';

use Test::Builder;

use Test::Deep qw (cmp_deeply eq_deeply cmp_details deep_diag cmp_bag bag subbagof superbagof subsetof noneof);

my $Test = new Test::Builder();

our @EXPORT_OK = qw [arrayStartsWith arrayContains arrayEndsWith arraysAreEqual isEmpty arraysAreEquivalent arrayDoesNotContain];

sub arrayEndsWith
{
    my ($a1, $a2, $desc) = @_;

    my @a1_copy = @$a1;
    my @a2_copy = _convertToArray($a2);

    my $a1_len = scalar(@a1_copy);
    my $a2_len = scalar(@a2_copy);

    if ($a1_len > $a2_len)
    {
        my $diff = $a1_len - $a2_len;
        splice(@a1_copy, 0, $diff);
    }

    @_ = (\@a1_copy, \@a2_copy, $desc);
    goto &_doCmp;
}

sub arraysAreEqual
{
    goto &_doCmp;
}

sub arrayContains
{
    my ($a1, $a2, $desc) = @_;

    my @a1_copy = @$a1;
    my @a2_copy = _convertToArray($a2);

    @_ = (\@a1_copy, superbagof(@a2_copy), $desc);
    goto &_doCmp;
}

sub arraysAreEquivalent
{
    my ($a1, $a2, $desc) = @_;

    my @a1_copy = @$a1;
    my @a2_copy = _convertToArray($a2);

    @_ = (\@a1_copy, bag(@a2_copy), $desc);
    goto &_doCmp;
}

sub arrayDoesNotContain
{
    my ($a1, $a2, $desc) = @_;

    my @a1_copy = @$a1;
    my @a2_copy = _convertToArray($a2);

    @_ = (\@a1_copy, noneof(@a2_copy), $desc);
    goto &_doCmp;
}

sub arrayStartsWith
{
    my ($a1, $a2, $desc) = @_;

    my @a1_copy = @$a1;
    my @a2_copy = _convertToArray($a2);
    _ajustArrays(\@a1_copy, \@a2_copy);
    @_ = (\@a1_copy, \@a2_copy, $desc);
    goto &_doCmp;
}

sub isEmpty
{
    my ($array, $desc) = @_;
    my @array = _convertToArray($array);
    return ($Test->ok(scalar(@array) == 0, $desc) || $Test->diag("array expected to be empty"));
}

sub _ajustArrays
{
    my ($a1, $a2) = @_;

    my $a1_len = scalar(@$a1);
    my $a2_len = scalar(@$a2);

    if ($a1_len > $a2_len)
    {
        my $diff = $a1_len - $a2_len;
        splice(@$a1, -$diff);
    }
}

sub _convertToArray
{
    my ($a) = @_;
    my @result;
    if (ref($a) eq 'ARRAY')
    {
        @result = @$a;
    }
    else
    {
        @result = ($a);
    }
    return @result;
}

sub _doCmp
{
    my ($a1, $a2, $desc) = @_;
    my ($ok, $stack) = cmp_details($a1, $a2);
    if (not $Test->ok($ok, $desc))
    {
        my $diag = deep_diag($stack);
        $Test->diag($diag);
    }
}

1;
