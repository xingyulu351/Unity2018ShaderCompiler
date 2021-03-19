use File::Basename qw ( dirname );
use Getopt::Long;
use File::Spec;
use Cwd qw (realpath);
use lib File::Spec->rel2abs(dirname($0) . '/../../External/Perl/lib');
use lib File::Spec->rel2abs(dirname($0) . '/../../Configuration');
use BuildConfig;
use OTEE::BuildUtils qw (svnGetRevision);

sub main
{
    my ($tool, $configuration, $host, $storageUri, $fileName) = readOptions();
    my %svn_info = svnGetRevision();

    my $branch = `hg branch`;
    chop($branch);

    my %description = (
        "Tool" => $tool,
        "Configuration" => $configuration,
        "Host" => $host,
        "Version" => $BuildConfig::unityVersion,
        "Changeset" => $svn_info{Revision},
        "Revision" => $svn_info{NumericRevision},
        "Date" => $svn_info{Date},
        "Branch" => $branch
    );
    writeKeyValuesIntoTheFile($fileName, %description);
}

sub writeKeyValuesIntoTheFile
{
    my ($filename, %data) = @_;
    open(DFILE, ">$filename");
    while (my ($k, $v) = each %data)
    {
        print(DFILE "$k=$v\n");
    }
    close(DFILE);
}

sub readOptions
{
    my $tool;
    my $configuration;
    my $host;
    my $storageUri;
    my $fileName;

    my $resGetOpt = GetOptions(
        "tool:s" => \$tool,
        "configuration:s" => \$configuration,
        "host:s" => \$host,
        "result-file:s" => \$fileName
    );

    if (not $resGetOpt)
    {
        croak("$!");
    }

    return ($tool, $configuration, $host, $storageUri, $fileName);
}

main();
