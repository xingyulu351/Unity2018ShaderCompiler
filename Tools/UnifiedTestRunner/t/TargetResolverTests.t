use warnings;
use strict;

use Test::More;
use File::Spec::Functions qw (canonpath);
use Dirs;
use lib Dirs::external_root();
use Test::Deep;

my $root = Dirs::getRoot();

my $resolverClassName = 'TargetResolver';
my @originalInc;

BEGIN:
{
    @originalInc = @INC;
    use_ok($resolverClassName);
}
can_ok($resolverClassName, 'resolve');

RestoredOriginalInc:
{
    my $executable = TargetResolver::resolve('editor', 'MSWin32');
    is_deeply(\@originalInc, \@INC);
}

EDITOR_WIN:
{
    my $expected_win_executable = canonpath("$root/build/WindowsEditor/Unity.exe");
    my $executable = TargetResolver::resolve('editor', 'MSWin32');
    is($executable, $expected_win_executable);
}

EDITOR_MAC:
{
    my $expected_mac_executable = canonpath("$root/build/MacEditor/Unity.app/Contents/MacOS/Unity");
    my $executable = TargetResolver::resolve('editor', 'darwin');
    is($executable, $expected_mac_executable);
}

STANDALONE_WIN:
{
    my $expected_win_executable = canonpath("$root/build/WindowsStandaloneSupport/Variations/win64_development_mono/WindowsPlayer.exe");
    my $executable = TargetResolver::resolve('standalone', 'MSWin32');
    is($executable, $expected_win_executable);
}

STANDALONE_WIN_VARIATION:
{
    my $variation = 'win32_development_mono';
    my $expected_win_executable = canonpath("$root/build/WindowsStandaloneSupport/Variations/$variation/WindowsPlayer.exe");
    my $executable = TargetResolver::resolve('standalone', 'MSWin32', $variation);
    is($executable, $expected_win_executable);
}

STANDALONE_MAC:
{
    my $expected_mac_executable = canonpath("$root/build/MacStandaloneSupport/Variations/macosx64_development_mono/UnityPlayer.app/Contents/MacOS/UnityPlayer");
    my $executable = TargetResolver::resolve('standalone', 'darwin');
    is($executable, $expected_mac_executable);
}

STANDALONE_MAC_VARIATION:
{
    my $variation = 'macosx64_development_ill2cpp';
    my $expected_mac_executable = canonpath("$root/build/MacStandaloneSupport/Variations/$variation/UnityPlayer.app/Contents/MacOS/UnityPlayer");
    my $executable = TargetResolver::resolve('standalone', 'darwin', $variation);
    is($executable, $expected_mac_executable);
}

done_testing();
