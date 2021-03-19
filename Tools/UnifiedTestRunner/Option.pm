package Option;

use warnings;
use strict;
use Dirs;
use lib Dirs::external_root();
use Carp qw (croak);
use Storable qw (dclone);

use constant {
    ValueTypeNone => 0,
    ValueTypeOptional => 1,
    ValueTypeRequired => 2,
};

sub new
{
    my ($pkg, %args) = @_;
    if (not defined($args{names}))
    {
        croak('option names must be specified');
    }

    if (not defined($args{valueType}))
    {
        croak('option value type must be specified');
    }

    my $instance = {
        names => $args{names},
        valueType => $args{valueType},
        description => $args{description},
        typeSpec => $args{typeSpec},
        allowedValues => $args{allowedValues},
        hidden => $args{hidden},
        defaultValue => $args{defaultValue},
        namespace => $args{namespace},
        value => $args{defaultValue},
        isPrototype => $args{isPrototype},
        valueChanged => 0,
    };

    $instance = bless($instance, $pkg);
    if (not $instance->{allowedValues})
    {
        $instance->{allowedValues} = [];
    }

    if (not $instance->{hidden})
    {
        $instance->{hidden} = 0;
    }

    if ($args{namespace})
    {
        $instance->moveToNamespace($args{namespace});
    }
    return $instance;
}

sub getNames
{
    my ($this) = @_;
    my @names = @{ $this->{names} };
    return @names;
}

sub getName
{
    my ($this) = @_;
    my @names = @{ $this->{names} };
    return $names[0];
}

sub getValue
{
    my ($this) = @_;
    return $this->{value};
}

sub setValue
{
    my ($this, $value) = @_;
    $this->{value} = $value;
    $this->{valueChanged} = 1;
}

sub getValueType
{
    my ($this) = @_;
    return $this->{valueType};
}

sub getDescription
{
    my ($this) = @_;
    return $this->{description};
}

sub getAllowedValues
{
    my ($this) = @_;
    return @{ $this->{allowedValues} };
}

sub isHidden
{
    my ($this) = @_;
    return $this->{hidden};
}

sub getDefaultValue
{
    my ($this) = @_;
    return $this->{defaultValue};
}

sub toSpec
{
    my ($this) = @_;
    my @names = $this->getNames();
    my $result = join('|', @names);
    if ($this->getValueType() == ValueTypeOptional)
    {
        $result .= ':' . $this->typeSpec();
    }

    if ($this->getValueType() == ValueTypeRequired)
    {
        $result .= '=' . $this->typeSpec();
    }

    return $result;
}

sub typeSpec
{
    my ($this) = @_;
    return $this->{typeSpec};
}

sub hasName
{
    my ($this, $name) = @_;
    if (grep(/^$name$/, $this->getNames()))
    {
        return 1;
    }

    return 0;
}

sub getValueChanged
{
    my ($this) = @_;
    return $this->{valueChanged};
}

sub fromDescription
{
    my @spec = @{ $_[0] };
    _validate(@spec);
    my ($names, $valueType, $typeSpec) = _parseNameSpec(@spec);
    my $description = _getDescription(@spec);
    my $details = _getDetails(@spec);
    return new Option(
        names => $names,
        valueType => $valueType,
        typeSpec => $typeSpec,
        description => $description,
        allowedValues => $details->{allowedValues},
        hidden => $details->{hidden},
        defaultValue => $details->{defaultValue},
        isPrototype => $details->{isPrototype}
    );
}

sub fromDescriptions
{
    my @descriptions = @_;
    my @result;
    foreach (@descriptions)
    {
        if (ref($_) eq 'HASH')
        {
            my @prototypes = @{ $_->{prototypes} };
            foreach my $prot (@prototypes)
            {
                my $descr = [$prot, { isPrototype => 1 }];
                push(@result, fromDescription($descr));
            }
        }
        else
        {
            push(@result, fromDescription($_));
        }

    }
    return @result;
}

sub moveToNamespace
{
    my ($this, $namespace) = @_;
    my @names = $this->getNames();
    @names = map { $_ = $namespace . '-' . $_; } @names;
    $this->{names} = \@names;
    $this->{namespace} = $namespace;
}

sub clone
{
    my ($this) = @_;
    my $cloneref = dclone($this);
    return bless($cloneref, __PACKAGE__);
}

sub reset
{
    my ($this) = @_;
    $this->setValue($this->{defaultValue});
    $this->{valueChanged} = 0;
}

sub _parseNameSpec
{
    my (@spec) = @_;
    my $nameSpec = $spec[0];
    my $valueType = ValueTypeNone;
    my $typeSpec = undef;
    my $valueTypeSpec = ($nameSpec =~ m/([:=].+$)/);
    my $matchIdx = $-[0];
    if ($valueTypeSpec)
    {
        $valueType = ValueTypeOptional;
        if (substr($nameSpec, $matchIdx, 1) eq '=')
        {
            $valueType = ValueTypeRequired;
        }
        $typeSpec = substr($nameSpec, $matchIdx + 1);

        $nameSpec = substr($nameSpec, 0, $matchIdx);
    }

    my @names = split(m/\|/, $nameSpec);
    return (\@names, $valueType, $typeSpec);
}

sub _validate
{
    my (@spec) = @_;

    if ($spec[0] =~ m/[:=]$/)
    {
        croak("Invalid option specification");
    }

    my @c = $spec[0] =~ /[:=]/g;
    if (scalar(@c) > 1)
    {
        croak("Option specification must contain only one value type specifier");
    }
}

sub _getDescription
{
    my (@spec) = @_;
    if (scalar(@spec) > 1)
    {
        if (ref($spec[1]) eq 'HASH')
        {
            return undef;
        }
        return $spec[1];
    }
    return;
}

sub _getDetails
{
    my (@spec) = @_;
    if (scalar(@spec) > 1)
    {
        if (ref($spec[1]) eq 'HASH')
        {
            return $spec[1];
        }
    }
    if (scalar(@spec) > 2)
    {
        return $spec[2];
    }
    return {};
}

1;
