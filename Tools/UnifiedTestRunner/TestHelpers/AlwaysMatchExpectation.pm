package TestHelpers::AlwaysMatchExpectation;

sub new
{
    my ($pkg, $result) = @_;
    my $instance = { result => $result };
    return bless($instance, $pkg);
}

sub match
{
    return 1;
}

sub result
{
    my ($this) = @_;
    return $this->{result};
}

1;
