package Options;

use Getopt::Long qw (GetOptionsFromArray :config pass_through);

use warnings;
use strict;
use Dirs;

sub new
{
    my ($pkg, %args) = @_;
    my $this = {
        options => [],
        lookupTable => {},
        unknownOptions => []
    };

    $this = bless($this, $pkg);
    foreach my $o (@{ $args{options} })
    {
        $this->registerOption($o);
    }
    return $this;
}

sub registerOption
{
    my ($this, $option) = @_;
    my @names = $option->getNames();
    foreach my $n (@names)
    {
        my $exitingOption = $this->{lookupTable}->{$n};
        if (defined($exitingOption))
        {
            my %lookupTable = %{ $this->{lookupTable} };
            $lookupTable{$n}->[0] = $option;
            my $idx = $lookupTable{$n}->[1];
            $this->{options}->[$idx] = $option;
            return;
        }
    }

    push(@{ $this->{options} }, $option);
    my $idx = scalar(@{ $this->{options} }) - 1;
    foreach my $n (@names)
    {
        $this->{lookupTable}->{$n} = [$option, $idx];
    }
}

sub getOptions
{
    my ($this) = @_;
    return @{ $this->{options} };
}

sub parse
{
    my ($this, @args) = @_;
    $this->_reset();
    my @specs = map { _makeOptionSpec($_) } $this->getOptions();
    my $res = GetOptionsFromArray(\@args, @specs);
    $this->{unknownOptions} = \@args;
    return $res;
}

sub getOptionValue
{
    my ($this, $name, $namespace) = @_;
    my $opt = $this->_getOptionByName($name, $namespace);
    if (not defined($opt))
    {
        return undef;
    }
    return $opt->getValue();
}

sub resolve
{
    my ($this, $name, $namespace) = @_;
    my $globalOption = $this->_getOptionByName($name);
    if (not defined($namespace))
    {
        return _safeGetOptionValue($globalOption);
    }

    my $optionInNamespace = $this->_getOptionByName($name, $namespace);
    if (not defined($optionInNamespace))
    {
        return _safeGetOptionValue($globalOption);
    }

    if (not $optionInNamespace->getValueChanged())
    {
        if (not defined($globalOption))
        {
            return $optionInNamespace->getValue();
        }
        if (not $globalOption->getValueChanged())
        {
            return $optionInNamespace->getValue();
        }
        return _safeGetOptionValue($globalOption);
    }

    return $optionInNamespace->getValue();
}

sub getUnknownOptions
{
    my ($this) = @_;
    return @{ $this->{unknownOptions} };
}

sub getOptionByName
{
    my ($name, $options) = @_;
    my $isArray = ref($options) eq 'ARRAY';
    if ($isArray)
    {
        my $opts = new Options(options => $options);
        return $opts->_getOptionByName($name);
    }
    return $options->_getOptionByName($name);
}

sub _safeGetOptionValue
{
    my ($option) = @_;
    if ($option)
    {
        return $option->getValue();
    }
    return;
}

sub _reset
{
    my ($this) = @_;
    foreach my $o ($this->getOptions())
    {
        $o->reset();
    }
}

sub _getOptionByName
{
    my ($this, $name, $namespace) = @_;
    my $index = $this->_getOptionIndex($name, $namespace);
    if ($index != -1)
    {
        return $this->{options}[$index];
    }
    return undef;
}

sub _getOptionIndex
{
    my ($this, $name, $namespace) = @_;
    if (defined($namespace))
    {
        $name = "$namespace-$name";
    }
    my %lookupTable = %{ $this->{lookupTable} };
    my $idx = $lookupTable{$name}->[1];
    if (defined($idx))
    {
        return $idx;
    }
    return -1;
}

sub _makeOptionSpec
{
    my ($option) = @_;
    my $spec = $_->toSpec();
    return $spec => sub
    {
        my ($name, $value) = @_;
        my $isArray = $option->toSpec() =~ '@';
        if ($isArray)
        {
            _processArrayValue($option, $name, $value);
        }
        else
        {
            $option->setValue($value);
        }
        }
}

sub _processArrayValue
{
    my ($option, $name, $value) = @_;
    my $arr = $option->getValue();
    my $defaultValue = $option->getDefaultValue();
    my $bothDefined = (defined($arr) and defined($defaultValue));
    my $shouldResetValue = (not defined($arr) or ($bothDefined && $arr eq $defaultValue));
    if ($shouldResetValue)
    {
        $option->setValue([]);
    }
    $arr = $option->getValue($name);
    push(@{$arr}, $value);
}

1;
