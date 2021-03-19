use File::Basename qw ( dirname );
use Getopt::Long;
use File::Spec;
use Cwd qw (realpath);
use lib File::Spec->rel2abs(dirname($0) . '/../../perl_lib');
use lib File::Spec->rel2abs(dirname($0) . '/../../../Configuration');

use BuildConfig;
use OTEE::BuildUtils qw (svnGetRevision);
use Coverage::Coverage;

my $root = realpath(File::Spec->rel2abs(dirname(__FILE__)) . '/../../..');
my $tempPath = "$root/build/temp";
my $storageUri;

GetOptions("platform:s" => \$platform, "storage_uri:s" => \$storageUri) or croak("Invalid number of arguments");

print("Building summary report platform: $platform, storage_uri: $storageUri\n");

my $os = $^O;
if ($os eq "MSWin32")
{
    $os = "windows";
}

if ($os eq "darwin")
{
    $os = "macos";
}

my $version = $BuildConfig::unityVersion;
my %svn_info = svnGetRevision();
my $ver_revision = $svn_info{Revision};
my $ver_numericRevision = $svn_info{NumericRevision};
my $ver_date = $svn_info{Date};
my $branch = `hg branch`;

chop($branch);

my $coverage = Coverage::Coverage::init();
my $summaryFolder = "$tempPath/CoverageSummary";
$coverage->Merge("$tempPath/CoverageSummary", "$root");
$coverage->GenerateDescriptionFile($platform, $os, $version, $ver_revision, $ver_numericRevision,
    $ver_date, $branch, $storageUri, "$summaryFolder/description.txt");
