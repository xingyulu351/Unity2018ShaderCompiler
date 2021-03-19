use Test::More;
use Coverage::Coverage;
use Carp qw (croak);
use File::Spec;
use Dirs;
use lib ('../../Tools/Build');
use Tools qw (UCopy RmtreeSleepy MkpathSleepy);

my $root = Dirs::getRoot();
my $tempPath = "$root/build/temp";
my $testDataDir = "$root/Tools/UnifiedTestRunner/Coverage/test_data";
my $testCoverageFile = "$testDataDir/test.cov";
my $bullsEyeBinValue = $ENV{'BULLSEYEBIN'};
my $covFileValue = $ENV{'COVFILE'};

undef $ENV{'BULLSEYEBIN'};

my $noCoverage = Coverage::Coverage::init();

isa_ok($noCoverage, Coverage::NoCoverage, 'NoCoverage instantiated');
is($noCoverage->EnableCoverage(), undef, 'NoCoverage::EnableCoverage invoked');
is($noCoverage->DisableCoverage(), undef, 'NoCoverage::DisableCoverage invoked');
is($noCoverage->CleanCoverageData(), undef, 'NoCoverage::CleanCoverageData invoked');
is($noCoverage->GenerateReports("suite", "path"), undef, 'NoCoverage::GenerateReports invoked');
is($noCoverage->GenerateDescriptionFile(), undef, 'NoCoverage::GenerateDescriptionFile invoked');

$ENV{'BULLSEYEBIN'} = $bullsEyeBinValue;
$ENV{'COVFILE'} = $testCoverageFile;

my $bullsEyeCoverage = Coverage::Coverage::init();
isa_ok($bullsEyeCoverage, Coverage::BullsEyeCoverage, 'BullsEyeCoverage instantiated');
is($bullsEyeCoverage->EnableCoverage(), 0, 'BullsEyeCoverage::EnableCoverage invoked');

UCopy("$testDataDir/tests.txt", "$tempPath/tests.txt");

is($bullsEyeCoverage->GenerateReports("supersuite", $tempPath, "$testDataDir/src"), 0, 'BullsEyeCoverage::GenerateReports invoked');

$ENV{'COVFILE'} = "/dev/null/test.cov";
is($bullsEyeCoverage->CleanCoverageData(), 0, 'Clean coverage data does not throw if file not exists');

$ENV{'COVFILE'} = $covFileValue;

is(-e "$tempPath/supersuite/cov.cov", 1, "Coverage file exists at target path");
is(-e "$tempPath/supersuite/cov.xml", 1, "Coverage xml exists at target path");
is(-e "$tempPath/supersuite/cov.html", 1, "Coverage html folder exists at target path");

is($bullsEyeCoverage->DisableCoverage(), 0, 'BullsEyeCoverage::DisableCoverage invoked');

#prepare folders structure for merge
#remove if something left from previous runs
if (-d "$tempPath/CoverageSummary")
{
    MkpathSleepy("$tempPath/CoverageSummary") or croak("Failed to cleanup $tempPath/CoverageSummary");
}

MkpathSleepy("$tempPath/CoverageSummary") or croak("Failed to create $tempPath/CoverageSummary");
UCopy("$testDataDir/CoverageSummary", "$tempPath/CoverageSummary");

$bullsEyeCoverage->Merge("$tempPath/CoverageSummary", "$testDataDir/src");
is(-d "$tempPath/CoverageSummary/overall", 1, "Overall folder exists");
is(-d "$tempPath/CoverageSummary/native", 1, "native folder exists");
is(-d "$tempPath/CoverageSummary/integration", 1, "integration folder exists");
is(-d "$tempPath/CoverageSummary/runtime", 1, "runtime folder exists");
is(-e "$tempPath/CoverageSummary/overall/summary.txt", 1, "overall summary file exists");
is(-e "$tempPath/CoverageSummary/native/summary.txt", 1, "native summary file exists");
is(-e "$tempPath/CoverageSummary/integration/summary.txt", 1, "integration summary file exists");
is(-e "$tempPath/CoverageSummary/runtime/summary.txt", 1, "runtime folder exists");
is(-e "$tempPath/CoverageSummary/native/tests.txt", 1, "native summary file exists");
is(-e "$tempPath/CoverageSummary/integration/tests.txt", 1, "integration summary file exists");
is(-e "$tempPath/CoverageSummary/runtime/tests.txt", 1, "runtime folder exists");

#description file validations
my $descriptionFile = "$tempPath/CoverageSummary/description.txt";

$bullsEyeCoverage->GenerateDescriptionFile("editor", "windows", "4.5.0a5", "4886caa49977", "82766",
    "111222333", "toolsmiths/coverage", "http://dummy.com/api/coverage",
    "$descriptionFile");

is(-e "$descriptionFile", 1, "Description file exists");

my %descr = ReadDescriptionFile("$descriptionFile");

ok($descr{"Platform"} eq "editor", 'Platform is correct');
ok($descr{"OsFamily"} eq "windows", 'OS is correct');
ok($descr{"Version"} eq "4.5.0a5", 'Version is correct');
ok($descr{"Changeset"} eq "4886caa49977", 'Changeset is correct');
ok($descr{"Revision"} eq "82766", 'Revision correct');
ok($descr{"Branch"} eq "toolsmiths/coverage", 'Branch is correct');
ok($descr{"Date"} eq "111222333", 'Date is ok');
ok($descr{"Uri"} eq "http://dummy.com/api/coverage", 'Download url is correct');

done_testing();

sub ReadDescriptionFile ()
{
    my ($descriptionFile) = @_;
    open $info, "$descriptionFile";
    my @lines = <$info>;
    my %descr = ();
    foreach my $line (@lines)
    {
        chomp($line);
        my ($key, $val) = split('=', $line);
        $descr{$key} = $val;
    }
    close($info);
    return %descr;
}

done_testing();
