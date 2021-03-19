use warnings;
use strict;

use Test::More;

BEGIN { use_ok('NativeSuiteOutputParser'); }

Creation:
{
    can_ok('NativeSuiteOutputParser', 'new');
    my $parser = _makeParser();
    isa_ok($parser, 'NativeSuiteOutputParser');
    can_ok('NativeSuiteOutputParser', 'getResults');
    can_ok('NativeSuiteOutputParser', 'processLine');
}

CanParseSuccesfullTest:
{
    my @lines = ("[DefaultSuite]\t\t\tAddMissingSnippetKeywords_PassingSubset_ReturnsBestCombo1\t\t\t\t\t\tPASS (321ms)");

    my @results = _parseLines(@lines);
    is(scalar(@results), 1, 'One succesfull result restored');

    my $result = $results[0];
    isnt($result, undef);
    is($result->getFixture(), 'DefaultSuite');
    is($result->getTest(), 'AddMissingSnippetKeywords_PassingSubset_ReturnsBestCombo1');
    ok($result->isSucceeded());
    is($result->getTime(), 321);
}

CanParseTestWithFailedCheck:
{
    my @lines = (
        "[BitUtilityTests]\t\t\t Math_BitsInMask\t\t\t\t\t\t\t\t\t\tFAIL!!!!",
        "\tCHECK FAILURE: Expected 19 but was 0",
        "\t\t (C:/unity/lane1/Runtime/Utilities/BitUtilityTests.cpp:21)"
    );
    my $parser = _makeParser();
    $parser->processLine($lines[0]);
    my @results = $parser->getResults();
    is(scalar(@results), 1, 'One failed result restored');
    my $result = $results[0];
    isnt($result, undef);
    ok($result->isFailed());
    is($result->getFixture(), 'BitUtilityTests');
    is($result->getTime(), 0);
    is($result->getTest(), 'Math_BitsInMask');
    is($result->getMessage(), undef);

    $parser->processLine($lines[1]);
    is(scalar(@results), 1, 'Test result count remains the same when reading error message');
    is($result->getMessage(), "\tCHECK FAILURE: Expected 19 but was 0", 'Test resullt error message contains first message part when met in log');

    $parser->processLine($lines[2]);
    is(scalar(@results), 1, 'Test result count remains the same when reading error message');
    is(
        $result->getMessage(),
        "\tCHECK FAILURE: Expected 19 but was 0\n\t\t (C:/unity/lane1/Runtime/Utilities/BitUtilityTests.cpp:21)",
        'Error message has been appended'
    );
}

CanParseDisabledTest:
{
    my @lines = ('[ShaderWriterTests] Test1 DISABLED (0ms)');

    my @results = _parseLines(@lines);

    is(scalar(@results), 1);
    ok($results[0]->isIgnored());
}

CanParseComplexMultilineErrorMessage:
{
    my @lines = ("[Suite]\t\t TestName\t\t\t\t\t\t\t\t\t\t FAIL!!!!", "\tUNEXPECTED Error: BLABLA", "error continuation 1", "\n", "\t\terror continuation 2");

    my @results = _parseLines(@lines);
    is(scalar(@results), 1, 'One failed result restored');

    my $result = $results[0];
    isnt($result, undef);
    is($result->getMessage(), "\tUNEXPECTED Error: BLABLA\nerror continuation 1\n\n\t\terror continuation 2");
}

CanParseSucesfullTestAfterFailedTest:
{
    my @lines = (
        "[Suite]\t\t TestName\t\t\t\t\t\t\t\t\t\t FAIL!!!!",
        "\tUNEXPECTED Error: BLABLA",
        "error continuation 1",
        "\n",
        "\t\terror continuation 2",
        "[GeometryToolbox_MathTest]\t\t Basic\t\t\t\t\t\t\t\t\t\t\tPASS (0ms)"
    );
    my @results = _parseLines(@lines);
    is(scalar(@results), 2, 'One failed result restored');

    my $result = $results[0];
    isnt($result, undef);
    is($result->getMessage(), "\tUNEXPECTED Error: BLABLA\nerror continuation 1\n\n\t\terror continuation 2");

    $result = $results[1];
    is($result->getMessage(), undef);
}

CanDealWithSummary:
{
    my @lines = (
        "[GeometryToolbox_MathTest]\t\t Basic\t\t\t\t\t\t\t\t\t\t\tPASS (0ms)",
        "[Suite]\t\t TestName\t\t\t\t\t\t\t\t\t\t FAIL!!!!",
        "\tUNEXPECTED Error: BLABLA",
        "error continuation 1",
        "\n",
        "\t\terror continuation 2",
        "Ran 2 tests with 1 failures in 0.26 seconds"
    );

    my @results = _parseLines(@lines);
    is(scalar(@results), 2, 'One failed result restored');

    my $result = $results[0];
    is($result->getMessage(), undef);
    ok($result->isSucceeded(), 'Test is succeeded');

    $result = $results[1];
    isnt($result, undef);
    is($result->getMessage(), "\tUNEXPECTED Error: BLABLA\nerror continuation 1\n\n\t\terror continuation 2");
}

CanDealWithmessageInbetweenTestAndStatus:
{
    my @lines = (
        "[ShaderWriterTests]                                     LightmapShaderVariants_AreKept_WhenAllLightmapModesAreUsed                                                                   Launching external process: /Users/yan/unity/lane1/build/MacEditor/Unity.app/Contents/Tools/UnityShaderCompiler",
        "PASS (100ms)"
    );
    my @results = _parseLines(@lines);
    is(scalar(@results), 1);
    my $result = $results[0];
    ok($result->isSucceeded());
}

CanDealWithMultilineMessageInbetweenTestAndStatus:
{
    my @lines = (
        "[ArchiveStorageConverterTests] Conversion_UnityWeb_To_Lz4_Archive_Works ",
        "LZMA 4.43 Copyright (c) 1999-2006 Igor Pavlov  2006-06-04",
        "PASS (2681ms)"
    );

    my @results = _parseLines(@lines);
    my $result = $results[0];
    is(scalar(@results), 1);
}

CanDealWithTestNameWithSpaces:
{
    my @lines = ("[DefaultSuite] triangulation_problem DISABLED", "MacOnly");
    my @results = _parseLines(@lines);
    my $result = $results[0];
    is(scalar(@results), 1);
    ok($result->isIgnored());
    is($result->getMessage(), 'MacOnly');
}

done_testing();

sub _makeParser
{
    my $parser = new NativeSuiteOutputParser();
    $parser->setSuite('native');
    return $parser;
}

sub _parseLines
{
    my (@lines) = @_;
    my $parser = _makeParser();
    foreach my $line (@lines)
    {
        $parser->processLine($line);
    }
    return $parser->getResults();
}
