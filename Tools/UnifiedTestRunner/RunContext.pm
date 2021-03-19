package RunContext;

use warnings;
use strict;

use File::Temp qw (tempdir);
use File::Spec::Functions qw (catfile);

sub new
{
    my $options = shift;

    my $instance = {
        options => undef,
        progress => undef,
        artifacts_dir => undef,
        artifactsCounter => 0,
        iteration => undef,
        smartSelectSessionId => undef,
    };
    $instance->{options} = $options;
    return bless($instance, __PACKAGE__);
}

sub getFilter
{
    my ($this) = @_;
    return $this->getOptionValue(undef, 'filter');
}

sub getCoverage
{
    my ($this) = @_;
    return $this->getOptionValue(undef, 'coverage');
}

sub setIteration
{
    my ($this, $value) = @_;
    $this->{iteration} = $value;
}

sub getIteration
{
    my ($this) = @_;
    return $this->{iteration};
}

sub getArtifactsPath
{
    my ($this) = @_;
    if ($this->{artifacts_dir})
    {
        return $this->adjustArtifactsPathForIteration($this->{artifacts_dir});
    }

    $this->{artifacts_dir} = $this->getOptionValue(undef, 'artifacts_path');
    if (not defined($this->{artifacts_dir}))
    {
        $this->{artifacts_dir} = tempdir(UNLINK => 1);
    }

    return $this->adjustArtifactsPathForIteration($this->{artifacts_dir});
}

sub adjustArtifactsPathForIteration
{
    my ($this, $artifacts_path) = @_;
    if (not defined($this->{iteration}))
    {
        return $artifacts_path;
    }

    return catfile($this->{artifacts_dir}, $this->{iteration});
}

sub getOptionValue
{
    my ($this, $default_value, $option, $namespace) = @_;
    if (defined($namespace))
    {
        $option = "$namespace-$option";
    }
    my $result = $this->{options}->getOptionValue($option);
    if (!defined($result))
    {
        $result = $default_value;
    }

    return $result;
}

sub resolve
{
    my ($this, $name, $namespace) = @_;
    return $this->{options}->resolve($name, $namespace);
}

sub getSmartSelectSessionId
{
    my ($this) = @_;
    return $this->{smartSelectSessionId};
}

sub setSmartSelectSessionId
{
    my ($this, $value) = @_;
    $this->{smartSelectSessionId} = $value;
}

sub reportProgress
{
    my ($this, $suite, $executed, $total) = @_;
    if (not defined($this->getOptionValue(undef, 'progress')))
    {
        return;
    }
    if (not defined($this->{progress}))
    {
        return;
    }
    $this->{progress}->progress($suite, $executed, $total);
}

1;
