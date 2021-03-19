package SuiteRunners::IL2CPP;

use parent NUnitConfigBasedSuiteRunner;

use File::Spec::Functions qw (catdir);

use warnings;
use strict;

use Dirs;
use Environments::MonoBleedingEdge;

my $il2cppSln = catdir(Dirs::getRoot(), 'External/il2cpp/il2cpp/il2cpp.sln');
my @suiteOptions = Option::fromDescriptions(
    [
        'platform:s',
        'Platform to run tests on',
        {
            allowedValues => ['host', 'MacOSX', 'WebGL', 'WindowsDesktop', 'WinRT'],
            defaultValue => 'host'
        }
    ]
);

sub getConfig
{
    my ($this) = @_;
    my $config = {
        name => 'il2cpp',
        prepareEnv => sub
        {
            ## no critic (RequireLocalizedPunctuationVars)
            $this->jam('UnityLinker', 'MapFileParser');
            $ENV{IL2CPP_TEST_PLATFORM} = $this->getOptionValue('platform');
        },
        nunitInputFiles => [$il2cppSln],
        targetFrameworkVersion => '4.5',
        monoArgs => ['--runtime=v4.0'],
        excludes => ['ExcludeOnBuildFarm'],
        environment => new Environments::MonoBleedingEdge(),
        disableResolveDlls => 1,
        suiteOptions => \@suiteOptions,
        help => {
            description => "il2cpp tests from the $il2cppSln",
            examples => [
                { description => "Run all tests matching 'Cecil'", command => '--testfilter=Cecil' },
                { description => "List all matching 'Cecil'", command => '--list --testfilter=Cecil' }
            ]
        }
    };
}

1;
