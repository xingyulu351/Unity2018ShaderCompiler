use strict;
use File::Basename qw(dirname basename);
use File::Spec;

my $mydir = File::Spec->rel2abs(dirname($0));
my $root = File::Spec->rel2abs(dirname($0) . "/../..");
use lib File::Spec->rel2abs(dirname($0) . "/../..") . '/Tools/Build';

use File::Copy;

(chdir "$mydir") == 1 or die "could not change directory to $mydir";
if ($^O eq 'MSWin32')
{
    copy("bin\\Debug\\UnderlyingModel.dll", "..\\UnityDocBrowser\\Assets\\Editor") or die "Post build copy failed on PC: $!";
    copy("bin\\Debug\\Unity.Cecil.dll", "..\\UnityDocBrowser\\Assets\\Editor") or die "Post build copy failed on PC: $!";
}
else
{
    copy("bin/Debug/UnderlyingModel.dll", "../UnityDocBrowser/Assets/Editor/.") or die "Post build copy failed on Mac: $!";
    copy("bin/Debug/Unity.Cecil.dll", "../UnityDocBrowser/Assets/Editor/.") or die "Post build copy failed on Mac: $!";
}
