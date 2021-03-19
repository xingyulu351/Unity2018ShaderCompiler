package ExtendedProfile::Suite;

sub new
{
    my ($pkg, %args) = @_;
    my $instance = {
        name => $args{name},
        args => $args{args},
        commandLineHasPriority => $args{commandLineHasPriority},
        combinedArgs => $args{combinedArgs},
        details => $args{details}
    };
    return bless($instance, __PACKAGE__);
}

sub getName
{
    my ($this) = @_;
    return $this->{name};
}

sub getArgs
{
    my ($this) = @_;
    return @{ $this->{args} };
}

sub getCombinedArgs
{
    my ($this) = @_;
    return @{ $this->{combinedArgs} };
}

sub setCombinedArgs
{
    my ($this, @args) = @_;
    $this->{combinedArgs} = \@args;
}

sub getCommandLineHasPriority
{
    my ($this) = @_;
    return $this->{commandLineHasPriority};
}

sub getDetails
{
    my ($this) = @_;
    return $this->{details};
}

1;
