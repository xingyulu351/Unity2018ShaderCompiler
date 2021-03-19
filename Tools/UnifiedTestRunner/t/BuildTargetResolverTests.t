use warnings;
use strict;

use Test::More;
use File::Spec::Functions qw (canonpath catfile);
use Dirs;
use lib Dirs::external_root();

use JSON;
use Test::MockModule;
use Test::Trap;

my $utrRoot = Dirs::UTR_root();

BEGIN { use_ok('BuildTargetResolver'); }

can_ok('BuildTargetResolver', 'resolveModule');

ResolveModule_Platform1_ResovlesCorrectModule:
{
    my $config = <<END_MESSAGE;
{
    "search_patterns" : [
        "Tools/UnifiedTestRunner/t/TestData/BuildTargets/*/*.pm"
    ]
}
END_MESSAGE
    my $module = resolveModule('Platform1', $config);
    is($module, canonpath("$utrRoot/t/TestData/BuildTargets/Platform1/Platform1Player.pm"));
}

ResolveModule_Platform2_ResovlesCorrectModule:
{
    my $config = <<END_MESSAGE;
{
    "search_patterns" : [
        "Tools/UnifiedTestRunner/t/TestData/BuildTargets/*/*.pm"
    ]
}
END_MESSAGE
    my $module = resolveModule('Platform2', $config);
    is($module, canonpath("$utrRoot/t/TestData/BuildTargets/Platform2/Platform2Player.pm"));
}

ResolveModule_PlatformIsSpecifiedInPlatformOverrides_ResovlesCorrectModule:
{
    my $config = <<END_MESSAGE;
{
    "search_patterns" : [
        "Tools/UnifiedTestRunner/t/TestData/BuildTargets/PlatformX/*.pm"
    ],
    "platform2" : ["Tools/UnifiedTestRunner/t/TestData/BuildTargets/Platform2/*.pm"]
}
END_MESSAGE
    my $module = resolveModule('Platform2', $config);
    is($module, canonpath("$utrRoot/t/TestData/BuildTargets/Platform2/Platform2Player.pm"));
}

ResolveModule_NonExistsingPlatform_ReturnsUndef:
{
    my $config = <<END_MESSAGE;
{
    "search_patterns" : []
}
END_MESSAGE
    my $module = resolveModule('Platform1', $config);
    is($module, undef);
}

ResolveTarget_ExistingTaregtisSpecified_ReturnsIt:
{
    my $config = <<END_MESSAGE;
{
    "search_patterns" : [],
    "platform2" : ["Tools/UnifiedTestRunner/t/TestData/BuildTargets/Platform2/*.pm"]
}
END_MESSAGE

    my $target = resolveTarget('platform2', $config);
    is($target->getName(), 'Platform2Player');
    is($target->getPlatform(), 'platform2');
    my $utrRoot = Dirs::UTR_root();
    my $module = catfile($utrRoot, 't/TestData/BuildTargets/Platform2/Platform2Player.pm');
    is($target->getModule(), $module);
}

ResolveTarget_TargetNameDoesNotMatchPlatformButTargetExists_ReturnsIt:
{
    my $config = <<END_MESSAGE;
{
    "search_patterns" : [],
    "platform2" : ["Tools/UnifiedTestRunner/t/TestData/BuildTargets/Platform1/Platform1Player.pm"]
}
END_MESSAGE

    my $target = resolveTarget('platform2', $config);
    is($target->getName(), 'Platform1Player');
    is($target->getPlatform(), 'platform2');
    my $utrRoot = Dirs::UTR_root();
    my $module = catfile($utrRoot, 't/TestData/BuildTargets/Platform1/Platform1Player.pm');
    is($target->getModule(), $module);
}

done_testing();

sub resolveTarget
{
    my ($platform, $config) = @_;
    my $resolverModuleRef = makeResolverMock($config);
    return BuildTargetResolver::resolveTarget($platform);
}

sub resolveModule
{
    my ($platform, $config) = @_;
    my $resolverModuleRef = makeResolverMock($config);
    return BuildTargetResolver::resolveModule($platform);
}

sub makeResolverMock
{
    my ($config) = @_;
    my $resolverModule = new Test::MockModule('BuildTargetResolver');
    $resolverModule->mock(
        _readConfig => sub
        {
            return jsonToObj($config);
        }
    );
    return $resolverModule;
}
