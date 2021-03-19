use warnings;
use strict;

use TestHelpers::ArrayAssertions qw (isEmpty arraysAreEqual);
use File::Temp qw (tempfile);
use Test::More;

BEGIN
{
    use_ok('ArtifactsMapBuilder');
}

fromNativeSuiteLog_NoTests_ProducesMapWithNoTests:
{
    my $content = '';
    my $artifactsMap = readMap($content);
    my @artifacts = $artifactsMap->getArtifacts();
    isEmpty(\@artifacts);
}

fromNativeSuiteLog_OneTestOneArtifacts_ProducesResultWithOneTestAndOneArtifact:
{
    my $content = <<END_MESSAGE;
=> NS.Test1
##teamcity[message text='Published: log1' status='NORMAL']
END_MESSAGE
    my $artifactsMap = readMap($content);
    my @artifacts = $artifactsMap->getArtifacts();
    arraysAreEqual(\@artifacts, ['log1']);
}

done_testing();

sub readMap
{
    my ($content, $artifactsRoot) = @_;
    my ($fh, $tempFileName) = tempfile();
    print($fh $content);
    close($fh);
    my $artifactsMap = ArtifactsMapBuilder::fromNativeSuiteLog($tempFileName, $artifactsRoot);
    return $artifactsMap;
}
