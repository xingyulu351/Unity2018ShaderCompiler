use warnings;
use strict;

use UnityTestProtocol::Messages;
use Test::More;
use Dirs;
use lib Dirs::external_root();
use File::Spec::Functions qw (catdir catfile canonpath);
use Test::MockObject;
use Test::MockModule;
use Test::Trap;

BEGIN { use_ok('SDKInstaller'); }

installSDK_InvokesInstallerScriptWithCorrectArguments:
{
    my $res = _install(
        {
            installerScriptArgs => ['repoName', 'sdkOverride', 'artifactsFolder'],
            exitCode => 0
        }
    );
    my $sdkInstaller = catfile(Dirs::UTR_root(), 'install_sdk.pl');
    is_deeply($res->{capturedInstallerScriptArgs},
        ['perl', $sdkInstaller, '--repo_name=repoName', '--sdk_override=sdkOverride', '--artifacts_folder=artifactsFolder']);
}

install_CoraksIfInstallerExitedWithNonZero:
{
    my @trap = trap
    {
        my $res = _install(
            {
                installerScriptArgs => [],
                exitCode => 1
            }
        );
        SDKInstaller::installSDK('repoName', 'sdkOverride', 'artifactsFolder');
    };
    like($trap->die, qr/Can not install/, 'croaks when sdk installer exits with non zero code');
}

install_SetsEnvariablesIfReportedByInstalledScript:
{
    my $envVargsMsg = UnityTestProtocol::Messages::makeEnvironmentVariablesMessage({ var1 => 'var1' });
    my $res = _install(
        {
            installerScriptArgs => ['repoName', 'sdkOverride', 'artifactsFolder'],
            stdOut => ['##utp:{"time":0,"vars":{"var1-set-by-sdk-installer":"value"},"type":"EnvVars"}'],
            exitCode => 0
        }
    );

    is($ENV{'var1-set-by-sdk-installer'}, 'value');
}

done_testing();

sub _install
{
    my ($args) = @_;

    my $systemCallMock = new Test::MockObject();
    my @capturedInstallerScriptArgs;
    $systemCallMock->mock(
        executeAndGetStdOut => sub
        {
            my $this = shift();
            @capturedInstallerScriptArgs = @_;
            my @stdOutLines;
            if ($args->{stdOut})
            {
                @stdOutLines = @{ $args->{stdOut} };
            }
            return ($args->{exitCode}, @stdOutLines);
        }
    );

    my $systemCallModuleMock = new Test::MockModule('SystemCall');
    $systemCallModuleMock->mock(
        new => sub
        {
            my ($pkg) = @_;
            return $systemCallMock;
        }
    );

    SDKInstaller::installSDK(@{ $args->{installerScriptArgs} });

    return { capturedInstallerScriptArgs => \@capturedInstallerScriptArgs };
}
