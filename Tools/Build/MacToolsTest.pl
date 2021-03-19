package MacToolsTest;
use strict;
use File::Basename qw( dirname basename );
use File::Spec;
use File::Path qw( mkpath rmtree );
use Cwd;
use File::stat;

use lib (File::Spec->rel2abs(dirname(__FILE__)) . '/../../External/Perl/lib');
use base qw(Test::Unit::TestCase);
use File::chdir;

use MacTools qw( LinkFlattenedDirs );

my $root;

# Each function must cleanup area before starting, but each function must local chdir to tmp
sub Cleanup
{
    $root = File::Spec->rel2abs(dirname(__FILE__));
    rmtree "$root/tmp";
    mkdir "$root/tmp";
}

END
{
    rmtree "$root/tmp";
}

sub test_LinkFlattenedDirs_SymLinksWithAbsolutePaths
{
    my $self = shift();
    Cleanup();
    local $CWD = "tmp";

    mkpath "dir1/dir2/dir3";
    mkpath "dir1/dir4";

    open my $file1, "+>", "dir1/testfile1" or die("Failed to create testfile1: $!");
    open my $file2, "+>", "dir1/dir2/testfile2" or die("Failed to create testfile2: $!");
    open my $file3, "+>", "dir1/dir2/dir3/testfile3 with spaces in name" or die("Failed to create testfile3 with spaces in name: $!");
    open my $file4, "+>", "dir1/dir4/testfile4" or die("Failed to create testfile4: $!");

    close $file1;
    close $file2;
    close $file3;
    close $file4;

    mkpath "receiver";
    LinkFlattenedDirs("$root/tmp/dir1", "$root/tmp/receiver", 1);

    $self->assert(-e "receiver/testfile1");
    $self->assert(-e "receiver/testfile2");
    $self->assert(-e "receiver/testfile3 with spaces in name");
    $self->assert(-e "receiver/testfile4");
}

sub test_LinkFlattenedDirs_HardLinksWithAbsolutePaths
{
    my $self = shift();
    Cleanup();
    local $CWD = "tmp";

    mkpath "dir1/dir2/dir3";
    mkpath "dir1/dir4";
    mkpath "receiver";

    my %fileLinks = (
        "dir1/testfile1" => "receiver/testfile1",
        "dir1/dir2/testfile2" => "receiver/testfile2",
        "dir1/dir2/dir3/testfile3 with spaces in name" => "receiver/testfile3 with spaces in name",
        "dir1/dir4/testfile4" => "receiver/testfile4"
    );

    for my $source (keys %fileLinks)
    {
        open my $file, "+>", $source or die("Failed to create $source: $!");
        close $file;
    }

    LinkFlattenedDirs("$root/tmp/dir1", "$root/tmp/receiver");

    while (my ($source, $target) = each(%fileLinks))
    {
        my $sourceStat = stat($source);
        my $targetStat = stat($target);
        $self->assert(defined($sourceStat) && defined($targetStat) && $sourceStat->ino == $targetStat->ino);
    }
}

1;

#below is boilerplate code that makes it so you can run the tests in this file by just running the file, not require any testrunner booha
use Test::Unit::TestRunner;
use FindBin;
my $testrunner = Test::Unit::TestRunner->new();
my $tn = $FindBin::Script;
$tn =~ s/.pl//g;
$testrunner->start($tn);
