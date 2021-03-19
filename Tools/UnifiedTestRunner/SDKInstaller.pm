package SDKInstaller;

use warnings;
use strict;

use File::Spec::Functions qw (catfile catdir);
use Dirs;
use Carp qw (croak);
use UnityTestProtocol::Utils;

sub installSDK
{
    my ($repoName, $sdkOverride, $artifactsFolder) = @_;

    my $systemCall = new SystemCall();
    my $sdkInstaller = catfile(Dirs::UTR_root(), 'install_sdk.pl');

    my $root = Dirs::getRoot();

    my ($exitCode, @lines) =
        $systemCall->executeAndGetStdOut('perl', $sdkInstaller, "--repo_name=$repoName", "--sdk_override=$sdkOverride", "--artifacts_folder=$artifactsFolder");

    if ($exitCode != 0)
    {
        croak("Can not install $repoName");
    }

    _applyScriptsEnvironemntVariables(@lines);
}

sub _applyScriptsEnvironemntVariables
{
    my (@lines) = @_;

    foreach my $line (@lines)
    {
        my $msg = UnityTestProtocol::Utils::getMessage($line);
        if (not $msg)
        {
            next;
        }

        if (not $msg->{type} eq 'EnvVars')
        {
            next;
        }

        foreach my $key (keys(%{ $msg->{vars} }))
        {
            if (not exists($ENV{$key}))
            {
                Logger::minimal("Copy environment variable '$key=$msg->{vars}->{$key}' to the UTR process, as it was set by SDK installer.");
                $ENV{$key} = $msg->{vars}->{$key};
            }
        }
    }
}

1;
