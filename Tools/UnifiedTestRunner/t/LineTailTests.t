use warnings;
use strict;

use File::Temp qw (tempfile);
use Test::More;
use threads;
use threads::shared;

BEGIN { use_ok('LineTail') }

BASIC:
{
    can_ok('LineTail', 'new');
    my $lineReceiver = sub { };
    my $tail = LineTail->new(file => 'filevalue', receiver => $lineReceiver);
    isa_ok($tail, 'LineTail');
    is($tail->getFile(), 'filevalue');
    is($tail->getReceiver(), $lineReceiver);
    can_ok('LineTail', 'stop');
}

CAN_STOP_READING:
{
    my ($fh, $tempFileName) = tempfile();
    my $tail = LineTail->new(file => $tempFileName, receiver => sub { });
    $tail->start();
    $tail->stop();
    ok(1);
}

CAN_STOP_READING_IF_FILE_NOT_EXISTS:
{
    my ($fh, $tempFileName) = tempfile();
    my $tail = LineTail->new(file => "BLABLA", receiver => sub { });
    LineTail::setRetriesCount(1);
    $tail->start();
    $tail->stop();
    ok(1);
}

CAN_READ_ONE_LINE_FROM_EXISTING_FILE:
{
    my ($fh, $tempFileName) = tempfile();
    print $fh "line2\n";
    close($fh);

    my $receivedLine : shared;
    my $receiver = sub
    {
        my $line = shift();
        $receivedLine = $line if defined($line);
    };

    my $tail = LineTail->new(file => $tempFileName, receiver => $receiver);
    $tail->start();
    $tail->stop();
    is($receivedLine, 'line2');
}

CAN_READ_TWO_LINES_FROM_EXISTING_FILE:
{
    my ($fh, $tempFileName) = tempfile();
    print $fh "line1\n";
    print $fh "line2\n";
    close($fh);

    my @receivedLines : shared;
    my $idx : shared = 0;
    my $receiver = sub
    {
        $receivedLines[$idx++] = shift;
    };

    my $tail = LineTail->new(file => $tempFileName, receiver => $receiver);
    $tail->start();
    $tail->stop();
    is($receivedLines[0], "line1");
    is($receivedLines[1], "line2");
}

CAN_READ_TWO_LINES_FROM_EXISTING_FILE_NO_LINE_END_AT_THE_END_OF_THE_FILE:
{
    my ($fh, $tempFileName) = tempfile();
    print $fh "line1\n";
    print $fh "line2";
    close($fh);

    my @receivedLines : shared;
    my $idx : shared = 0;
    my $receiver = sub
    {
        $receivedLines[$idx++] = shift;
    };

    my $tail = LineTail->new(file => $tempFileName, receiver => $receiver);
    $tail->start();
    $tail->stop();
    is($receivedLines[0], "line1");
    is($receivedLines[1], "line2");
}

ADDITIONAL_OBJECT_APPEARS_IN_CALLBACK:
{
    my ($fh, $tempFileName) = tempfile();
    print $fh "line1\n";
    close($fh);

    my $arg : shared;
    my $receiver = sub
    {
        shift;
        ($arg) = @_;
    };

    my $tail = LineTail->new(file => $tempFileName, receiver => $receiver, arg => 3);
    $tail->start();
    $tail->stop();
    is($arg, 3);
}

CAN_READ_INCOMMING_DATA_FROM_OTHER_THREAD:
{
    my ($fh, $tempFileName) = tempfile();

    sub write_file
    {
        for (my $i = 0; $i < 20; $i++)
        {
            print $fh "line$i\n";
        }
    }

    my $idx : shared = 0;
    my @receivedLines : shared;
    my $receiver = sub
    {
        $receivedLines[$idx++] = shift;
    };

    my $tail = LineTail->new(file => $tempFileName, receiver => $receiver);
    my ($thr) = threads->create(\&write_file);
    $tail->start();
    close($fh);
    $tail->stop();
    for (my $i = 0; $i < 20; $i++)
    {
        is($receivedLines[$i], "line$i");
    }
    $thr->join();
}

WATCHED_FILENAME_APPEARS_IN_CALLBACK:
{
    my ($fh, $tempFileName) = tempfile();
    print $fh "line1\n";
    close($fh);

    my $fileName : shared;
    my $receiver = sub
    {
        (undef, undef, $fileName) = @_;
    };

    my $tail = LineTail->new(file => $tempFileName, receiver => $receiver, arg => 3);
    $tail->start();
    $tail->stop();
    is($fileName, $tempFileName);
}

done_testing();
