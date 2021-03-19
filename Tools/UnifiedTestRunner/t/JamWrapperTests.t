use warnings;
use strict;

use Test::More;

use Dirs;
use TestHelpers::ArrayAssertions qw (arraysAreEqual arrayEndsWith);
use File::Spec::Functions qw (canonpath);
use lib Dirs::external_root();
use Test::MockModule;
use Dirs;
use SystemCall;
use JamWrapper qw (Jam);
use Test::Trap;

my $root = Dirs::getRoot();

BEGIN { use_ok('JamWrapper'); }

can_ok('JamWrapper', 'Jam');

Jam_TargetIsSpecified_InvokesJamWithAppopriateArguments:
{
    my $systemCall = new Test::MockModule('SystemCall');
    my @args;
    $systemCall->mock(
        execute => sub
        {
            (undef, @args) = @_;
            return 0;
        }
    );
    my $jam = canonpath("$root/jam.pl");
    Jam('target1');
    arraysAreEqual(\@args, ['perl', $jam, '-a', '-q', '-sUNITY_RUN_NATIVE_TESTS_DURING_BUILD=0', 'target1']);
}

Jam_TargetLogFileAreSpecified_InvokesJamWithAppopriateArguments:
{
    my $systemCall = new Test::MockModule('SystemCall');
    my @args;
    $systemCall->mock(
        execute => sub
        {
            (undef, @args) = @_;
            return 0;
        }
    );
    my $jam = canonpath("$root/jam.pl");
    Jam('target1', 'log.txt');
    arrayEndsWith(\@args, ['>log.txt']);
}

Jam_CroaksIfJamFailed:
{
    my $systemCall = new Test::MockModule('SystemCall');
    $systemCall->mock(
        execute => sub
        {
            return 123;
        }
    );

    my @trap = trap
    {
        Jam('target1');
    };
    like($trap->die, qr/jam/i);
    like($trap->die, qr/failed/i);
}

done_testing();
