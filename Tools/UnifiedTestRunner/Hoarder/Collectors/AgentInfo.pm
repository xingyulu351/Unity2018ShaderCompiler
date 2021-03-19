package Hoarder::Collectors::AgentInfo;

use Dirs;
use lib Dirs::external_root();
use StringUtils qw (isNullOrEmpty);

use Sys::Hostname::Long;

sub new
{
    my $object = {
        host => _getHostName(),
        os => $^O,
        build_agent => _isBuildAgent()
    };

    return bless($object, __PACKAGE__);
}

sub name
{
    return 'agent-info';
}

sub data
{
    return { %{ shift() } };
}

sub _isBuildAgent
{
    my $thisIsABuildMachine = $ENV{'UNITY_THISISABUILDMACHINE'};
    if (defined($thisIsABuildMachine) and $thisIsABuildMachine eq 1)
    {
        return 1;
    }
    return 0;
}

sub _getHostName
{
    my $hostName = Sys::Hostname::Long::hostname_long();
    if (not isNullOrEmpty($hostName))
    {
        return $hostName;
    }

    if ($^O eq "darwin")
    {
        $hostName = `hostname -f`;
        chomp($hostName);
    }

    if (isNullOrEmpty($hostName) and _isBuildAgent())
    {
        $hostName = $ENV{'hostname'};
    }

    if (isNullOrEmpty($hostName))
    {
        $hostName = '<unknown>';
    }

    return $hostName;
}

1;
