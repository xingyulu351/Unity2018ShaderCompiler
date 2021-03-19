package Plugin;

use warnings;
use strict;
use Carp qw (croak);
use File::Spec::Functions qw (canonpath);
use File::Basename qw (fileparse);
use File::Spec::Functions qw (catfile catdir);
use FileWatcher;
use Dirs;
use SuiteHelpPrinter;
use Option;
use Options;
use OptionsMerger;
use SuiteRunnerArtifactsNameBuilder;
use RunnerOptions;

sub new
{
    my ($package, %params) = @_;
    my $this = {
        parent => undef,
        artifacts => undef,
        environment => undef,
        runContext => undef,
        suiteOptions => _fromDescriptions($params{suiteOptions}),
        additionalOptions => $params{additionalOptions},
        optionsGenProj => $params{optionsGenProj},
        runnerOptions => $params{runnerOptions},
        options => undef,
        testRunnerLibProj => $params{testRunnerLibProj},
    };
    if (not $this->{runnerOptions})
    {
        $this->{runnerOptions} = [];
    }
    $this->{artifactsNameBuilder} = new SuiteRunnerArtifactsNameBuilder($this);
    return bless($this, $package);
}

sub getName
{
    croak("Method 'getName' must be implemented in subclasses");
}

sub getSuiteOptions
{
    my ($this) = @_;
    return $this->{suiteOptions};
}

sub getOptions
{
    my ($this) = @_;
    $this->_prepareOptions();
    return $this->{options};
}

sub _prepareOptions
{
    my ($this) = @_;
    if ($this->{options})
    {
        return;
    }

    my $runnerOptionsFileName = $this->_getRunnerOptionsFileName();
    if (-e $runnerOptionsFileName)
    {
        my @runnerOptions = RunnerOptions::readFromFile($runnerOptionsFileName);
        $this->{runnerOptions} = \@runnerOptions;
    }
    elsif ($this->{optionsGenProj})
    {
        my $optionsFile = $this->updateRunnerHelp();
        my @runnerOptions = RunnerOptions::readFromFile($optionsFile);
        $this->{runnerOptions} = \@runnerOptions;
    }
    my $options = OptionsMerger::merge($this->{suiteOptions}, $this->{runnerOptions});
    if ($this->{additionalOptions})
    {
        $options = OptionsMerger::merge($options, $this->{additionalOptions});
    }

    $this->{allOptionsWithoutNamespace} = $options;
    my @optionsInNamespace;
    foreach my $o (@{$options})
    {
        my $clone = $o->clone();
        $clone->moveToNamespace($this->getName());
        push(@optionsInNamespace, $clone);
    }
    $this->{options} = [@{$options}, @optionsInNamespace];

}

sub getOptionsWithoutNamespace
{
    my ($this) = @_;
    $this->_prepareOptions();
    return @{ $this->{allOptionsWithoutNamespace} };
}

sub registerArtifact
{
    my ($this, $artifact) = @_;
    if (not defined($this->{artifacts}))
    {
        return;
    }
    my $suite = $this->getName();
    $this->{artifacts}->register($suite, $artifact);
}

sub setArtifacts
{
    my ($this, $value) = @_;
    $this->{artifacts} = $value;
}

sub getArtifacts
{
    my ($this, $value) = @_;
    return $this->{artifacts};
}

sub setRunContext
{
    my ($this, $value) = @_;
    $this->{runContext} = $value;
    return $this->{runContext};
}

sub getRunContext
{
    my ($this) = @_;
    return $this->{runContext};
}

sub getOptionValue
{
    my ($this, $name, $default_value) = @_;
    my $value = $this->{runContext}->resolve($name, $this->getName());
    if (not defined($value))
    {
        return $default_value;
    }
    return $value;
}

sub setArtifactsNameBuilder
{
    my ($this, $value) = @_;
    $this->{artifactsNameBuilder} = $value;
}

sub getArtifactsNameBuilder
{
    my ($this) = @_;
    return $this->{artifactsNameBuilder};
}

sub printLogMessage
{
    my ($message, $this, $filename) = @_;
    if (not defined($message))
    {
        return;
    }
    Logger::verbose(src => $filename, msg => $message);
}

sub printLogErrorMessage
{
    my ($message, $this, $filename) = @_;
    if (not defined($message))
    {
        return;
    }
    Logger::error(src => $filename, msg => $message);
}

sub getArtifactsPath
{
    my ($this) = @_;
    if (not defined($this->{runContext}))
    {
        return;
    }
    return canonpath($this->{runContext}->getArtifactsPath());
}

sub callSystem
{
    my ($this, @params) = @_;
    my $environment = $this->getEnvironment();
    my $systemCall = $environment->getSystemCall();
    $systemCall->execute(@params);
}

sub executeAndWatchFiles
{
    my ($this, %params) = @_;
    my $environment = $this->getEnvironment();
    my $systemCall = $environment->getSystemCall();
    return $systemCall->executeAndWatchFiles(%params);
}

sub runDotNetProgram
{
    my ($this, %args) = @_;
    my $environment = $this->getEnvironment();
    $environment->runDotNetProgram(%args);
}

sub setEnvironment
{
    my ($this, $value) = @_;
    $this->{environment} = $value;
}

sub getEnvironment
{
    my ($this) = @_;
    return $this->{environment};
}

sub _addRunnerOptions
{
    my ($this, $args) = @_;
    my $runnerOptions = OptionsMerger::exclude($this->{runnerOptions}, $this->{suiteOptions});
    foreach (@{$runnerOptions})
    {
        my @names = @{ $_->{names} };
        my $name = $names[0];
        my @alreadyAdded = grep (/^--$name/, @$args);
        if (scalar(@alreadyAdded) > 0)
        {
            next;
        }
        my $optionValue = $this->getOptionValue($name);

        my $opt = _getOptionByName($runnerOptions, $name);
        my $defaultValue = $opt->getDefaultValue();
        my $hasDefaultValue = defined($defaultValue);
        if (not defined($optionValue) or ($hasDefaultValue and $optionValue eq $opt->getDefaultValue()))
        {
            next;
        }
        if ($_->{valueType} != Option->ValueTypeNone)
        {
            if ($opt->typeSpec() =~ m/\@$/)
            {
                foreach my $v (@{$optionValue})
                {
                    push(@{$args}, "--$name=$v");
                }
            }
            else
            {
                push(@{$args}, "--$name=$optionValue");
            }
        }
        else
        {
            push(@{$args}, "--$name");
        }
    }
}

sub _getOptionByName
{
    my ($options, $name) = @_;
    my $opts = new Options(options => $options);
    return $opts->_getOptionByName($name);
}

sub _getHelpText
{
    my ($this, %args) = @_;
    $this->_prepareOptions();
    $args{name} = $this->getName();
    $args{options} = $this->{allOptionsWithoutNamespace};
    return SuiteHelpPrinter::getText(%args);
}

sub _fromDescriptions
{
    my ($descriptions) = @_;
    if (not defined($descriptions))
    {
        return [];
    }
    my @result = Option::fromDescriptions(@{$descriptions});
    return \@result;
}

sub updateRunnerHelp
{
    my ($this) = @_;
    if (not $this->{optionsGenProj})
    {
        return (undef, 0);
    }
    if ($this->{helpUpdated})
    {
        return ($this->_getRunnerOptionsFileName(), 0);
    }
    my $projectPath = $this->{optionsGenProj};
    my $optionsFileName = $this->_getRunnerOptionsFileName();
    return $this->_doUpdateRunnerHelp($projectPath, $optionsFileName);
}

sub _doUpdateRunnerHelp
{
    my ($this, $projectPath, $optionsFileName) = @_;
    my ($fileName, $runnerDir) = fileparse($projectPath);
    Logger::minimal("Updating help for $fileName.");
    $this->createArtifactsDirIfNotExistsOrCroak('UnifiedTestRunner');
    my @projects = ($projectPath);
    if ($this->{testRunnerLibProj})
    {
        push(@projects, $this->{testRunnerLibProj});
    }
    my $msbuildProjFileName = $this->_generateMSBuildScript(@projects);
    $this->buildCSProject2($msbuildProjFileName);
    my @programArgs;
    if ($projectPath =~ m/UnifiedTestRunner.csproj$/)
    {
        push(@programArgs, '--suite=' . $this->getName());
    }
    push(@programArgs, "--saveoptions=$optionsFileName");
    my $exitCode = $this->runDotNetProgram(
        program => $this->_getAssemblyFullPath($projectPath),
        programArgs => \@programArgs
    );

    if ($exitCode == 0)
    {
        my $program = catfile(Dirs::UTR_root(), 'updateAutomationDocs.pl');
        $exitCode = $this->callSystem($program, "--suite=" . $this->getName());
    }

    $this->{helpUpdated} = 1;
    return ($optionsFileName, $exitCode);
}

sub _getRunnerOptionsFileName
{
    my ($this) = @_;
    my $utrRoot = Dirs::UTR_root();
    my $name = lc($this->getName());
    $name =~ s/\b(\w)/\U$1/g;
    my $runnerOptionsFileName = canonpath(catfile($utrRoot, 'Configs', "$name.Options.json"));
    return canonpath($runnerOptionsFileName);
}

1;
