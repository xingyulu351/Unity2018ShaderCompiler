package Hoarder::Collectors::TestSessionInfo;

sub new
{
    my $instance = { args => [] };
    return bless($instance, __PACKAGE__);
}

sub addSuite
{
    my ($this, $suite) = @_;
    push(@{ $this->{suites} }, $suite);
}

sub name
{
    return 'run-test-session-data',;
}

sub data
{
    my ($this) = @_;
    @{ $this->{args} } = @ARGV;
    return { %{$this} };
}

1;
