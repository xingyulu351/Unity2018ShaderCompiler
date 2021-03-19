use warnings;
use strict;

use Carp qw (croak);
use File::Spec::Functions qw (canonpath);
use File::Temp qw (tempfile);
use TestHelpers::ArrayAssertions qw (arraysAreEqual);
use Dirs;
use lib Dirs::external_root();
use Test::More;
use Test::Trap;
use IO::Handle;
use Test::MockModule;

BEGIN { use_ok('FileUtils'); }

GetDirName_InvalidPath_Croaks:
{
    my @trap = trap
    {
        FileUtils::getDirName(undef);
    };

    like($trap->die, qr/path/, 'croaks when invalid path is specified');
}

GetDirName_ValidPath_ReturnsDirName:
{
    is(FileUtils::getDirName('/tmp/1/2/3/1.txt'), canonpath('/tmp/1/2/3/'));
}

RemoveSubPath_ValidSubPath_ReturnsPathWithotSubPath:
{
    is(FileUtils::removeSubPath('/tmp/1/2/3/1.txt', '/tmp/1/2'), canonpath('3/1.txt'));
}

RemoveSubPath_InvalidSubPath_ReturnsOriginalPath:
{
    is(FileUtils::removeSubPath('/tmp/1/2/3/1.txt', '/invalid/12'), canonpath('/tmp/1/2/3/1.txt'));
}

RemoveSubPath_UndefinedSubPath_ReturnsOriginalPath:
{
    is(FileUtils::removeSubPath('/tmp/1/2/3/1.txt', undef), canonpath('/tmp/1/2/3/1.txt'));
}

ReadAllLines_ValidFileNameIsGiven_ReturnsArrayWithAllLines:
{
    my ($fh, $tempFileName) = tempfile();
    print($fh "l1\nl2\nl3");
    close($fh);
    my @lines = FileUtils::readAllLines($tempFileName);
    arraysAreEqual(\@lines, ['l1', 'l2', 'l3']);
}

ReadAllLines_InvalidFileName_Dies:
{
    my @trap = trap
    {
        FileUtils::readAllLines("invalid_file_name_ever");
    };
    like($trap->die, qr/invalid_file_name_ever/);
}

WriteAllLinesToFile_InvalidFileName_Dies:
{
    my @trap = trap
    {
        FileUtils::writeAllLines("/nowhere/invalid_file_name_ever");
    };
    like($trap->die, qr/invalid_file_name_ever/);
}

WriteAllLinesToFile_ValidFileName_WriteAllLinesToFile:
{
    my ($fh, $tempFileName) = tempfile();
    close($fh);
    FileUtils::writeAllLines($tempFileName, ('l1', 'l2', 'l3'));
    my @lines = FileUtils::readAllLines($tempFileName);
    arraysAreEqual(\@lines, ['l1', 'l2', 'l3']);
}

OpenForReadOrCroak_ValidFileName_WriteAllLinesToFile:
{
    my ($fh, $tempFileName) = tempfile();
    close($fh);
    FileUtils::writeAllLines($tempFileName, ('l1', 'l2', 'l3'));
    $fh = FileUtils::openForReadOrCroak($tempFileName);
    my @lines;
    while (<$fh>)
    {
        my $line = $_;
        chomp($line);
        push(@lines, $line);
    }
    arraysAreEqual(\@lines, ['l1', 'l2', 'l3']);
}

OpenForReadOrCroak_InvalidfileName_Croaks:
{
    my @trap = trap
    {
        FileUtils::openForReadOrCroak('/file/which/does/not/exists/ever');
    };
    like($trap->die, qr/Can not read from the file.*/ims);
}

OpenForWriteOrCroak_ValidFileName_WriteAllLinesToFile:
{
    my ($fh, $tempFileName) = tempfile();
    close($fh);
    $fh = FileUtils::openForWriteOrCroak($tempFileName);
    print($fh "l1\nl2\nl3");
    close($fh);
    my @lines = FileUtils::readAllLines($tempFileName);
    arraysAreEqual(\@lines, ['l1', 'l2', 'l3']);
}

OpenForWriteOrCroak_InvalidFileName_Croaks:
{
    my @trap = trap
    {
        FileUtils::openForWriteOrCroak('/file/which/does/not/exists/ever');
    };
    like($trap->die, qr/Can not write to the file.*/ims);
}

CloseOrCroak_ValidHandle_ClosesIt:
{
    my ($fh, $tempFileName) = tempfile();
    FileUtils::closeOrCroak($fh, 'filename');
    ok(not $fh->opened());
}

CloseOrCroak_InvalidFileHandle_Croaks:
{
    my $fh = -1;
    my @trap = trap
    {
        FileUtils::closeOrCroak('-1', 'some file name');
    };
    like($trap->die, qr/Can not close the file.*/ims);
}

ScanLines_CallbackIsGiven_InvokesItOnEachLine:
{
    my ($fh, $tempFileName) = tempfile();
    FileUtils::closeOrCroak($fh);
    FileUtils::writeAllLines($tempFileName, ('l1', 'l2', 'l3'));
    my $invokesCount = 0;
    my @matchingLines;
    my $lineHandler = sub
    {
        my ($line) = @_;
        ++$invokesCount;
        if ($line =~ m/l[23]/s)
        {
            push(@matchingLines, $line);
        }
        return 1;    #continue
    };
    FileUtils::scanLines(file => $tempFileName, func => $lineHandler);
    arraysAreEqual(\@matchingLines, ['l2', 'l3']);
    is($invokesCount, 3);
}

scanLines_CallbackReturns1OnSecondLine_StopsScanning:
{
    my ($fh, $tempFileName) = tempfile();
    FileUtils::closeOrCroak($fh);
    FileUtils::writeAllLines($tempFileName, ('l1', 'l2', 'l3'));
    my @match;
    my @lines;
    my $lineHandler = sub
    {
        my ($line) = @_;
        push(@lines, $line);
        if ($line eq 'l2')
        {
            return 0;
        }
        return 1;
    };
    FileUtils::scanLines(file => $tempFileName, func => $lineHandler);
    arraysAreEqual(\@lines, ['l1', 'l2']);
}

ScanLines_CallsOpenAndCloseFiles:
{
    my ($fh, $tempFileName) = tempfile();
    my $fileUtilsModule = new Test::MockModule('FileUtils');
    my $openOrCroakCalled = 0;
    my $closeOrCroakCalled = 1;
    $fileUtilsModule->mock(
        openForReadOrCroak => sub
        {
            my ($file) = @_;
            $openOrCroakCalled = 1;
            open(my $fh, '<', $file);
            return $fh;
        }
    );
    $fileUtilsModule->mock(
        closeOrCroak => sub
        {
            my ($fh) = @_;
            $closeOrCroakCalled = 1;
            close($fh);
        }
    );
    my $lineHandler = sub { };
    FileUtils::scanLines(file => $tempFileName, func => $lineHandler);
    ok($openOrCroakCalled);
    ok($closeOrCroakCalled);
}

ScanLines_SubRoutineCroaks_InvokesCloseAndCroaks:
{
    my ($fh, $tempFileName) = tempfile();
    FileUtils::closeOrCroak($fh);
    FileUtils::writeAllLines($tempFileName, ('l1', 'l2', 'l3'));
    my $fileUtilsModule = new Test::MockModule('FileUtils');
    my $closeOrCroakCalled = 0;
    $fileUtilsModule->mock(
        openForReadOrCroak => sub
        {
            my ($file) = @_;
            open(my $fh, '<', $file);
            return $fh;
        }
    );
    $fileUtilsModule->mock(
        closeOrCroak => sub
        {
            my ($fh) = @_;
            $closeOrCroakCalled = 1;
            close($fh);
        }
    );
    my $lineHandler = sub
    {
        croak("haha");
    };
    my @trap = trap
    {
        FileUtils::scanLines(file => $tempFileName, func => $lineHandler);
    };

    ok($closeOrCroakCalled);
    like($trap->die, qr/haha/s);
}

done_testing();
