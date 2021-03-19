package TestHelpers::Stub;

use Carp qw (croak);
use Dirs;
use lib Dirs::external_root();

use Test::MockObject;
use Test::More;
use TestHelpers::Expectation;
use TestHelpers::AlwaysMatchExpectation;

sub new
{
    my $package = shift();
    my $instance = {
        mockObject => Test::MockObject->new(),
        expectations => {},
        default_expectation => {}
    };
    return bless($instance, $package);
}

sub specifyExpectation
{
    my ($this, %args) = @_;
    my $method = $args{method};
    my $expected_params = $args{params};
    my $result = $args{result};
    my $default = $args{default};

    if (not defined($method))
    {
        croak("method is not defined");
    }

    if (defined($expected_params))
    {
        my $expectation = _createExpectation(%args);
        $this->_addExpectationForMethod($method, $expectation);
    }
    else
    {
        $this->_addDefaultExpectationForMethod($method, $result);
    }

    my $stub = $this;

    $this->{mockObject}->mock(
        $method,
        sub
        {
            my ($this, %args) = @_;
            my @expectations = $stub->_getExpectationsForMethod($method);
            foreach my $e (@expectations)
            {
                if ($e->match(%args))
                {
                    return $e->result();
                }
            }

            my $defaultExpectation = $stub->{default_expectations}{$method};
            if (defined($defaultExpectation))
            {
                return $defaultExpectation->result();
            }

            return undef;
        }
    );
}

sub object
{
    my ($this) = @_;
    return $this->{mockObject};
}

sub _createExpectation
{
    my (%params) = @_;
    return new TestHelpers::Expectation(%params);
}

sub _addDefaultExpectationForMethod
{
    my ($this, $method, $result) = @_;
    my $defaultExpectation = new TestHelpers::AlwaysMatchExpectation($result);
    $this->{default_expectations}{$method} = $defaultExpectation;
}

sub _addExpectationForMethod
{
    my ($this, $method, $expectation) = @_;

    if (not exists($this->{expectations}{$method}))
    {
        $this->{expectations}{$method} = [];
    }

    push(@{ $this->{expectations}{$method} }, $expectation);
}

sub _getExpectationsForMethod
{
    my ($this, $method, $expectation) = @_;
    if (not exists($this->{expectations}{$method}))
    {
        $this->{expectations}{$method} = [];
    }

    return @{ $this->{expectations}{$method} };
}

1;
