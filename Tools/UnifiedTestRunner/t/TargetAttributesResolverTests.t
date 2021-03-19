use Test::More;

use warnings;
use strict;

use TestHelpers::ArrayAssertions qw (isEmpty arraysAreEqual arraysAreEquivalent);

BEGIN { use_ok('TargetAttributesResolver') }

DEGENERATED_TESTS:
{
    can_ok('TargetAttributesResolver', 'new', 'resolve');
    my $emptyXml = '<?xml version="1.0"?><root />';
    my $resolver = TargetAttributesResolver->new($emptyXml);
    isa_ok($resolver, 'TargetAttributesResolver');
    is(undef, $resolver->resolve(undef), 'Result is undefined if requested target is undefined');
}

CAN_READ_TARGET_WITH_INCLUDES_AND_EXCLUDES_DIRECTLY_IN_TARGET:
{
    my $project = <<END_MESSAGE;
<?xml version="1.0"?>
<Project ToolsVersion="3.5" DefaultTargets="TestEditor" xmlns="http://schemas.microsoft.com/developer/msbuild/2003">
    <Target Name="TargetOne">
        <MSBuild Projects="$(MSBuildProjectFile)"
            Targets="RunTest"
            Properties="ExcludeCategory=A,B,C;IncludeCategory=D,E" />
    </Target>
</Project>
END_MESSAGE
    my $resolver = new TargetAttributesResolver($project);
    my $tartget = $resolver->resolve('TargetOne');
    is($tartget->getName(), 'TargetOne');
    is(join(' ', $tartget->getExcludes()), 'A B C');
    is(join(' ', $tartget->getIncludes()), 'D E');
}

CAN_RESOLVE_TARGET_WITH_EMPTY_INCLUEDES_AND_EXCLUDES:
{
    my $project = <<END_MESSAGE;
<?xml version="1.0"?>
<Project ToolsVersion="3.5" DefaultTargets="TestEditor" xmlns="http://schemas.microsoft.com/developer/msbuild/2003">
    <Target Name="TargetOne">
        <MSBuild Projects="$(MSBuildProjectFile)"
            Targets="RunTest"
            Properties="ExcludeCategory=;IncludeCategory=" />
    </Target>
</Project>
END_MESSAGE
    my $resolver = new TargetAttributesResolver($project);
    my $tartget = $resolver->resolve('TargetOne');
    is($tartget->getName(), 'TargetOne');
    is($tartget->getExcludes(), 0);
    is($tartget->getIncludes(), 0);
}

CAN_RESOLVE_TARTGET_WITHOUT_MS_BUILD_ELEMENTS_IN_IT:
{
    my $project = <<END_MESSAGE;
<?xml version="1.0"?>
<Project ToolsVersion="3.5" DefaultTargets="TestEditor" xmlns="http://schemas.microsoft.com/developer/msbuild/2003">
    <Target Name="T1">
    </Target>
</Project>
END_MESSAGE
    my $resolver = new TargetAttributesResolver($project);
    my $tartget = $resolver->resolve('T1');
    is($tartget->getName(), 'T1');
    is($tartget->getExcludes(), 0);
    is($tartget->getIncludes(), 0);
}

CAN_RESOLVE_TARGET_DEPENDENCIES:
{
    my $project = <<END_MESSAGE;
<?xml version="1.0"?>
<Project ToolsVersion="3.5" DefaultTargets="TestEditor" xmlns="http://schemas.microsoft.com/developer/msbuild/2003">
    <Target Name="T1" />
    <Target Name="T2" />
    <Target Name="T3" DependsOnTargets="T2; T1" />
</Project>
END_MESSAGE
    my $resolver = new TargetAttributesResolver($project);
    my $tartget = $resolver->resolve('T3');
    is($tartget->getName(), 'T3');
    my @dependencies = $tartget->getDependsOnTargets();
    is($dependencies[0]->getName(), 'T2');
    is($dependencies[1]->getName(), 'T1');
}

CAN_GET_ALL_TARGETS:
{
    my $project = <<END_MESSAGE;
<?xml version="1.0"?>
<Project ToolsVersion="3.5" DefaultTargets="TestEditor" xmlns="http://schemas.microsoft.com/developer/msbuild/2003">
    <Target Name="T1" />
    <Target Name="T2" />
    <Target Name="T3" DependsOnTargets="T2; T1" />
</Project>
END_MESSAGE
    my $resolver = new TargetAttributesResolver($project);
    my @allTargets = $resolver->getAllTargetNames();
    arraysAreEquivalent(\@allTargets, ['T1', 'T2', 'T3']);
}

RESOLVE_ATTRIBUTES_FROM_DEPENDANT_TARGETS:
{
    my $project = <<END_MESSAGE;
<?xml version="1.0"?>
<Project ToolsVersion="3.5" DefaultTargets="TestEditor" xmlns="http://schemas.microsoft.com/developer/msbuild/2003">

    <Target Name="T1">
        <MSBuild Projects="$(MSBuildProjectFile)"
            Targets="RunTest"
            Properties="ExcludeCategory=T1E1,T1E2,T1E3;IncludeCategory=T1I1,T1I2" />
    </Target>

    <Target Name="T2" DependsOnTargets="T1">
        <MSBuild Projects="$(MSBuildProjectFile)"
            Targets="RunTest"
            Properties="ExcludeCategory=T2E1,T2E2,T2E3;IncludeCategory=T2I1,T2I2" />
    </Target>

    <Target Name="T3" DependsOnTargets="T2">
        <MSBuild Projects="$(MSBuildProjectFile)"
            Targets="RunTest"
            Properties="ExcludeCategory=T3E1,T3E2,T3E3;IncludeCategory=T3I1,T3I2" />
    </Target>

    <Target Name="T4" DependsOnTargets="T3">
        <MSBuild Projects="$(MSBuildProjectFile)"
            Targets="RunTest"
            Properties="ExcludeCategory=T4E1,T4E2,T4E3;IncludeCategory=T4I1,T4I2" />
    </Target>

    <Target Name="T5" DependsOnTargets="T4; T3">
        <MSBuild Projects="$(MSBuildProjectFile)"
            Targets="RunTest"
            Properties="ExcludeCategory=T5E1,T5E2,T5E3;IncludeCategory=T5I1,T5I2" />
    </Target>
</Project>
END_MESSAGE
    my $resolver = new TargetAttributesResolver($project);
    my $tartget = $resolver->resolve('T5');

    my ($includes, $excludes) = TargetAttributesResolver::resolveDependencies($tartget);
    is(join(' ', @$includes), 'T1I1 T1I2 T2I1 T2I2 T3I1 T3I2 T4I1 T4I2 T5I1 T5I2');
    is(join(' ', @$excludes), 'T1E1 T1E2 T1E3 T2E1 T2E2 T2E3 T3E1 T3E2 T3E3 T4E1 T4E2 T4E3 T5E1 T5E2 T5E3');
}

CAN_RESOLVE_ATTRIBUTES_VIA_CALL_TARGET:
{
    my $project = <<END_MESSAGE;
<?xml version="1.0"?>
<Project ToolsVersion="3.5" DefaultTargets="TestEditor" xmlns="http://schemas.microsoft.com/developer/msbuild/2003">

    <Target Name="T1">
        <MSBuild Projects="$(MSBuildProjectFile)"
            Targets="RunTest"
            Properties="ExcludeCategory=T1E1,T1E2,T1E3;IncludeCategory=T1I1,T1I2" />
    </Target>

    <Target Name="T2" DependsOnTargets="T1">
        <MSBuild Projects="$(MSBuildProjectFile)"
            Targets="RunTest"
            Properties="ExcludeCategory=T2E1,T2E2,T2E3;IncludeCategory=T2I1,T2I2" />
    </Target>

    <Target Name="T3" DependsOnTargets="T2">
        <MSBuild Projects="$(MSBuildProjectFile)"
            Targets="RunTest"
            Properties="ExcludeCategory=T3E1,T3E2,T3E3;IncludeCategory=T3I1,T3I2" />
    </Target>

    <Target Name="T4" DependsOnTargets="T1">
        <CallTarget Targets="T3" />
        <CallTarget Targets="T2" />
    </Target>
</Project>
END_MESSAGE
    my $resolver = new TargetAttributesResolver($project);
    my $tartget = $resolver->resolve('T4');

    my ($includes, $excludes) = TargetAttributesResolver::resolveDependencies($tartget);
    is(join(' ', @$includes), 'T1I1 T1I2 T2I1 T2I2 T3I1 T3I2');
    is(join(' ', @$excludes), 'T1E1 T1E2 T1E3 T2E1 T2E2 T2E3 T3E1 T3E2 T3E3');
}

resolvePlatforms_NoPlatformSpecified_ReturnsEmptyList:
{
    my $project = <<END_MESSAGE;
<?xml version="1.0"?>
<Project ToolsVersion="3.5" DefaultTargets="TargetOne" xmlns="http://schemas.microsoft.com/developer/msbuild/2003">
    <Target Name="TargetOne">
        <MSBuild Projects="$(MSBuildProjectFile)"
            Targets="RunTest"
            Properties="" />
    </Target>
</Project>
END_MESSAGE
    my $resolver = new TargetAttributesResolver($project);
    my $tartget = $resolver->resolve('TargetOne');
    TargetAttributesResolver::resolveDependencies($tartget);
    my @platforms = $tartget->getRuntimePlatforms();
    isEmpty(\@platforms);
}

resolveDependencies_NoPlatformSpecified_ReturnsEmptyList:
{
    my $project = <<END_MESSAGE;
<?xml version="1.0"?>
<Project ToolsVersion="3.5" DefaultTargets="TargetOne" xmlns="http://schemas.microsoft.com/developer/msbuild/2003">
    <Target Name="TargetOne">
        <MSBuild Projects="$(MSBuildProjectFile)"
            Targets="RunTest"
            Properties="RuntimePlatforms=Platform1, Platform2" />
    </Target>
</Project>
END_MESSAGE
    my $resolver = new TargetAttributesResolver($project);
    my $tartget = $resolver->resolve('TargetOne');
    TargetAttributesResolver::resolveDependencies($tartget);
    my @platforms = $tartget->getRuntimePlatforms();
    arraysAreEqual(\@platforms, ['Platform1', 'Platform2']);
}

done_testing();
