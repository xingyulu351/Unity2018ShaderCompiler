use warnings;
use strict;

use Test::More;
use Dirs;
use ExtendedProfile::Suite;
use lib Dirs::external_root();
use Test::MockModule;
use FileUtils;
use Test::Deep;
use TestHelpers::ArrayAssertions qw(arraysAreEquivalent);

BEGIN { use_ok('ExtendedProfileReader'); }

read_EmptyProfileFile_ReturnsNothing:
{
    my $result = ExtendedProfileReader::read(undef);
    is($result, undef);
}

my $defaultContent = <<END_FILE;
{
    "suites": [
        {
            "name":"native",
            "args":["--testfilter=VR"],
            "details":"more detailed description"
        }
    ]
}
END_FILE

my $fileUtils = new Test::MockModule('FileUtils');
$fileUtils->mock(
    readAllLines => sub
    {
        return $defaultContent;
    }
);

read_ProfileContainsOneSuite_ReturnsIt:
{
    my $result = ExtendedProfileReader::read('file_name');
    my $expectedSuite = new ExtendedProfile::Suite(
        name => 'native',
        args => ['--testfilter=VR'],
        details => 'more detailed description'
    );

    is($expectedSuite->getName(), 'native');
    my @args = $expectedSuite->getArgs();
    is_deeply($result->getSuites(), $expectedSuite);
}

read_ArgvIsProvided_ReturnsCombinedArguments:
{
    my $commandLine = ['--arg1=v1', '--arg2=v2'];
    my $result = ExtendedProfileReader::read('file_name', $commandLine);
    my $expectedSuite = new ExtendedProfile::Suite(
        name => 'native',
        args => ['--testfilter=VR'],
        combinedArgs => ['--testfilter=VR', '--arg1=v1', '--arg2=v2']
    );

    my @suites = $result->getSuites();
    my $suite = $suites[0];
    my @combinedArgs = $suite->getCombinedArgs();
    my @expectedCombinedArgs = $expectedSuite->getCombinedArgs();

    is_deeply(\@combinedArgs, \@expectedCombinedArgs);
}

read_ArgvHasPrioriy_ReturnsCombinedArguments:
{
    my $fileUtils = new Test::MockModule('FileUtils');
    my $content = <<END_FILE;
{
    "suites": [
        {
            "name":"native",
            "args":["--testfilter=VR", "--arg1=value_from_profile"],
            "commandLineHasPriority":1
        }
    ]
}
END_FILE
    $fileUtils->mock(
        readAllLines => sub
        {
            return $content;
        }
    );

    my $commandLine = ['--arg1=value_from_command_line1', '--arg2=value_from_command_line2'];
    my $result = ExtendedProfileReader::read('file_name', $commandLine);
    my $expectedSuite = new ExtendedProfile::Suite(
        name => 'native',
        args => ['--testfilter=VR', '--arg1=value_from_command_line1'],
        combinedArgs => ['--testfilter=VR', '--arg1=value_from_command_line1', '--arg2=value_from_command_line2']
    );

    my @suites = $result->getSuites();
    my $suite = $suites[0];
    my @combinedArgs = $suite->getCombinedArgs();
    my @expectedCombinedArgs = $expectedSuite->getCombinedArgs();
    arraysAreEquivalent(\@expectedCombinedArgs, \@combinedArgs);
}

#TODO: description
#TODO: croaks on invalid json
done_testing();
