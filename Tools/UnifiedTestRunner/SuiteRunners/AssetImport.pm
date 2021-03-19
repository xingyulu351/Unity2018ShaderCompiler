package SuiteRunners::AssetImport;

use parent UnityTestFrameworkSuiteRunnerBase;

use warnings;
use strict;

my $root = Dirs::getRoot();
my $defaultProjectListFileName = 'Tests/3DAssetImportTest/projectlist.txt';

my $suiteOptions = [
    ['testresultsxml:s', "Test results xml file for old Katana builder"],
    ['projectlist:s', "Project list filename", { defaultValue => $defaultProjectListFileName }],
    ['testprojects:s@', "Name(s) of the test projects to run"],
    ['categories:s@', "Filter tests by categories e.g. 'ABV'"],
    [
        'platform:s',
        'Platform to run the tests on',
        {
            defaultValue => 'editmode',
            allowedValues => ['editmode']
        }
    ],
    [
        'scripting-backend:s',
        'Scripting backend to execute test on.',
        {
            defaultValue => 'Mono2x',
            allowedValues => ['Mono2x', 'IL2CPP', 'WinRTDotNET']
        }
    ],
    [
        'api-profile:s',
        'Api profile to use.',
        {
            allowedValues => ['NET_2_0', 'NET_2_0_Subset', 'NET_4_6', 'NET_Standard_2_0']
        }
    ],
    ['use-latest-scripting-runtime-version', 'Force the editor to use latest scripting runtime.'],
    ['use-legacy-scripting-runtime-version', 'Force the editor to use legacy scripting runtime.'],
    ['assetpipelinev2', 'Enable Asset Import Pipeline V2 backend for the test run.']
];

sub new
{
    my ($pkg) = @_;
    my $this = $pkg->SUPER::new(suiteOptions => $suiteOptions);
    $this->{testresultsxml} = [];
    return $this;
}

sub usage
{
    my ($this) = @_;
    return (
        description => 'Editor tests.',
        url => 'https://confluence.hq.unity3d.com/display/DEV/Editor+Tests',
        prerequisites => 'perl build.pl repos fetch Tests/3DAssetImportTest/build',
        examples => [
            {
                description => "Run test from 'Undo' and 'Animations' projects only",
                command => '--testprojects=Undo  --testprojects=Animation'
            },
            { description => 'Run ABV editor tests', command => '--categories=ABV' }
        ],
    );
}

sub getName
{
    return 'assetimport';
}

1;
