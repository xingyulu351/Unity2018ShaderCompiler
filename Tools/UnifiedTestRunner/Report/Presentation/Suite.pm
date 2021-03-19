package Report::Presentation::Suite;

use warnings;
use strict;

use Report::Presentation::SuiteMinimalCommandLineBuilder qw (extract);

sub new
{
    my ($pkg, %params) = @_;
    my $instance = {
        name => $params{name},
        description => $params{description},
        platform => $params{platform},
        artifacts => [],
        tests => [],
        summary => $params{summary},
        minimalCommandLine => [],
        error => $params{error},
        data => $params{data},
        forciblyFailed => $params{forciblyFailed},
        failureReasons => $params{failureReasons},
        testresultsxml => $params{testresultsxml},
        details => $params{details},
        description => $params{description},
        partitionIndex => $params{partitionIndex},
    };

    if ($params{artifacts})
    {
        $instance->{artifacts} = $params{artifacts};
    }
    if ($params{tests})
    {
        $instance->{tests} = $params{tests};
    }

    if ($params{details})
    {
        $instance->{details} = $params{details};
    }

    if ($params{commandLine})
    {
        my %result = extract($params{commandLine}, $params{overridedOptions});
        $instance->{minimalCommandLine} = $result{ $params{name} };
    }

    return bless($instance, $pkg);
}

sub data
{
    my ($this) = @_;
    return { %{$this} };
}

1;
