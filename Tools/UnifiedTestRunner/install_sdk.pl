use warnings;
use strict;

use FindBin qw ($Bin);
use lib "$Bin";

use Dirs;
use lib Dirs::external_root();
use JSON;

use File::Spec::Functions;
use File::Basename qw (dirname);
use Getopt::Long;

use lib catdir(Dirs::getRoot(), 'Tools', 'Build');
use Tools qw (AmRunningOnBuildServer ProgressMessage);
use lib catdir(Dirs::getRoot(), 'Tools', 'SDKDownloader');
use SDKDownloader;
use UnityTestProtocol::Messages;

my ($repoName, $sdkOverride, $artifactsFolder) = _readOptions(@ARGV);
ProgressMessage("Setting up the $repoName");
SDKDownloader::PrepareSDK($repoName, $sdkOverride, $artifactsFolder);
my $msg = UnityTestProtocol::Messages::makeEnvironmentVariablesMessage(\%ENV);
print("\n##utp:" . objToJson($msg) . "\n");

exit(0);

sub _readOptions
{
    my (@ARGV) = @_;
    my ($repoName, $sdkOverride, $artifacts_folder);
    GetOptions(
        "repo_name=s" => \$repoName,
        "sdk_override=s" => \$sdkOverride,
        "artifacts_folder=s" => \$artifacts_folder,
    ) or croak("could not parse commandline");

    return ($repoName, $sdkOverride, $artifacts_folder);
}
