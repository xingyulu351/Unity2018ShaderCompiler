use Coverage::Coverage;
use Getopt::Long;
use File::Basename qw ( dirname );
use Carp qw (croak);
use Cwd qw (abs_path);
my $suite;
my $outputdir;
my $filedir = abs_path(dirname(__FILE__));
my $root = abs_path($filedir . "/../../..");

my $coverage = Coverage::Coverage::init();

GetOptions("suite:s" => \$suite, "outputdir:s" => \$outputdir) or croak("Invalid number of arguments");

$coverage->GenerateReports($suite, $outputdir, $root);
