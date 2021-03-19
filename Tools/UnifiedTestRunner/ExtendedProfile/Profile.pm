package ExtendedProfile::Profile;

use warnings;
use strict;

use ExtendedProfile::Suite;

sub new
{
    my ($pkg, $obj) = @_;

    my $instance = { suites => _createSuites($obj) };
    return bless($instance, $pkg);
}

sub getSuites
{
    my ($this) = @_;
    return @{ $this->{suites} };
}

sub _createSuites
{
    my ($json) = @_;
    my @result;
    foreach my $s (@{ $json->{suites} })
    {
        my $suite = _createSuite($s);
        push(@result, $suite);
    }
    return \@result;
}

sub _createSuite
{
    my ($json) = @_;
    my $name = $json->{name};
    my $result = new ExtendedProfile::Suite(
        name => $name,
        args => $json->{args},
        commandLineHasPriority => $json->{commandLineHasPriority},
        details => $json->{details}
    );
    return $result;
}

1;
