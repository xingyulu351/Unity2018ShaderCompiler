use Test::More;

use warnings;
use strict;

use Dirs;
use lib Dirs::external_root();

use Sys::Hostname::Long;

use Test::MockObject;
use Test::MockModule;
use Sys::Hostname::Long;

BEGIN { use_ok('Hoarder::Collectors::AgentInfo') }

BASIC:
{
    can_ok('Hoarder::Collectors::AgentInfo', 'new');
    my $ai = Hoarder::Collectors::AgentInfo->new();
    isa_ok($ai, 'Hoarder::Collectors::AgentInfo');
}

RECEIVE_DATA:
{
    my $ai = Hoarder::Collectors::AgentInfo->new();
    is($ai->name(), 'agent-info');

    my %data = %{ $ai->data() };

    my $expectedHostName = hostname_long;
    is($data{host}, $expectedHostName);
    is($data{os}, $^O);
    is(scalar(keys(%data)), 3);

IS_BUILD_AGENT:
    {
        my $initialValue = $ENV{'UNITY_THISISABUILDMACHINE'};
        $ENV{'UNITY_THISISABUILDMACHINE'} = 0;
        $ai = Hoarder::Collectors::AgentInfo->new();
        %data = %{ $ai->data() };
        is($data{build_agent}, 0);
        $ENV{'UNITY_THISISABUILDMACHINE'} = 1;
        $ai = Hoarder::Collectors::AgentInfo->new();
        %data = %{ $ai->data() };
        is($data{build_agent}, 1);
        $ENV{'UNITY_THISISABUILDMACHINE'} = $initialValue;
    }
}

Host_Name:
{
    if ($^O eq "darwin")
    {
        my $honameModule = new Test::MockModule('Sys::Hostname::Long');
        $honameModule->mock('hostname_long' => sub { return 'hostname'; });
        my %data = data();
        is($data{host}, 'hostname');

        $honameModule->mock('hostname_long' => sub { return ''; });
        %data = data();
        chomp(my $hostName = `hostname -f`);
        is($data{host}, $hostName);
    }
}

done_testing();

sub data
{
    my $ai = new Hoarder::Collectors::AgentInfo();
    my %data = %{ $ai->data() };
    return %data;
}
