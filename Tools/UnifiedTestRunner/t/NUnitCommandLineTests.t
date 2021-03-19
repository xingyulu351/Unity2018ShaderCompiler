use warnings;
use strict;

use Test::More;
use File::Spec::Functions qw (canonpath);
use TestHelpers::ArrayAssertions qw (arraysAreEqual);
use Dirs;

my $root = Dirs::getRoot();

BEGIN { use_ok('NUnitCommandLine'); }

can_ok('NUnitCommandLine', 'build');
my $nunit = canonpath("$root/External/NUnit/nunit-console.exe");
my $dll1 = canonpath("/tmp/somepath1/myproj.dll");
my $dll2 = canonpath("/tmp/somepath2/myproj.dll");

build_OneDllPath_AddtsItToCommandLine:
{
    my @args = NUnitCommandLine::build(dlls => [$dll1]);

    arraysAreEqual(\@args, [$nunit, '--encoding=utf-8', $dll1]);
}

build_TwoDllPathes_AddtsItToCommandLine:
{
    my @args = NUnitCommandLine::build(dlls => [$dll1, $dll2]);

    arraysAreEqual(\@args, [$nunit, '--encoding=utf-8', $dll1, $dll2]);
}

build_DirectRunnerArgs_AddtsItToCommandLine:
{
    my @args = NUnitCommandLine::build(
        dlls => [$dll1],
        directRunnerArgs => ['-somearg']
    );

    arraysAreEqual(\@args, [$nunit, '--encoding=utf-8', $dll1, '-somearg']);
}

done_testing();
