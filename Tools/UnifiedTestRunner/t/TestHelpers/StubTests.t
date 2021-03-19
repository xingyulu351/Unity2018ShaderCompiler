use Test::More;

use warnings;
use strict;

use Dirs;
use lib Dirs::external_root();
use Test::Trap;

use_ok('TestHelpers::Stub');

can_ok('TestHelpers::Stub', 'new');

my $stub = new TestHelpers::Stub();
isa_ok($stub, 'TestHelpers::Stub');

specifyExpectation_NoMethodSpecified_Croaks:
{
    my $stub = new TestHelpers::Stub();
    my @trap = trap
    {
        $stub->specifyExpectation();
    };
    like($trap->die, qr/method/, 'specifyExpectation croaks when method is not specified');
}

invokeStubedMethod_MethodNameAndResult_ReturnsResult:
{
    my $stub = new TestHelpers::Stub();
    $stub->specifyExpectation(method => 'getA', result => 'A');
    is($stub->object()->getA(), 'A');
}

invokeStubedMethod_ExpectedParamsAndResult_ReturnsResult:
{
    my $stub = new TestHelpers::Stub();
    $stub->specifyExpectation(
        method => 'getA',
        params => { p1 => 1, p2 => 2 },
        result => 'A'
    );

    is($stub->object()->getA(p1 => 1, p2 => 2), 'A');
}

invokeStubedMethod_UnexpectedParamsAndResult_ReturnsResult:
{
    my $stub = new TestHelpers::Stub();
    $stub->specifyExpectation(
        method => 'getA',
        params => { p1 => 1, p2 => 2 },
        result => 'A'
    );

    is($stub->object()->getA(p1 => 2, p2 => 2), undef);
}

invokeStubedMethod_UnexpectedParamsAndDefaultValue_ReturnsDefaultValue:
{
    my $stub = new TestHelpers::Stub();

    $stub->specifyExpectation(
        method => 'getA',
        result => 'B'
    );

    $stub->specifyExpectation(
        method => 'getA',
        params => { p1 => 1, p2 => 2 },
        result => 'A'
    );

    is($stub->object()->getA(p1 => 2, p2 => 2), 'B');
}

invokeStubedMethod_ResultIsNotSpecified_ReturnsUndef:
{
    my $stub = new TestHelpers::Stub();
    $stub->specifyExpectation(
        method => 'getA',
        params => { p1 => 1, p2 => 2 }
    );

    is($stub->object()->getA(p1 => 1, p2 => 2), undef);
}

invokeSameStubedMetohdTwice_MatchTwoDifferentExpectations_RerunsResultForEachExpectation:
{
    my $stub = new TestHelpers::Stub();

    $stub->specifyExpectation(
        method => 'getA',
        params => { p1 => 1 },
        result => 1
    );

    $stub->specifyExpectation(
        method => 'getA',
        params => { p1 => 2 },
        result => 2
    );

    is($stub->object()->getA(p1 => 1), 1);
    is($stub->object()->getA(p1 => 2), 2);
}

done_testing();
