use warnings;
use strict;

use Test::More;
use Dirs;
use lib Dirs::external_root();
use Test::MockModule;
use UnityTestProtocol::Utils qw (:all);

getMessage_UndefinedMessage_ReturnsUndef:
{
    my $msg = getMessage(undef);
    is($msg, undef);
}

IsProtocolMessage_InvalidMessage_ReturnsFalse:
{
    my $msg = getMessage("invalid");
    is($msg, undef);
}

IsProtocolMessage_ValidMessage_ReturnsMessage:
{
    my $line =
        '##utp:{"type":"StepStatus","phase":"End","error":"System.Exception: something failed","success":false,"name":"ProjectBuild","description":"Building test project"}';
    my $result = getMessage($line);
    is($result->{type}, 'StepStatus');
}

IsProtocolMessage_PrefixBeforeMessage_ReturnsMessage:
{
    my $line =
        'some prefix##utp:{"type":"StepStatus","phase":"End","error":"System.Exception: something failed","success":false,"name":"ProjectBuild","description":"Building test project"}';
    my $result = getMessage($line);
    is($result->{type}, 'StepStatus');
}

GetFirstProtocolMessage_NoSuchMessageInFile_ReturnsUndef:
{
    my $content = <<END_FILE;
Do this
Reaload assemblies
##utp:{"type":"StepStatus","phase":"End","error":"System.Exception: something failed","success":false,"name":"ProjectBuild","description":"Building test project"}
Do that
END_FILE
    my $fileUtils = new Test::MockModule('FileUtils');
    $fileUtils->mock(
        openForReadOrCroak => sub
        {
            open(my $fh, '<', \$content);
            return $fh;
        }
    );

    my $msg = getFirst(
        fileName => 'does_not_matter',
        predicate => sub
        {
            my ($msg) = @_;
            return $msg->{type} eq 'StepStatus';
        }
    );
    is($msg->{type}, 'StepStatus');
}

ActionMessages:
{
    my $msg = getMessage('##utp:{"type":"Action","time":0, "name":"actionName", "phase":"Begin"}');
    is(isAction($msg), 1);
    is(isActionBegin($msg), 1);
    is(isActionEnd($msg), 0);
    is(isBeginMessage($msg), 1);
    is(isEndMessage($msg), 0);

    $msg = getMessage('##utp:{"type":"Action","time":0, "name":"actionName", "phase":"End"}');

    is(isAction($msg), 1);
    is(isActionBegin($msg), 0);
    is(isActionEnd($msg), 1);
    is(isBeginMessage($msg), 0);
    is(isEndMessage($msg), 1);
}

TestMessages:
{
    my $msg = getMessage('##utp:{"type":"TestStatus","time":0,"version":2,"phase":"Begin","name":"testName"}');

    is(isTestStatus($msg), 1);
    is(isTestBegin($msg), 1);
    is(isTestEnd($msg), 0);

    $msg = getMessage('##utp:{"type":"TestStatus","time":0,"version":2,"phase":"End","name":"testName"}');
    is(isTestStatus($msg), 1);
    is(isTestBegin($msg), 0);
    is(isTestEnd($msg), 1);
}

TestMessages_chekcs:
{
    my $msg = getMessage('##utp:{"type":"TestPlan","time":0,"version":2,"phase":"Immediate"}');
    is(isTestPlan($msg), 1);
}

getMessages:
{
    my $content = <<END_FILE;
Do this
Reaload assemblies
##utp:{"type":"Action","time":22166,"version":1,"name":"sceneBuild","description":"Building scene Assets/001-AnimProc.unity","phase":"Begin"}
##utp:{"type":"StepStatus","phase":"End","error":"System.Exception: something failed","success":false,"name":"ProjectBuild","description":"Building test project"}
##utp:{"type":"Action","time":22455,"version":1,"name":"sceneBuild","success":true,"description":"Building scene Assets/001-AnimProc.unity","phase":"End","duration":289}
Do that
END_FILE
    my $fileUtils = new Test::MockModule('FileUtils');
    $fileUtils->mock(
        openForReadOrCroak => sub
        {
            open(my $fh, '<', \$content);
            return $fh;
        }
    );

    my @messages = getMessages(
        fileName => 'does_not_matter',
        predicate => sub
        {
            my ($msg) = @_;
            return $msg->{type} eq 'Action';
        }
    );

    is(scalar(@messages), 2);
    is($messages[0]->{type}, 'Action');
    is($messages[1]->{type}, 'Action');
}

done_testing();
