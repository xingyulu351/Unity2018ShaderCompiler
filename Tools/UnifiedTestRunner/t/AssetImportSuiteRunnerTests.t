use warnings;
use strict;

use Test::More;
use RunContext;

BEGIN
{
    use_ok('SuiteRunners::AssetImport');
}

Creation:
{
    my $runner = createRunner();
    isa_ok($runner, 'SuiteRunners::AssetImport');
}

defaultValues:
{
    my $r = createRunner();
    is($r->getName(), 'assetimport');
    is($r->getOptionValue('platform'), 'editmode');
    is($r->getOptionValue('projectlist'), 'Tests/3DAssetImportTest/projectlist.txt');
}

done_testing();

sub createRunner
{
    my $r = new SuiteRunners::AssetImport();
    my $options = new Options(options => $r->getOptions());
    my $runContext = RunContext::new($options);
    $r->setRunContext($runContext);
    return $r;
}
