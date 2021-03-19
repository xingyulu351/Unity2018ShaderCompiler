package Platform2Player;
use strict;

use warnings;

use Test::More;

sub Register
{
    Main::RegisterTarget(
        "Platform2Player",
        {
            windowsbuildfunction => \&BuildPlatform2Player,
            windowspreparefunction => \&PreparePlatform2Player,
            options => {
                "codegen" => ["debug", "release"],
                "unityDeveloperBuild" => [0, 1],
                "incremental" => [0, 1],
                "zipresults" => [0, 1],
                "scriptingBackend" => ["default", "mono", "il2cpp"],
                "abvsubset" => [0, 1]
            }
        }
    );
    Main::RegisterTarget(
        "Platform2Player",
        {
            macbuildfunction => \&BuildPlatform2Player,
            macpreparefunction => \&PreparePlatform2Player,
            options => {
                "codegen" => ["debug", "release"],
                "unityDeveloperBuild" => [0, 1],
                "incremental" => [0, 1],
                "zipresults" => [0, 1],
                "scriptingBackend" => ["default", "mono", "il2cpp"],
                "abvsubset" => [0, 1]
            }
        }
    );
    Main::RegisterTarget(
        "Platform2Player",
        {
            linuxbuildfunction => \&BuildPlatform2Player,
            linuxpreparefunction => \&PreparePlatform2Player,
            options => {
                "codegen" => ["debug", "release"],
                "unityDeveloperBuild" => [0, 1],
                "incremental" => [0, 1],
                "zipresults" => [0, 1],
                "scriptingBackend" => ["default", "mono", "il2cpp"],
                "abvsubset" => [0, 1]
            }
        }
    );
}

sub PreparePlatform2Player
{
    print('PreparePlatform2XPlayer got called');
}

sub BuildPlatform2Player
{
    print('BuildPlatform2XPlayer got called');
}

1;
