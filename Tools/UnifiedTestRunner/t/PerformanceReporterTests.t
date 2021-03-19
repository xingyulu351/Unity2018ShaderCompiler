use warnings;
use strict;

use Dirs;
use lib Dirs::external_root();

use Test::More;

use Test::MockObject;
use Environment;

BEGIN { use_ok('PerformanceReporter'); }

my $env = new Test::MockObject();

#TODO: testresultscsv is undef,
#TODO: croakcs is non zero exit code?
BasicScenario_runDotNetProgram:
{
    my $invoked = 0;
    my %args;
    $env->mock(
        runDotNetProgram => sub
        {
            (undef, %args) = @_;
            return 42;
        }
    );

    my $result = PerformanceReporter::report(
        $env,
        reporterPath => 'path_to_reporter',
        platform => 'platform_name',
        suiteName => 'test_suite_name',
        testResultsXml => 'path_to_test_result_xml',
        testResultsJson => 'path_to_test_result_json',
        outputJson => 'path_to_output_json',
        csvResultsFile => 'path_to_test_result_csv',
        stdOut => 'std_out_path',
        stdErr => 'std_err_path',
    );

    is($args{program}, 'path_to_reporter');
    is($result, 42);
    is_deeply(
        $args{programArgs},
        [
            '--platform=platform_name', '--testsuite=test_suite_name',
            '--testresultsxml=path_to_test_result_xml', '--testresultsjson=path_to_test_result_json',
            '--outputjson=path_to_output_json', '--testresultscsv=path_to_test_result_csv',
            '1>std_out_path', '2>std_err_path',
        ]
    );
}

done_testing();
