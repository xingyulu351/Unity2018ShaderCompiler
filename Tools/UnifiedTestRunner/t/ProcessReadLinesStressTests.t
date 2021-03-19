use warnings;
use strict;

use File::Spec::Functions qw (catfile);
use File::Temp qw (tempfile);
use Dirs;

use SystemCall;

use Test::More;

my $writerProcess = catfile(Dirs::UTR_root(), 'write_lines.pl');
my ($fh, $tempFileName) = tempfile();
close($fh);

my $expectedLinesCount = 10000;
my $expectedLineLength = 1000;

my $cmd = ['perl', $writerProcess, $expectedLinesCount, $expectedLineLength, ">$tempFileName"];

my $linesCount : shared = 0;
my $incompleteLines : shared = 0;

my $systemCall = new SystemCall();
$systemCall->executeAndWatchFiles(
    command => $cmd,
    filesToWatch => [{ file => $tempFileName, callback => \&recordLine }]
);

is($linesCount, $expectedLinesCount);
is($incompleteLines, 0);

sub recordLine
{
    my ($line) = @_;
    if (not $line or length($line) == 0)
    {
        return;
    }
    if (length($line) != $expectedLineLength)
    {
        ++$incompleteLines;
    }
    ++$linesCount;
}

unlink($tempFileName);

done_testing();
