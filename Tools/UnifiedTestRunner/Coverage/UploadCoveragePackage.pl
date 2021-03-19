use Coverage::Coverage;
use Getopt::Long;
use File::Basename qw ( dirname );
use Cwd qw (realpath);
use lib File::Spec->rel2abs(dirname($0) . '/../../perl_lib');
use Coverage::Coverage;
my $descriptionFilePath;
my $targetUri;

GetOptions("description_file_path:s" => \$descriptionFilePath, "uri:s" => \$targetUri) or croak("Invalid number of arguments");

open $info, "$descriptionFilePath";
my @lines = <$info>;
my %descr = ();
foreach my $line (@lines)
{
    chomp($line);
    my ($key, $val) = split('=', $line);
    print("$key = $val\n");
    $descr{$key} = $val;
}

my $coverage = Coverage::Coverage::init();
$coverage->UploadPackage(
    $descr{"Platform"}, $descr{"OsFamily"}, $descr{"Version"}, $descr{"Changeset"}, $descr{"Revision"},
    $descr{"Date"}, $descr{"Branch"}, $targetUri, $descr{"Uri"}
);
