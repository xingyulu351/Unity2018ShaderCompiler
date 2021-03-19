package RunTests;

use warnings;
use strict;

use Test::More;

use File::Spec::Functions qw (catfile);
use File::Temp qw (tempdir tempfile);

use Carp qw (croak);
use Exporter 'import';

use FindBin qw ($Bin);
use lib "$Bin/../../";
use Dirs;
use lib Dirs::external_root();
use JSON;
use File::pushd;
use FileUtils;
use English qw (-no_match_vars);

our @EXPORT_OK = qw [runTests isEmptyFolder fetchTrackedRepo createTestPlan getJSONReport];

sub runTests
{
    my @args = @_;
    my $artifactsPath = tempdir();
    my $exitCode = system('perl', 'test.pl', "--artifacts_path=$artifactsPath", '-loglevel=none', '--hoarder-uri=null', @args);
    diag("artifacts path: $artifactsPath");
    return ($exitCode, $artifactsPath);
}

sub isEmptyFolder
{
    my $dirname = shift();
    opendir(my $dh, $dirname) or croak("Not a directory $dirname");
    return scalar(grep { $_ ne "." && $_ ne ".." } readdir($dh)) == 0;
}

sub fetchTrackedRepo
{
    my ($repoDir) = @_;
    my $d = pushd(Dirs::getRoot());
    my $exitCode = system("perl build.pl repos fetch $repoDir");
    if ($exitCode != 0)
    {
        croak("failed to fetch $repoDir");
    }
}

sub createTestPlan
{
    my (@tests) = @_;
    my ($fh, $tempFileName) = tempfile();
    my $closeRes = close($fh);
    if (not $closeRes)
    {
        croak("Can not close the file $tempFileName. $OS_ERROR\n");
    }
    FileUtils::writeAllLines($tempFileName, @tests);
    return $tempFileName;
}

sub getJSONReport
{
    my ($artifactsPath) = @_;
    my $jsonReportFile = catfile($artifactsPath, 'TestReportData.json');
    my $text = join(q (), FileUtils::readAllLines($jsonReportFile));
    return jsonToObj($text);
}

1;
