package LinuxBugReporter;

use Carp;
use Tools qw (JamRaw AmRunningOnBuildServer ProgressMessage);
use File::Basename qw (dirname);

use lib File::Spec->rel2abs(dirname(__FILE__) . "/../../SDKDownloader");
use SDKDownloader;

sub Register
{
    Main::RegisterTarget(
        "BugReporter",
        {
            linuxbuildfunction => \&BuildBugReporter,
            options => {
                codegen => ["debug", "release"],
                platform => undef,
                enableBugReporterTests => [0, 1]
            }
        }
    );
}

sub CheckInstallSDK
{
    my ($root) = @_;
    if (AmRunningOnBuildServer() || exists $ENV{'UNITY_USE_LINUX_SDK'})
    {
        ProgressMessage('Setting up the Linux SDK');
        SDKDownloader::PrepareSDK('linux-sdk', '20180406', "$root/artifacts");
    }

}

sub BuildBugReporter
{
    my ($root, $targetPath, $options) = @_;
    my $platform = GetPlatform($options);
    my $config = GetConfig($options);
    CheckInstallSDK($root);
    ProgressMessage("Building bug reporter");

    JamRaw($root, "-sPLATFORM=$platform", "-sCONFIG=$config", "bugreporter");
    JamRaw($root, "-sPLATFORM=$platform", "-sCONFIG=$config", "bugreporter_test") if $options->{"enableBugReporterTests"} eq "1";

}

sub GetPlatform
{
    my ($options) = @_;
    my $platform = $options->{platform};
    if (!defined($platform) or !$platform)
    {
        $platform = "linux64";
    }
    return lc $platform;
}

sub GetConfig
{
    my ($options) = @_;
    my $config = $options->{codegen};
    if (!defined($config) or !$config)
    {
        $config = "release";
    }
    return lc $config;
}

1;
