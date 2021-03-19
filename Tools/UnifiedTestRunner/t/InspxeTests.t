use Test::More;
use TestHelpers::SuiteTools;
use Artifacts;

BEGIN { use_ok('SuiteTools::Inspxe') }

CREATION:
{
    can_ok('SuiteTools::Inspxe', 'new');
    my $inspxe = SuiteTools::Inspxe->new();
    isa_ok($inspxe, 'SuiteTools::Inspxe');
    can_ok($inspxe, 'getName');
    is($inspxe->getName(), 'inspxe');
}

RUN_CONTEXT:
{
    my $inspxe = new SuiteTools::Inspxe();
    my @args = ("--analysis-type=ti2");
    my $runContext = TestHelpers::SuiteTools::createRunContext($inspxe, @args);
    $inspxe->setRunContext($runContext);
    is($inspxe->getRunContext(), $runContext);
}

ARTIFACTS:
{
    my $inspxe = SuiteTools::Inspxe->new();
    can_ok($inspxe, 'setArtifacts');
    can_ok($inspxe, 'getArtifacts');
    my $artifacts = Artifacts->new();
    $inspxe->setArtifacts($artifacts);
    is($inspxe->getArtifacts(), $artifacts);
}

DECORATE:
{
    my $root = Dirs::getRoot();
    my $suppressionFile = "$root/Tools/DynamicAnalysis/suppressions/inspxe/default.sup";
    my @args = ("--analysis-type=ti2", "--suppression-file=$root/sup.txt", "--artifacts_path=c:/artifacts");

    my $inspxe = new SuiteTools::Inspxe();
    my $runContext = TestHelpers::SuiteTools::createRunContext($inspxe, @args);
    $inspxe->setRunContext($runContext);

    can_ok($inspxe, 'decorate');

    my @args = ('pgm', '--arg=1', '--arg2=2');
    my $inspxebin = '/opt/inspxe/bin';
    $ENV{'INSPXEBIN'} = $inspxebin;

    my $analysis_type = "ti2";    # deadlocks and data races
    @args = $inspxe->decorate(@args);
    is($args[0], "\"${inspxebin}inspxe-cl\"");
    is($args[1], '-collect');
    is($args[2], 'ti2');
    is($args[3], "-s-f=$root/sup.txt");
    is($args[4], '-result-dir=c:/artifacts');
    is($args[5], '-return-app-exitcode');
    is($args[6], '--');
    is($args[7], 'pgm');
    is($args[8], '--arg=1');
    is($args[9], '--arg2=2');
}

done_testing();
