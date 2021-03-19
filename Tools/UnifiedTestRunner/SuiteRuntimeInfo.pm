package SuiteRuntimeInfo;

sub new
{
    my ($pkg, %params) = @_;
    my $instance = {
        name => $params{name},
        platform => $params{platform}
    };
    return bless($instance, $pkg);
}

sub getName
{
    my ($this) = @_;
    return $this->{name};
}

sub getPlatform
{
    my ($this) = @_;
    return $this->{platform};
}

1;
