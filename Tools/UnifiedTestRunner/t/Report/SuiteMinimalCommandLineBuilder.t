use warnings;
use strict;

use RunContext;

use Dirs;
use lib Dirs::external_root();
use Test::More;
use TestHelpers::ArrayAssertions qw (isEmpty arraysAreEquivalent arrayDoesNotContain);

use Report::Presentation::SuiteMinimalCommandLineBuilder qw (extract);
use Test::Deep 're';
use Test::MockModule;

my $runnerMock = new Test::MockModule('Plugin');
$runnerMock->mock(_updateRunnerHelp => sub { return (); });

Extract_EmptyCommandLine_SuiteArgumentsAreEmpty:
{
    my %result = extract([]);
    my @keys = keys(%result);
    isEmpty(\@keys);
}

Extract_NoSuiteArguments_SuiteArgumentsAreEmpty:
{
    my %result = extract(['-suite=native']);
    ok(exists($result{'native'}));
    my @results = @{ $result{'native'} };
    isEmpty(\@results);
}

Extract_OneSuiteArgument_SuiteArgumentsContainIt:
{
    my %result = extract(['--suite=native', '--platform=Standalone']);
    my $suiteArgs = $result{'native'};
    arraysAreEquivalent($suiteArgs, ['--platform=Standalone']);
}

Extract_SuiteArgumentOntheSecondPlace_SuiteArgumentsContainIt:
{
    my %result = extract(['--platform=Standalone', '-suite=native']);
    my $suiteArgs = $result{'native'};
    arraysAreEquivalent($suiteArgs, ['--platform=Standalone']);
}

Extract_TwoSuitesDefaultArguments_EmptyArrayForBothSuites:
{
    my %result = extract(['-suite=native', '-suite=cgbatch']);

    ok(exists($result{'native'}));
    ok(exists($result{'cgbatch'}));

    my $nativeSuiteArgs = $result{'native'};
    isEmpty($nativeSuiteArgs);

    my $cgbatchSuiteRunnerArgs = $result{'cgbatch'};
    isEmpty($cgbatchSuiteRunnerArgs);
}

Extract_TwoSuitesAndSharedArgument_BothSuitesArgumentsContainSharedArgument:
{
    my %result = extract(['-suite=native', '-suite=runtime', '--platform=Standalone']);

    arraysAreEquivalent($result{'native'}, ['--platform=Standalone']);
    arraysAreEquivalent($result{'runtime'}, ['--platform=Standalone']);
}

Extract_ArgumentsValuesSeparatedBySpace_BothSuitesArgumentsContainSharedArgument:
{
    my %result = extract(['--suite', 'native', '--platform=Standalone']);
    arraysAreEquivalent($result{'native'}, ['--platform=Standalone']);
}

Extract_FlagOptionsContainMinus_SuiteArgumentsContainIt:
{
    my %result = extract(['--suite=native', '--skip-build-deps']);
    arraysAreEquivalent($result{'native'}, ['--skip-build-deps']);
}

Extract_OptionsKatanaSpecieficOptionsAreFilteredOut_DoesNotPutItToCommandLine:
{
    my %result = extract(
        [
            '-suite=native', '--platform=Standalone', '-tag=A', '-tag=B',
            '--testresultsxml=c:/somewhere/tr.xml', '-config=A', '-config-id=A', '-build-number=666',
            '--artifacts_path=blabla', '--testfilter=blabla'
        ]
    );
    arraysAreEquivalent($result{'native'}, ['--platform=Standalone', '--testfilter=blabla']);
}

Extract_Profile_SkipIt:
{
    my %result = extract(['--suite=native', '--profile=katana-native-editor', '--testfilter=blabla']);
    arrayDoesNotContain($result{'native'}, re('profile'));
}

Extract_HoardeUri_SkipIt:
{
    my %result = extract(['-suite=native', '--hoarder-uri=xxx']);
    my $suiteArgs = $result{'native'};
    arrayDoesNotContain($suiteArgs, [re('hoarder-uri.*')]);
}

Extract_OverrideIsSpecifed_UsesOverridedValue:
{
    my $overrides = [{ name => 'projectpath', value => 'overriden' }];
    my %result = extract(['-suite=editor', '--projectpath=project'], $overrides);
    arraysAreEquivalent($result{'editor'}, ['--projectpath=overriden']);
}

Extract_OverrideIsSpecifedButThereIsNoSuchOptionInMainCommandLine_UsesOverridedValue:
{
    my $overrides = [{ name => 'projectpath', value => "overrided" }];
    my %result = extract(['-suite=editor'], $overrides);
    arraysAreEquivalent($result{'editor'}, ['--projectpath=overrided']);
}

Extract_RealityCheckKatanaSpecificOptionsAreFilteredOut_DoesNotPutItToCommandLine:
{
    my %result = extract(
        [
            '--suite=integration',
            '--testtarget=Metro',
            '--env=monobe',
            '--testprojects=All',
            '--testresultsxml=C:\\buildslave/unity/build/Projects/CSharp/TestResult.xml',
            '--artifacts_path=C:\\buildslave/unity/build/build/ReportedArtifacts/',
            '-submitter=http://katana.hq.unity3d.com/',
            '-config=Test IntegrationTests - MetroBlue',
            '-config-id=proj0-Test IntegrationTests - MetroBlue',
            '-build-number=14551',
            '--smart-select',
            '--owner=yan',
            '-tag=Trunk',
            '-tag=Trunk-ABV',
            '-tag=Trunk-Nightly',
            '-tag=5.2',
            '-tag=5.2-ABV',
            '-tag=5.2-Nightly',
            '-tag=5.1',
            '-tag=5.1-ABV',
            '-tag=5.1-Nightly',
            '-tag=5.0',
            '-tag=5.0-ABV',
            '-tag=5.0-Nightly'
        ]
    );
    arraysAreEquivalent($result{'integration'}, ['--testtarget=Metro', '--environment=monobe', '--testprojects=All']);
}

done_testing();
