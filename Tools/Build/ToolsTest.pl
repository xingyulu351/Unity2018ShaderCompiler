package ToolsTest;
use strict;
use File::Basename qw( dirname basename );
use File::Spec;
use File::Path qw( mkpath rmtree );
use Cwd;

use lib (File::Spec->rel2abs(dirname(__FILE__)) . '/../../External/Perl/lib');
use base qw(Test::Unit::TestCase);
use File::chdir;

use Tools qw( UCopy );

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

sub CreateFile
{
    my $filename = shift;
    open my $file1, "+>", $filename or die("Failed to create $filename $!");
    print $file1 "dummycontent";
    close $file1;
}

sub test_UCopy_TargetFileOverwritten
{
    my $self = shift();
    Cleanup();
    local $CWD = "tmp";

    CreateFile("testfile1");
    CreateFile("testfile2");

    UCopy("testfile1", "testfile2");

    open my $file2, "<", "testfile2" or die("Failed to open testfile2: $!");

    my $file2Contents = <$file2>;

    $self->assert($file2Contents eq "dummycontent");
}

sub test_UCopy_CopyDirectoryRecursively
{
    my $self = shift();
    Cleanup();
    local $CWD = "tmp";

    mkpath "dir1/dir2";

    CreateFile("dir1/testfile1");
    CreateFile("dir1/dir2/testfile2");

    my $result = UCopy("dir1", "dir3");
    $self->assert($result == 0);

    # Check if source dir1 is identical to target dir3
    $self->assert(-e "dir3/testfile1");
    $self->assert(-e "dir3/dir2/testfile2");
}

sub test_UCopy_HandleTargetIsHardLinkOfSameFile
{
    my $self = shift();
    Cleanup();
    local $CWD = "tmp";

    # Test does not apply on windows
    return if $^O eq "MSWin32";

    CreateFile("testfile1");

    mkdir "newcontainer";
    chdir "newcontainer";
    system("ln ../testfile1 testfile1");
    chdir "..";
    my $result = UCopy("testfile1", "newcontainer/testfile1");

    $self->assert($result == 0);
}

sub test_UCopy_UnixPathSeperatorConvertedForWindows
{
    my $self = shift();
    Cleanup();
    local $CWD = "tmp";

    mkpath "dir1/dir2/dir3";

    CreateFile("testfile1");

    UCopy("$root/tmp/testfile1", "$root/tmp/dir1/dir2/dir3/testfile1");

    $self->assert(-e "$root/tmp/dir1/dir2/dir3/testfile1");
}

sub test_UCopy_CanCopyWithWildCard
{
    return if ($^O eq "MSWin32");    #on windows we dont support wildcard UCopy yet.
    my $self = shift;
    Cleanup();
    local $CWD = "tmp";

    CreateFile("somefile");
    mkpath("somedir");

    UCopy("$root/tmp/*", "$root/tmp/dest/");

    $self->assert(-e "$root/tmp/dest/somefile");
    $self->assert(-e "$root/tmp/dest/somedir");
}

1;

#below is boilerplate code that makes it so you can run the tests in this file by just running the file, not require any testrunner booha
use Test::Unit::TestRunner;
use FindBin;
my $testrunner = Test::Unit::TestRunner->new();
my $tn = $FindBin::Script;
$tn =~ s/.pl//g;
$testrunner->start($tn);
