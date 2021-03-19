use warnings;
use strict;

use Test::More;
use File::Spec::Functions qw (canonpath);

use SystemCall;
use Dirs;

my $root = Dirs::getRoot();

BEGIN { use_ok('Environment'); }

BASIC:
{
    can_ok('Environment', qw    (new getName runDotNetProgram setBuildEngine getBuildEngine runNUnitProject));
    my $env = Environment->new();
    isa_ok($env, 'Environment');
    isa_ok($env->getSystemCall(), 'SystemCall');
    $env->setSystemCall(undef);
    is($env->getSystemCall(), undef);

    isa_ok($env->getBuildEngine(), 'BuildEngines::XBuild');
    $env->setBuildEngine(undef);
    is($env->getBuildEngine(), undef);
}

CAN_GET_EDITOR_PATH:
{
    my $editorTargetPath = Environment::getEdititorTargetPath();
    if ($^O eq 'MSWin32')
    {
        is($editorTargetPath, canonpath("$root/build/WindowsEditor/Unity.exe"));
    }
    if ($^O eq 'darwin')
    {
        is($editorTargetPath, canonpath("$root/build/MacEditor/Unity.app/Contents/MacOS/Unity"));
    }
    if ($^O eq 'linux')
    {
        is($editorTargetPath, canonpath("$root/build/LinuxEditor/Unity"));
    }
}

CAN_GET_CGBATCH_PATH:
{
    my $shaderCompilerTargetPath = Environment::getCgBatchTargetPath();
    if ($^O eq 'MSWin32')
    {
        is($shaderCompilerTargetPath, canonpath("$root/build/WindowsEditor/Data/Tools/UnityShaderCompiler.exe"));
    }
    if ($^O eq 'darwin')
    {
        is($shaderCompilerTargetPath, canonpath("$root/build/MacEditor/Unity.app/Contents/Tools/UnityShaderCompiler"));
    }
    if ($^O eq 'linux')
    {
        is($shaderCompilerTargetPath, canonpath("$root/build/LinuxEditor/Data/Tools/UnityShaderCompiler"));
    }
}

done_testing();
