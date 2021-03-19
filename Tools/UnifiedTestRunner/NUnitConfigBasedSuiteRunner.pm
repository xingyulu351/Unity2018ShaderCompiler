package NUnitConfigBasedSuiteRunner;

use parent NUnitSuiteRunnerBase;

use warnings;
use strict;

use StringUtils qw (isNullOrEmpty);
use Carp qw (croak);
use File::Spec::Functions qw (canonpath catfile);
use Dirs;
use CSProj;
use VSSolution;
use Scalar::Util 'blessed';
use lib Dirs::external_root();
use List::MoreUtils qw(uniq);
use Option;

my $root = Dirs::getRoot();

my $suiteOptions = [
    ['testresultsxml:s', 'Specifies an output xml file for test results.'],
    [
        'testresultsformat:s',
        'Choose which nunit format the xml should be in',
        {
            defaultValue => 'nunit3',
            allowedValues => ['nunit3', 'nunit2']
        }
    ],
    [
        'domain:s',
        'AppDomain Usage for tests',
        {
            allowedValues => ['None', 'Single', 'Multiple'],
            defaultValue => 'None'
        }
    ],
    [
        'trace:s',
        'Set internal trace level',
        {
            allowedValues => ['Off', 'Error', 'Warning', 'Info', 'Verbose'],
        }
    ],
    [
        'apartment:s',
        'Apartment for running tests',
        {
            allowedValues => ['MTA', 'STA'],
            defaultValue => 'MTA',
        }
    ],
    ['basepath:s', 'Base path to be used when loading the assemblies.'],
    ['timeout:i', ' Set timeout for each test case in milliseconds.'],
    [
        'framework:s',
        'Framework version to be used for tests.',
        {
            allowedValues => ['3.5', '4.0', '4.5', '...']
        }
    ],
    ['stoponerror', ' Stop after the first test failure or error.'],
    ['cleanup', 'Erase any leftover cache files and exit'],
    ['privatebinpath:s@', 'Additional directories to be probed when loading assemblies'],
    ['assetpipelinev2', 'Enable Asset Import Pipeline V2 backend for the test run.']
];

sub new
{
    my ($pkg) = @_;
    return $pkg->SUPER::new(suiteOptions => $suiteOptions);
}

sub usage
{
    my ($this) = @_;
    my $config = $this->getConfig();
    my $help = $config->{help};
    return %{$help};
}

sub doRun
{
    my ($this) = @_;
    $this->verifyConfig();
    $this->prepareEnvironmentVariables();
    $this->SUPER::doRun();
}

sub getConfig
{
    croak('getConfig must be implemented in subclasses');
}

sub getName
{
    my ($this) = @_;
    my $config = $this->getConfig();
    return $config->{name};
}

sub getPlatform
{
    my ($this) = @_;
    my $platform = $this->getConfigValue('platform');
    if (not defined($platform))
    {
        $platform = $this->getName();
    }
    return $platform;
}

sub buildDependencies
{
    my ($this) = @_;
    if ($this->{dependencies_built})
    {
        return;
    }

    my @projectsToCleanup = $this->getCSProjectsToCleanup();
    for my $csProject (@projectsToCleanup)
    {
        $this->cleanCSProject($csProject);
    }

    my @csProjects = $this->getCSProjectsToBuild();
    for my $csProject (@csProjects)
    {
        my $targetFramework = $this->getTargetFrameworkVersion();
        $this->buildCSProject($csProject, $targetFramework);
    }

    $this->{dependencies_built} = 1;
}

sub getCSProjectsToBuild
{
    my ($this) = @_;
    my @csProjects = $this->getCSDependencies();
    my $testLister = canonpath("$root/Tests/Unity.TestLister/Unity.TestLister.csproj");
    @csProjects = ($testLister, @csProjects);
    my $inputFiles = $this->getConfigValue('nunitInputFiles');
    if (defined($inputFiles))
    {
        foreach my $file (@{$inputFiles})
        {
            if ($file =~ m/csproj$/is or $file =~ m/sln$/is)
            {
                push(@csProjects, $file);
            }
        }
    }
    return @csProjects;
}

sub prepareEnv
{
    my ($this) = @_;
    my $prepareEnv = $this->getConfigValue('prepareEnv');
    if ($prepareEnv)
    {
        &{$prepareEnv}();
    }
}

sub getIncludes
{
    my ($this) = @_;
    return $this->getArrayConfigValue('includes');
}

sub getExcludes
{
    my ($this) = @_;
    return $this->getArrayConfigValue('excludes');
}

sub getCSDependencies
{
    my ($this) = @_;
    return $this->getArrayConfigValue('csDependencies');
}

sub getCSProjectsToCleanup
{
    my ($this) = @_;
    return $this->getArrayConfigValue('csProjectToCleanup');
}

sub getTargetFrameworkVersion
{
    my ($this) = @_;
    my $config = $this->getConfig();
    return $config->{targetFrameworkVersion};
}

sub getNUnitArgs
{
    my ($this) = @_;
    return $this->getArrayConfigValue('nunitArgs');
}

sub getMonoArgs
{
    my ($this) = @_;
    return $this->getArrayConfigValue('monoArgs');
}

sub getDisableResolveDlls
{
    my ($this) = @_;
    return $this->getConfigValue('disableResolveDlls');
}

sub getConfigValue
{
    my ($this, $key) = @_;
    my $config = $this->getConfig();
    return $config->{$key};
}

sub getArrayConfigValue
{
    my ($this, $key) = @_;
    my $value = $this->getConfigValue($key);
    if (defined($value))
    {
        return @{$value};
    }
    return ();
}

sub getEnvironment
{
    my ($this) = @_;
    my $env = $this->getConfigValue('environment');
    if (not defined($env))
    {
        return $this->SUPER::getEnvironment();
    }
    return $env;
}

sub verifyConfig
{
    my ($this) = @_;
    my $platform = $this->getPlatform();
    if (isNullOrEmpty($platform))
    {
        croak('platform is not specified');
    }

    my $name = $this->getName();
    if (isNullOrEmpty($name))
    {
        croak('name must be specified');
    }

    my @inputFiles = $this->getDllNames();
    if (scalar(@inputFiles) == 0)
    {
        croak('nunitInputFiles is must be specified');
    }

    return $platform;
}

sub prepareEnvironmentVariables
{
    my ($this) = @_;

    if ($this->getOptionValue('assetpipelinev2'))
    {
        $ENV{'UNITY_USE_ASSET_PIPELINE_V2'} = 1;
    }
}

sub getDllNames
{
    my ($this) = @_;
    my $inputFiles = $this->getConfigValue('nunitInputFiles');
    if (not defined($inputFiles))
    {
        return ();
    }
    my @dlls;
    my @inputFiles = @{$inputFiles};
    if ($this->getDisableResolveDlls())
    {
        return @inputFiles;
    }

    foreach my $file (@inputFiles)
    {
        if ($file =~ m/csproj$/is)
        {
            my $csProj = new CSProj($file);
            push(@dlls, $csProj->getAssemblyFullPath());
        }
        elsif ($file =~ m/sln$/is)
        {
            my $sln = new VSSolution($file);
            my @libs = $sln->getAllClassLibraries();
            my @pathes = map { $_->getAssemblyFullPath() } @libs;
            push(@dlls, @pathes);
        }
        else
        {
            push(@dlls, $file);
        }
    }

    return uniq(@dlls);
}

1;
