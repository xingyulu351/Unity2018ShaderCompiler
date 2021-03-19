package Coverage::BullsEyeCoverage;

use File::Spec;
use File::Basename;
use File::Copy;
use LWP::UserAgent;
use HTTP::Request::Common;
use Cwd qw (realpath);
use Carp qw (croak);

my $root = realpath(File::Spec->rel2abs(dirname(__FILE__)) . '/../../..');
use lib File::Spec->rel2abs(dirname($0) . '/../../Build');
use lib File::Spec->rel2abs(dirname($0) . '/../Build');

use Tools qw (UCopy RmtreeSleepy MkpathSleepy);

my $cov01 = "$ENV{'BULLSEYEBIN'}cov01";
my $covxml = "$ENV{'BULLSEYEBIN'}covxml";
my $covhtml = "$ENV{'BULLSEYEBIN'}covhtml";
my $covdir = "$ENV{'BULLSEYEBIN'}covdir";

sub new
{
    my $instance = {};
    return bless($instance, __PACKAGE__);
}

sub EnableCoverage
{
    SystemWrapper("$cov01", "--on", "--quiet", "--no-banner");
}

sub DisableCoverage
{
    SystemWrapper("$cov01", "--off", "--quiet", "--no-banner");
}

sub CleanCoverageData
{
    if (-e $ENV{'COVFILE'})
    {
        my $covclear = "$ENV{'BULLSEYEBIN'}covclear";
        SystemWrapper("$covclear", "--quiet", "--no-banner");
    }
    return 0;
}

sub GenerateReports
{
    my ($this, $suite, $targetPath, $root) = @_;

    my $covfile = "$ENV{'COVFILE'}";

    if (-d "$targetPath/$suite")
    {
        RmtreeSleepy("$targetPath/$suite") or croak("Can not remove $targetPath/$suite");
    }

    MkpathSleepy("$targetPath/$suite") or croak("Can not cleanup $targetPath/$suite");

    #copy coverage file
    my $suiteCoverageFile = "$targetPath/$suite/cov.cov";
    UCopy("$covfile", "$suiteCoverageFile");

    #copy tests.txt file
    UCopy("$targetPath/tests.txt", "$targetPath/$suite/tests.txt");

    #generate html report
    MkpathSleepy("$targetPath/$suite/cov.html") or croak("Failed to create $targetPath/$suite/cov.html");
    print("Generating html reports... It may take a few minutes.\n");
    SystemWrapper("\"$covhtml\" --srcdir \"$root\" \"$targetPath/$suite/cov.html\" --no-banner");
    SystemWrapper("\"$covxml\" --file $suiteCoverageFile --output \"$targetPath/$suite/cov.xml\" --no-banner");
    my $summaryFile = "$targetPath/$suite/summary.txt";
    WriteSuiteCoverageSummary($summaryFile, $suiteCoverageFile);

    return 0;
}

sub GenerateDescriptionFile
{
    my ($this, $platform, $osFamily, $version, $changeset, $revision, $date, $branch, $uri, $targetFile) = @_;
    my %description = (
        "Platform" => $platform,
        "OsFamily" => $osFamily,
        "Version" => $version,
        "Changeset" => $changeset,
        "Revision" => $revision,
        "Date" => $date,
        "Branch" => $branch,
        "Uri" => $uri,
    );

    WriteKeyValuesIntoTheFile($targetFile, %description);
}

sub Merge
{

    # at this moment $summaryDir should contain data for each individual test suite
    my ($this, $summaryDir, $root) = @_;

    if (-d "$summaryDir/overall")
    {
        RmtreeSleepy("$summaryDir/overall") or croak("Failed to delete $summaryDir/overall");
    }

    my @covFiles = glob("$summaryDir/*/cov.cov");

    my $covmerge = "$ENV{'BULLSEYEBIN'}covmerge";
    my $targetCoverageFile = "$summaryDir/overall/cov.cov";

    MkpathSleepy("$summaryDir/overall") or croak("Failed creating path: $summaryDir/overall");

    print("Merging: \n[" . join("\n", @covFiles) . "]\n->\n$targetCoverageFile\n");

    my $srcFiles = join(' ', @covFiles);

    SystemWrapper("\"$covmerge\" --create --no-banner --file $targetCoverageFile $srcFiles");

    MkpathSleepy("$targetPath/overall/cov.html") or croak("Failed to create $targetPath/overall/cov.html");
    SystemWrapper("\"$covhtml\" --file $targetCoverageFile --srcdir \"$root\" \"$summaryDir/overall/cov.html\" --no-banner");
    SystemWrapper("\"$covxml\" --file $targetCoverageFile --output \"$summaryDir/overall/cov.xml\" --no-banner");
    my $summaryFile = "$summaryDir/overall/summary.txt";
    WriteSuiteCoverageSummary($summaryFile, $targetCoverageFile);
}

sub WriteSuiteCoverageSummary
{
    my ($summaryFile, $suiteCoverageFile) = @_;
    SystemWrapper("\"$covdir\"", "--csv", "-f$suiteCoverageFile", "--quiet", "--no-banner", "-o$summaryFile");
    my $CDCoverageLine = GetLastLineFromFile($summaryFile);

    SystemWrapper("\"$covdir\"", "--csv", "-f$suiteCoverageFile", "--decision", "--quiet", "--no-banner", "-o$summaryFile");
    my $DCoverageLine = GetLastLineFromFile($summaryFile);

    my ($fnCovered, $fnTotal, $cdCov, $cdTotal) = GetSummaryCoverageFromTotalLine($CDCoverageLine);
    my ($fnCovered, $fnTotal, $dCov, $dTotal) = GetSummaryCoverageFromTotalLine($DCoverageLine);

    my %summaryCoverage = (
        "FunctionsCovered" => $fnCovered,
        "FunctionsTotal" => $fnTotal,
        "ConditionsDecisionsCovered" => $cdCov,
        "ConditionsDecisionsTotal" => $cdTotal,
        "DecisionsCovered" => $dCov,
        "DecisionsTotal" => $dTotal,
    );

    WriteKeyValuesIntoTheFile($summaryFile, %summaryCoverage);
}

sub WriteKeyValuesIntoTheFile
{
    my ($filename, %data) = @_;
    open(DFILE, ">$filename");
    while (my ($k, $v) = each %data)
    {
        print(DFILE "$k=$v\n");
    }
    close(DFILE);
}

sub UploadPackage
{
    my ($this, $platform, $os, $version, $ver_revision, $ver_numericRevision, $ver_date, $branch, $targetUri, $storageUri) = @_;
    my $userAgent = LWP::UserAgent->new();
    my $req_url =
        "$targetUri?platform=$platform&os=$os&version=$version&changeset=$ver_revision&revision=$ver_numericRevision&date=$ver_date&branch=$branch&uri=$storageUri";
    print("marking request: $req_url\n");
    my $response = $userAgent->post($req_url);
    if ($response->is_error)
    {
        print($response->error_as_HTML . "\n");
    }
}

sub SystemWrapper
{
    my $exitcode = system(@_);
    if ($exitcode)
    {
        my $cmdTxt = join " ", @_;
        croak("Failed to execute command $cmdTxt. Exit code $exitcode");
    }
}

sub GetSummaryCoverageFromTotalLine
{
    my ($csvline) = @_;
    my @mertics = split(',', $csvline);
    my $fnCovered = @mertics[1];
    my $fnTotal = @mertics[2];
    my $cdCov = @mertics[4];
    my $cdTotal = @mertics[5];
    return ($fnCovered, $fnTotal, $cdCov, $cdTotal);
}

sub GetLastLineFromFile
{
    my ($targetFile) = @_;
    open my $info, $targetFile or croak("Could not open $file: $!");
    @lines = <$info>;
    my $last = @lines[-1];
    close $info;
    return $last;
}

1;
