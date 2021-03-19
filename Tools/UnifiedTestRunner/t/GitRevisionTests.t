use strict;

use Dirs;
use lib Dirs::external_root();

use Test::More;
use Test::MockObject;
use_ok('GitRevision');

Basic_SuccessfullScenario:
{
    my $systemCall = new Test::MockObject();

    my $isCalled = 0;
    my @cmd;
    $systemCall->mock(
        executeAndGetStdOut => sub
        {
            $isCalled = 1;
            shift();
            @cmd = @_;
            return '{"revision":"shortRevision", "fullRevision": "longRevision", "date":"2018-05-23 08:00:31 +0200", "branch":"HEAD -> foobar, baz, baz2"}';
        }
    );

    my $r = new GitRevision($systemCall);
    is($isCalled, 1);
    is($cmd[0], 'git');
    is($cmd[1], 'log');
    is($cmd[2], '--max-count=1');
    like($cmd[3], qr/^--pretty=format:"\{.*\}"/);

    is($r->getId(), "shortRevision");
    is($r->getLongRevision(), "longRevision");
    is($r->getDate(), "2018-05-23 08:00:31 +0200");
    is($r->getBranch(), "foobar");
    is_deeply($r->getBranches(), ['baz', 'baz2']);
}

DetachedBranch:
{
    my $systemCall = new Test::MockObject();

    my $isCalled = 0;
    my @cmd;
    $systemCall->mock(
        executeAndGetStdOut => sub
        {
            $isCalled = 1;
            shift();
            @cmd = @_;
            return '{"revision":"shortRevision", "fullRevision": "longRevision", "date":"2018-05-23 08:00:31 +0200", "branch":"HEAD, foobar, baz"}';
        }
    );

    my $r = new GitRevision($systemCall);
    is($r->getBranch(), undef);
    is_deeply($r->getBranches(), ['foobar', 'baz']);
}

done_testing();
