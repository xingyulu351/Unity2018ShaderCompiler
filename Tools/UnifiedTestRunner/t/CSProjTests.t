use Test::More;

use warnings;
use strict;

use File::Spec::Functions qw (canonpath);

use Dirs;
use lib Dirs::external_root();
use Test::MockObject::Extends;
use Test::Trap;

my $utrRoot = Dirs::UTR_root();

Basic:
{
    BEGIN { use_ok('CSProj'); }
    can_ok('CSProj', 'new');
}

getAssemblyFullPath_ClassLibrary_RetunsExpectedPath:
{
    my $proj = new CSProj(canonpath("$utrRoot/t/TestData/csproj/dummy_proj.csproj"));
    my $dllPath = $proj->getAssemblyFullPath('Debug|AnyCPU');
    is($dllPath, canonpath("$utrRoot/t/TestData/csproj/bin/Debug/dummy.dll"));
}

getAssemblyFullPath_ConsoleApplication_RetunsExpectedPath:
{
    my $projectPath = canonpath("$utrRoot/t/TestData/ConsoleApp/ConsoleApp.csproj");
    my $proj = new CSProj($projectPath);
    my $exePath = $proj->getAssemblyFullPath('Debug|AnyCPU');
    is($exePath, canonpath("$utrRoot/t/TestData/ConsoleApp/bin/Debug/ConsoleApp.exe"));
}

getAssemblyFullPath_Config_RetunsExpectedPath:
{
    my $proj = new CSProj(canonpath("$utrRoot/t/TestData/csproj/dummy_proj.csproj"));
    my $dllPath = $proj->getAssemblyFullPath('Debug|AnyCPU');
    is($dllPath, canonpath("$utrRoot/t/TestData/csproj/bin/Debug/dummy.dll"));
}

getProjectType_ClassLibrary_ReturnsCSClassLibrary:
{
    my $proj = new CSProj(canonpath("$utrRoot/t/TestData/csproj/dummy_proj.csproj"));
    is($proj->getProjectType(), CSProj::PROJECT_TYPE_CS_CLASS_LIBRARY);
}

getProjectPath_ReturnsExpectedPath:
{
    my $projectPath = canonpath("$utrRoot/t/TestData/csproj/dummy_proj.csproj");
    my $proj = new CSProj($projectPath);
    is($proj->getProjectPath(), $projectPath);
}

getProjectType_ConsoleApp_ReturnsExeProjectType:
{
    my $proj = new CSProj(canonpath("$utrRoot/t/TestData/ConsoleApp/ConsoleApp.csproj"));
    is($proj->getProjectType(), CSProj::PROJECT_TYPE_CS_EXE);
}

getAssemblyFullPath_NotSupportedProjectType_Croaks:
{
    my $projectPath = canonpath("$utrRoot/t/TestData/ConsoleApp/ConsoleApp.csproj");
    my $proj = new Test::MockObject::Extends(new CSProj($projectPath));
    $proj->mock(getProjectType => sub { return CSProj::PROJECT_TYPE_UNKNOWN; });
    my @trap = trap
    {
        $proj->getAssemblyFullPath();
    };
    ok(index($trap->die, $projectPath) > 0);
}

getGUID_ReturnsProjectGUID:
{
    my $proj = new CSProj(canonpath("$utrRoot/t/TestData/ConsoleApp/ConsoleApp.csproj"));
    is($proj->getGUID(), 'BB5F6B9A-FAAC-4A94-96FC-72541B2C5D36');
}

done_testing();
