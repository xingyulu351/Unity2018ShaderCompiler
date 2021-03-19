package GlobalOptions;

use warnings;
use strict;

use Option;
use SuiteRunners;

sub getAllTestSuites
{
    my @runners = SuiteRunners::getRunners();
    return map { $_->getName() } @runners;
}

my @descriptions = (
    ['help', "Prints top-level help. **Help is 2 level**. To see help for the suite, use `--help --suite=[value]`"],

    ['suite=s@', 'Name(s) of test suite to run', { allowedValues => [getAllTestSuites()] }],

    ['testfilter:s', 'Regexp test filter'],

    ['list', 'Print the test list wihtout running them'],

    ['debug', "Sets UNITY_GIVE_CHANCE_TO_ATTACH_DEBUGGER=1. As as result promt to attach debbuger will appear when editor or player are started."],

    [
        'build-only',
        'Only perform the building of the testing artifacts without running any tests. This option only makes sense for test suites where there exists an integrated build process.'
    ],

    [
        'loglevel:s',
        'Verbosity of output',
        {
            allowedValues => [
                ['minimal', 'Logs error and progress messages'],
                ['verbose', 'Logs all messages from a test runner and in an old good times'],
                ['none', 'Logs nothing']
            ],
            defaultValue => 'minimal'
        }
    ],

    [
        'artifacts_path:s',
        'The absolute path to the folder to store artifacts produced by a test run. By default set to random temporary folder. All the artifacts produced by a test run should be stored in this folder. If they are not - please report to #qa-frameworks'
    ],

    ['repeat:i', 'Repeat test execution n times', { defaultValue => 1 }],

    [
        'environment:s',
        'Environment is used to build & run C# projects',
        {
            allowedValues => [
                ['mono', "Use mono from 'External/Mono' to run C# assemblies. 'External/MonoBleedingEdge' to compile project"],
                ['win', 'msbuild/native windows environment are used to build & run C# projects'],
                ['monobe', "Use from `External/MonoBleedingEdge` to build and run C# projects"]
            ],
            defaultValue => 'mono'
        }
    ],

    ['profile:s', 'File with the argument list. If directory is omited then Tools/UnifiedTestRunner/Profiles will be searched for the specified profile'],

    [
        'runrules:s',
        'File defines rules to run a given test. Evolution "profile" agrument (in json format). See Tools/UnifiedTestRunner/Profiles for more examples'
    ],

    [
        'progress:s',
        'Progress tracking mode',
        {
            allowedValues => ['simple', 'null'],
            defaultValue => 'simple'
        }
    ],

    ['nocolor', "Displays console output without color"],

    [
        'hoarder-uri:s',
        'Specifies uri to submit test run results',
        {
            defaultValue => 'http://api.hoarder.qa.hq.unity3d.com/v1/utr',
            hidden => 1,
        }
    ],

    [
        'event-uri:s',
        'Specifies uri to submit test run results',
        {
            defaultValue => 'http://testlogger.qa.hq.unity3d.com/api',
            hidden => 1,
        }
    ],

    [
        'log-events-only',
        'If specified, utr will only log protocol event, insted of sending them to WS',
        {
            hidden => 1,
        }
    ],

    [
        'config:s',
        "Used for when data reported to Hoarder web-service. Katana build configuration name e.g. 'Test IntegrationTests - MetroBlue'",
        {
            hidden => 1
        }
    ],

    [
        'config-id:s',
        "Used for when data reported to Hoarder web-service. Katana build configuration unique id e.g. 'proj0-Test IntegrationTests - MetroBlue'",
        {
            hidden => 1
        }
    ],

    [
        'build-number:s',
        "Used for when data reported to Hoarder web-service. Katana build number",
        {
            hidden => 1
        }
    ],

    [
        'submitter:s',
        "Used for when data reported to Hoarder web-service. Data sumbitter identifier. e.g. http://katana-staging.hq.unity3d.com",
        {
            hidden => 1
        }
    ],

    [
        'owner:s',
        "User started the build",
        {
            hidden => 1
        }
    ],

    [
        'tag:s@',
        "Used for when data reported to Hoarder web-service. List of tags. e.g. ABV, Nightly, Unstable.",
        {
            hidden => 1
        }
    ],

    [
        'report-generation-test',
        "Used to generate fake test report to be able to quickly test katana json report page generation",
        {
            hidden => 1
        }
    ],

    [
        'build-isolation:s',
        'Build isolation mode',
        {
            allowedValues =>
                [['none', 'Build tests to default locations'], ['enabled', 'Build test binraries to isolated location to avoid mono/native environment mix'],],
            defaultValue => 'none'
        }
    ],
    [
        'build-mode:s',
        'Build mode',
        {
            allowedValues => [
                ['none', 'Do not compile runner and test assemblies'],
                ['minimal', 'Automatic incremental build'],
                ['full', 'Clean CSharpProjects.sln, rebuild each assembly']
            ],
            defaultValue => 'full'
        }
    ],
    ['skip-build-deps', 'Deprecated. Use --build-mode=none instead.'],

    # HACK: keep it here for now to allow utr to resolve suite wihtout importing
    # this option from the SuiteRunns/Graphics.pm
    # in case if command line looks like this: ./test --player-load-path=<path>
    # Will be fixed, then test.pl become more testable.
    ['player-load-path:s', undef, { hidden => 1 }],
    ['update-runner-options', undef, { hidden => 1 }],

    ['query-partitions', 'If specified UTR will query partions and exit', { hidden => 1 }],
    ['query-partitions-result:s', 'Specifies where to save query partions result json file.', { hidden => 1 }],
    [
        'parallel-tests-selector-uri:s',
        'Url used to query partitions for a given test katana builder',
        { hidden => 1, defaultValue => 'http://api.hoarder.qa.hq.unity3d.com/v1/ParallelTestsSelector' }
    ],
    ['query-partitions-timeout:s', 'How long to wait for a response when query partitions.', { hidden => 1, defaultValue => 30 }],
    ['query-partitions-retries:s', 'Number of attempts to query paritions.', { hidden => 1, defaultValue => 3 }],
    ['partition-count:s', 'Maximum number of partitions', { hidden => 1 }],
    ['partition-source:s', 'File name or url, contains json data with partions', { hidden => 1 }],
    ['partitions-id:s', 'Partition identifier used to query a web web-service', { hidden => 1 }],
    ['partition-index:s', 'Parition index used during parallel test execution.', { hidden => 1, defaultValue => undef }],
    ['zero-tests-are-ok:i', 'Do *not* consider suite with 0 executed tests as a failure', { defaultValue => 0 }],
);

sub getOptions
{
    return Option::fromDescriptions(@descriptions);
}

1;
