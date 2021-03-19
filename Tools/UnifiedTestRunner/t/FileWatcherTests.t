use warnings;
use strict;

use Test::More;
use File::Temp qw (tempfile);
use TestHelpers::ArrayAssertions qw (arraysAreEqual arrayContains);
use Carp qw (croak);
use English qw( -no_match_vars );

BEGIN { use_ok('FileWatcher'); }

Creation:
{
    can_ok('FileWatcher', 'new');
    my $fileWatcher = createWatcher();
    isa_ok($fileWatcher, 'FileWatcher');
}

Watch_EmptyFile_NoLinesAreReceived:
{
    my $file = createFile();
    my @receivedLines = sendAndReceive($file, []);
    is(scalar(@receivedLines), 0);
}

Watch_OneLineIsWrittenIntoTheWatchedFile_ReceivesIt:
{
    my $file = createFile();
    my @receivedLines = sendAndReceive($file, ['line1']);
    arraysAreEqual(\@receivedLines, ['line1']);
}

Watch_TwoLinesAreWrittenIntoTheWatchedFile_ReceivesThem:
{
    my $file = createFile();
    my @receivedLines = sendAndReceive($file, ['line1', 'line2']);
    arraysAreEqual(\@receivedLines, ['line1', 'line2']);
}

Watch_TwoFilesOneReceiver_GetsLinesFromBothFile:
{
    my $file1 = createFile();
    my $file2 = createFile();

    my @receivedLines : shared;
    my $receiver = sub
    {
        my ($line) = @_;
        if (not defined($line))
        {
            return;
        }
        push(@receivedLines, $line);
    };

    sendLinesToFile($file1, ['line1']);
    sendLinesToFile($file2, ['line2']);

    my $fileWatcher = createWatcher();
    $fileWatcher->watch($file1, $receiver);
    $fileWatcher->watch($file2, $receiver);

    $fileWatcher->start();
    $fileWatcher->stop();
    arrayContains(\@receivedLines, ['line1', 'line2']);
}

Watch_TwoFilesTwoReceivers_GetsLinesFromBothFile:
{
    my $file1 = createFile();
    my $file2 = createFile();

    my @receivedLines1 : shared;
    my $receiver1 = sub
    {
        my ($line) = @_;
        if (not defined($line))
        {
            return;
        }
        push(@receivedLines1, $line);
    };

    my @receivedLines2 : shared;
    my $receiver2 = sub
    {
        my ($line) = @_;
        if (not defined($line))
        {
            return;
        }
        push(@receivedLines2, $line);
    };

    sendLinesToFile($file1, ['line1']);
    sendLinesToFile($file2, ['line2']);

    my $fileWatcher = createWatcher();
    $fileWatcher->watch($file1, $receiver1);
    $fileWatcher->watch($file2, $receiver2);

    $fileWatcher->start();
    $fileWatcher->stop();
    arraysAreEqual(\@receivedLines1, ['line1']);
    arraysAreEqual(\@receivedLines2, ['line2']);
}

Watch_OneFileTwoReceivers_BothReceiversGetAllLines:
{
    my $file1 = createFile();
    my $file2 = createFile();

    my @receivedLines1 : shared;
    my $receiver1 = sub
    {
        my ($line) = @_;
        if (not defined($line))
        {
            return;
        }
        push(@receivedLines1, $line);
    };

    my @receivedLines2 : shared;
    my $receiver2 = sub
    {
        my ($line) = @_;
        if (not defined($line))
        {
            return;
        }
        push(@receivedLines2, $line);
    };

    sendLinesToFile($file1, ['line1', 'line2', 'line3']);

    my $fileWatcher = createWatcher();
    $fileWatcher->watch($file1, $receiver1);
    $fileWatcher->watch($file1, $receiver2);

    $fileWatcher->start();
    $fileWatcher->stop();
    arraysAreEqual(\@receivedLines1, ['line1', 'line2', 'line3']);
    arraysAreEqual(\@receivedLines2, ['line1', 'line2', 'line3']);
}

Watch_ContextData_Recevied:
{
    my $file1 = createFile();
    my @receivedLines2 : shared;

    my $contextValue : shared = 0;
    my $receiver = sub
    {
        my (undef, $context) = @_;
        $contextValue = $context;
    };

    my $fileWatcher = createWatcher();
    $fileWatcher->watch($file1, $receiver, 1);
    $fileWatcher->start();
    $fileWatcher->stop();
    is($contextValue, 1);
}

done_testing();

sub createWatcher
{
    my $fileWatcher = new FileWatcher();
    return $fileWatcher;
}

sub createFile
{
    my ($fh, $tempFileName) = tempfile();
    closeFile($fh, $tempFileName);
    return $tempFileName;
}

sub sendAndReceive
{
    my ($fileName, @lines) = @_;
    sendLinesToFile($fileName, @lines);
    return receiveLinesFromFile($fileName);
}

sub sendLinesToFile
{
    my ($file, $lines) = @_;
    my $openRes = open(my $fh, '>', $file);
    if (!defined($openRes))
    {
        croak("Can not open the file: $file. $OS_ERROR");
    }
    foreach my $line (@{$lines})
    {
        print($fh "$line\n");
    }
    closeFile($fh, $file);
}

sub receiveLinesFromFile
{
    my ($fileName) = @_;
    my @receivedLines : shared;
    my $receiver = sub
    {
        my ($line) = @_;
        if (not defined($line))
        {
            return;
        }
        push(@receivedLines, $line);
    };
    my $fileWatcher = createWatcher();
    $fileWatcher->watch($fileName, $receiver);
    $fileWatcher->start();
    $fileWatcher->stop();
    return @receivedLines;
}

sub closeFile
{
    my ($handle, $fileName) = @_;
    my $closeRes = close($handle);
    if (not $closeRes)
    {
        croak("Can not open the file $fileName. $OS_ERROR\n");
    }
}
