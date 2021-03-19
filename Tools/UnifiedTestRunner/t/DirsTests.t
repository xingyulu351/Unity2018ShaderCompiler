use warnings;
use strict;

use File::Spec::Functions qw (canonpath catfile catdir);
use File::Path qw(make_path remove_tree);
use Carp qw (croak);

use Dirs;
use lib Dirs::external_root();

use Test::More;
use Test::Trap;

use TestHelpers::ArrayAssertions qw (arrayContains arraysAreEqual);
use File::Temp qw (tempdir);

my $testDataFolder = tempdir();

_populateTestDir($testDataFolder);

BEGIN { use_ok('Dirs'); }

ROOT_DIR:
{
    my $root = Dirs::getRoot();
    my $buildPl = $root . '/build.pl';
    ok(-e $buildPl);
}

GetFilesByExtentsionRecursive_CroaksDirDoesNotExists:
{
    my @trap = trap
    {
        Dirs::getFilesByExtentsionRecursive('NonExiststingDir', 'ext1');
    };

    like($trap->die, qr/not exist/);
}

GetFilesByExtentsionRecursive_ExistingExtensionIsSpecified_AddsItInResult:
{
    my $utrRoot = Dirs::UTR_root();

    my @files = Dirs::getFilesByExtentsionRecursive($testDataFolder, 'txt1');

    my @expectedFiles = (
        canonpath("$testDataFolder/1/1.1/1.1.1.txt1"), canonpath("$testDataFolder/2/2.1.txt1"),
        canonpath("$testDataFolder/2/2.1/2.1.1.txt1"), canonpath("$testDataFolder/2/2.2/2.2.1.txt1")
    );

    ok(scalar(@expectedFiles) eq scalar(@files));
    arrayContains(\@files, \@expectedFiles);
}

GetFilesByExtentsionRecursive_ExisitingExtensionButWithDifferentCase_AddsItInResult:
{
    my @files = Dirs::getFilesByExtentsionRecursive($testDataFolder, 'ext');

    my @expectedFiles = (canonpath("$testDataFolder/1.ExT"), canonpath("$testDataFolder/2.eXt"),);

    ok(scalar(@expectedFiles) eq scalar(@files));
    arrayContains(\@files, \@expectedFiles);
}

GetFilesByExtentsionRecursive_MultipleExtensionsAreSpecified_AddsThemInResult:
{
    my @files = Dirs::getFilesByExtentsionRecursive($testDataFolder, 'ext', 'txt1');

    my @expectedFiles = (
        canonpath("$testDataFolder/1/1.1/1.1.1.txt1"), canonpath("$testDataFolder/2/2.1.txt1"),
        canonpath("$testDataFolder/2/2.1/2.1.1.txt1"), canonpath("$testDataFolder/2/2.2/2.2.1.txt1"),
        canonpath("$testDataFolder/1.ExT"), canonpath("$testDataFolder/2.eXt")
    );
    ok(scalar(@expectedFiles) eq scalar(@files));
    arrayContains(\@files, \@expectedFiles);
}

GetFilesByExtentsionRecursive_SearchStringMatchesFileName_ReturnsNothing:
{
    my @files = Dirs::getFilesByExtentsionRecursive($testDataFolder, '1');
    my @expectedFiles = (
        canonpath("$testDataFolder/1/1.1.ext1"), canonpath("$testDataFolder/1/1.1/1.1.1.txt1"),
        canonpath("$testDataFolder/2/2.1.txt1"), canonpath("$testDataFolder/2/2.1/2.1.1.txt1"),
        canonpath("$testDataFolder/2/2.2/2.2.1.txt1"),
    );

    ok(scalar(@files) eq scalar(@expectedFiles));
    arrayContains(\@files, \@expectedFiles);
}

GetFilesByExtentsionRecursive_SearchStringMatchesFileName_ReturnsNothing:
{
    my @files = Dirs::getFilesByExtentsionRecursive($testDataFolder, '$[');
    my @expectedFiles = (canonpath("$testDataFolder/1.\$\["));

    arraysAreEqual(\@files, \@expectedFiles);
}

done_testing();

sub _populateTestDir
{
    _createEmptyFile(catdir($testDataFolder,), '1.$[');
    _createEmptyFile(catdir($testDataFolder,), '1.ExT');
    _createEmptyFile(catdir($testDataFolder,), '2.eXt');

    _createEmptyFile(catdir($testDataFolder, '1'), '1.1.ext1');

    _createEmptyFile(catdir($testDataFolder, '1', '1.1'), '1.1.1.txt1');
    _createEmptyFile(catdir($testDataFolder, '1', '1.1'), '1.1.2.txt2');
    _createEmptyFile(catdir($testDataFolder, '1', '1.1'), '1.1.1.txt1');

    _createEmptyFile(catdir($testDataFolder, '2'), '2.1.txt');
    _createEmptyFile(catdir($testDataFolder, '2'), '2.1.txt1');
    _createEmptyFile(catdir($testDataFolder, '2'), '2.2.txt2');

    _createEmptyFile(catdir($testDataFolder, '2', '2.1'), '2.1.1.txt1');
    _createEmptyFile(catdir($testDataFolder, '2', '2.1'), '2.1.2.txt2');

    _createEmptyFile(catdir($testDataFolder, '2', '2.2'), '2.2.1.txt1');
    _createEmptyFile(catdir($testDataFolder, '2', '2.2'), '2.2.2.txt2');
}

sub _createEmptyFile
{
    my ($dirname, $filename) = @_;
    make_path($dirname);
    my $openres = open(my $fh, '>', catfile($dirname, $filename));
    if (not defined($openres))
    {
        croak("\ncan not open file $filename: $!\n");
    }
    close($fh);
}

END
{
    remove_tree($testDataFolder);
}
