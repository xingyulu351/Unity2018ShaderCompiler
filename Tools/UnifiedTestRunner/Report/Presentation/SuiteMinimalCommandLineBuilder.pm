package Report::Presentation::SuiteMinimalCommandLineBuilder;

use warnings;
use strict;

use GlobalOptions;
use SuiteRunners;
use SuiteTools;
use Options;
use Dirs;
use lib Dirs::external_root();
use List::MoreUtils qw (any);
use Exporter 'import';
use SingleSuiteRunnerArtifactsNameBuilder;
use RunContext;
use AppHelper;

our @EXPORT_OK = qw [extract];

my @optionsToOmit = (
    'tag', 'testresultsxml', 'config', 'config-id', 'build-number', 'artifacts_path',
    'submitter', 'profile', 'hoarder-uri', 'smart-select', 'owner', 'parallel-tests-selector-uri',
);

sub extract
{
    my ($commandLine, $overridedOptions) = @_;
    my %result;
    if (scalar(@{$commandLine}) == 0)
    {
        return %result;
    }
    my $result = AppHelper::parseOptions(@{$commandLine});
    my $options = $result->{options};
    my @suites = _getSuites($options);
    foreach my $suite (@suites)
    {
        my $suiteArgs = _getSuiteArgs($suite, $options, $overridedOptions);
        $result{$suite} = $suiteArgs;
    }
    return %result;
}

sub _getSuiteArgs
{
    my ($suite, $options, $overridedOptions) = @_;

    my @optionDefs = _getOptionDefs($options);
    my @result;

    my %usedOverridedOptions;
    foreach my $opt ($options->getOptions())
    {
        my @names = $opt->getNames();
        my $name = $names[0];
        if ($name eq 'suite')
        {
            next;
        }

        my $value = $opt->getValue();
        if (not defined($value))
        {
            next;
        }

        my $omited = any { $opt->hasName($_) } @optionsToOmit;
        if ($omited)
        {
            next;
        }

        my $defaultValue = $opt->getDefaultValue();
        if (defined($defaultValue) and $value eq $defaultValue)
        {
            next;
        }

        if ($opt->getValueType() == Option->ValueTypeNone)
        {
            push(@result, "--$name");
        }
        else
        {
            my $value = $opt->getValue();
            my $override = _getOverridedValue($opt, $overridedOptions);

            if (defined($override))
            {
                $usedOverridedOptions{$name} = 1;
                $value = $override->{value};
            }

            my $isArray = ref($value) eq 'ARRAY';
            if ($isArray)
            {
                foreach my $item (@$value)
                {
                    push(@result, "--$name=$item");
                }
            }
            else
            {
                push(@result, "--$name=$value");
            }
        }
    }

    foreach my $override (@{$overridedOptions})
    {
        my $optionName = $override->{name};
        if ($usedOverridedOptions{$optionName})
        {
            next;
        }
        my $value = $override->{value};
        push(@result, "--$optionName=$value");
    }

    return \@result;
}

sub _getOverridedValue
{
    my ($option, $overridedOptions) = @_;
    if (not defined($overridedOptions))
    {
        return undef;
    }
    foreach my $override (@{$overridedOptions})
    {
        if ($option->hasName($override->{name}))
        {
            return $override;
        }
    }
    return undef;
}

sub _getSuites
{
    my ($options) = @_;
    my $suites = $options->getOptionValue('suite');
    return @$suites;
}

sub _getOptionDefs
{
    my ($options) = @_;
    return map { $_->toSpec() } $options->getOptions();
}

1;
