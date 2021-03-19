package Platform1Player;
use strict;

use warnings;

use Test::More;

sub Register
{
    Main::RegisterTarget(
        "Platform1Player",
        {
            windowsbuildfunction => \&BuildPlatform1Player,
            windowspreparefunction => \&PreparePlatform1Player,
            disposefunction => \&Dispose,
            testsettingfunction => \&GetTestSettings,
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
        "Platform1Player",
        {
            macbuildfunction => \&BuildPlatform1Player,
            macpreparefunction => \&PreparePlatform1Player,
            disposefunction => \&Dispose,
            testsettingfunction => \&GetTestSettings,
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
        "Platform1Player",
        {
            linuxbuildfunction => \&BuildPlatform1Player,
            linuxpreparefunction => \&PreparePlatform1Player,
            dispose => \&Dispose,
            testsettingfunction => \&GetTestSettings,
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

sub PreparePlatform1Player
{
    print('PreparePlatform1Player got called');
}

sub BuildPlatform1Player
{
    print('BuildPlatform1Player got called');
}

sub Dispose
{
    print('Dispose got called');
}

sub GetTestSettings
{
    my %result;
    $result{'platform1Setting1'} = 'platform1Setting1Value';
    $result{'platform1Setting2'} = 'platform1Setting2Value';
    return \%result;
}

1;
