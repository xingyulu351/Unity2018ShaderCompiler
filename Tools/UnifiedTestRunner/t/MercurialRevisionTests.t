use warnings;
use strict;

use Dirs;
use lib Dirs::external_root();
use Test::More;
use Test::MockObject;
use Test::Trap;

BEGIN { use_ok('MercurialRevision'); }

can_ok('MercurialRevision', 'new');

Basic_SuccessfullScenario:
{
    my $revisionId = '123';
    my @calledCommands;
    my $systemCall = new Test::MockObject();
    $systemCall->mock(
        executeAndGetAllOutput => sub
        {
            shift();
            push(@calledCommands, join(' ', @_));
            return (0, "6c6784c0361d 321 1407313128 -10800 toolsmiths/dynamic-analysis\n");
        }
    );

    my $rev = new MercurialRevision($systemCall);
    is(@calledCommands, 1);
    is($calledCommands[0], "hg log --rev . --limit=1 --template \"{node|short} {rev} {date|hgdate} {branch}\\n\"");
    is($rev->getId(), '6c6784c0361d');
    is($rev->getDate(), '1407313128');
    is($rev->getRevNumber(), '321');
    is($rev->getBranch(), 'toolsmiths/dynamic-analysis');
    is($rev->getVcsType(), 'mercurial');
}

done_testing();
