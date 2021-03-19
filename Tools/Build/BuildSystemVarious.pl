use File::Basename qw (dirname);
use File::Spec;
use lib File::Spec->rel2abs(dirname($0) . '/../../External/Perl/lib');

use File::Path qw/make_path/;
use File::Copy::Recursive qw/dircopy/;

use lib File::Spec->rel2abs(dirname($0) . '/../../PlatformDependent/AndroidPlayer/Tools');
use lib File::Spec->rel2abs(dirname($0));
use PrepareAndroidSDK qw(UnpackAndroidNDK UnpackAndroidSDK);

#katana will sometimes set TMPDIR variable to a directory that doens't exist. the mono AOT compiler chokes on that, so let's set it to a directory that we know exists
delete $ENV{'TMPDIR'};

# Note that this prepare step is necessary on Katana in order to run the tests. Usually tests will do their own prepare step,
# but to save time, we disabled it because we're already doing it here. If you want to remove this prepare from this script,
# you will need to modify the Katana config to make sure that the tests are getting what they need set up still.
print "\n\n*** Running BSV step: perl build.pl --prepare --preparingforhuman=0\n\n";
system("perl build.pl --prepare --preparingforhuman=0") && die("failed running prepare");

UnpackAndroidNDK();
UnpackAndroidSDK();

print "\n\n*** Running BSV step: perl jam.pl ProjectFiles\n\n";
system("perl jam.pl ProjectFiles") && die("failed building ProjectFiles");

my $dotnetcmd = "mono ";
$dotnetcmd = "" if ($^O eq "MSWin32");

#this builds bee distributions and buildsystem tests
print "\n\n*** Running BSV step: build bee distributions and buildsystem tests\n\n";
system("$dotnetcmd Tools/BeeBootstrap/bee_bootstrap.exe all_tundra_nodes") && die("failed running bee bootstrap");

#katana BuildBuildSystemVarious configs depend on this argument. Update there as well if changing it here.
if ($ARGV[0] !~ /^--noutr/)
{
    print "\n\n*** Running BSV step: run buildsystem tests through UTR\n\n";
    system("perl utr.pl --suite=buildsystem --testresultsxml=TestResult.xml --testresultsformat=nunit2") && die("failed running tests for build system");
}

my $artifactsDir = 'build/ReportedArtifacts';
if (-e $artifactsDir)
{
    print "\n\n*** Copying build profiles to artifacts directory: ";
    my $copycount = dircopy('artifacts/BuildProfile', "$artifactsDir/BuildProfile");
    print "$copycount files copied.\n\n";
}
else
{
    print "\n\n*** Skipping copy of build profiles to output because $artifactsDir does not exist\n\n";
}
