use warnings;
use strict;

use Test::More;
use TestHelpers::ArrayAssertions qw (isEmpty);
use Dirs;
use lib Dirs::external_root();
use Test::Trap;

BEGIN { use_ok('NUnitXmlParser'); }

parse_TestResultsXmlWithNoTests_ReturnsNoTests:
{

    my $xml = <<END;
<?xml version="1.0" encoding="utf-8" standalone="no"?>
<!--This file represents the results of running a test suite-->
<test-results name="C:\\unity\\lab\\NUnitTests\\NUnitTests\\bin\\Debug\\NUnitTests.dll" total="0" errors="0" failures="0" not-run="0" inconclusive="0" ignored="0" skipped="0" invalid="0" date="2015-08-31" time="16:33:43">
    <environment nunit-version="2.6.4.0" clr-version="2.0.50727.8009" os-version="Microsoft Windows NT 6.2.9200.0" platform="Win32NT" cwd="C:\\unity\\lab\\NUnitTests\\NUnitTests" machine-name="YAN-PC" user="yan" user-domain="YAN-PC" />
    <culture-info current-culture="en-US" current-uiculture="en-US" />
    <test-suite type="Assembly" name="C:\\unity\\lab\\NUnitTests\\NUnitTests\\bin\\Debug\\NUnitTests.dll" executed="True" result="Inconclusive" success="False" time="0.016" asserts="0">
        <results>
            <test-suite type="Namespace" name="NUnitTests" executed="True" result="Inconclusive" success="False" time="0.007" asserts="0">
                <results>
                    <test-suite type="TestFixture" name="Class1" executed="True" result="Inconclusive" success="False" time="0.005" asserts="0" />
                </results>
            </test-suite>
        </results>
    </test-suite>
</test-results>
END
    my %result = NUnitXmlParser::parse($xml);
    my @tests = @{ $result{testResults} };
    isEmpty(\@tests);
}

parse_TestResultsXmlWithOneSucessfullTest_ReturnsIt:
{

    my $xml = <<END;
<?xml version="1.0" encoding="utf-8" standalone="no"?>
<!--This file represents the results of running a test suite-->
<test-results name="C:\\unity\\lab\\NUnitTests\\NUnitTests\\bin\\Debug\\NUnitTests.dll" total="1" errors="0" failures="0" not-run="0" inconclusive="0" ignored="0" skipped="0" invalid="0" date="2015-08-31" time="16:39:02">
    <environment nunit-version="2.6.4.0" clr-version="2.0.50727.8009" os-version="Microsoft Windows NT 6.2.9200.0" platform="Win32NT" cwd="C:\\unity\\lab\\NUnitTests\\NUnitTests" machine-name="YAN-PC" user="yan" user-domain="YAN-PC" />
    <culture-info current-culture="en-US" current-uiculture="en-US" />
    <test-suite type="Assembly" name="C:\\unity\\lab\\NUnitTests\\NUnitTests\\bin\\Debug\\NUnitTests.dll" executed="True" result="Success" success="True" time="0.039" asserts="0">
        <results>
            <test-suite type="Namespace" name="NUnitTests" executed="True" result="Success" success="True" time="0.029" asserts="0">
                <results>
                    <test-suite type="TestFixture" name="Class1" executed="True" result="Success" success="True" time="0.028" asserts="0">
                        <results>
                            <test-case name="NUnitTests.Class1.GreatSuccess" executed="True" result="Success" success="True" time="0.021" asserts="1" />
                        </results>
                    </test-suite>
                </results>
            </test-suite>
        </results>
    </test-suite>
</test-results>
END
    my %result = NUnitXmlParser::parse($xml);
    my @testResults = @{ $result{testResults} };
    is(scalar(@testResults), 1);
    my $testResult = $testResults[0];
    is($testResult->getTest(), 'NUnitTests.Class1.GreatSuccess');
    is($testResult->getFixture(), undef);
    is($testResult->getState(), TestResult::SUCCESS);
    is($testResult->getTime(), 21);
    is($testResult->getMessage(), undef);
}

parse_InvalidFile_Croaks:
{
    my @trap = trap
    {
        my %result = NUnitXmlParser::parse('files_that_does_not_exists');
    };
    ok(defined($trap->die));
}

parse_TestResultsXmlWithOneSucessfullTest_ReturnsIt:
{

    my $xml = <<END;
<?xml version="1.0" encoding="utf-8" standalone="no"?>
<!--This file represents the results of running a test suite-->
<test-results name="C:\\unity\\lab\\NUnitTests\\NUnitTests\\bin\\Debug\\NUnitTests.dll" total="1" errors="0" failures="0" not-run="0" inconclusive="0" ignored="0" skipped="0" invalid="0" date="2015-08-31" time="16:39:02">
    <environment nunit-version="2.6.4.0" clr-version="2.0.50727.8009" os-version="Microsoft Windows NT 6.2.9200.0" platform="Win32NT" cwd="C:\\unity\\lab\\NUnitTests\\NUnitTests" machine-name="YAN-PC" user="yan" user-domain="YAN-PC" />
    <culture-info current-culture="en-US" current-uiculture="en-US" />
    <test-suite type="Assembly" name="C:\\unity\\lab\\NUnitTests\\NUnitTests\\bin\\Debug\\NUnitTests.dll" executed="True" result="Success" success="True" time="0.039" asserts="0">
        <results>
            <test-suite type="Namespace" name="NUnitTests" executed="True" result="Success" success="True" time="0.029" asserts="0">
                <results>
                    <test-suite type="TestFixture" name="Class1" executed="True" result="Success" success="True" time="0.028" asserts="0">
                        <results>
                            <test-case name="NUnitTests.Class1.GreatSuccess" executed="True" result="Success" success="True" time="0.021" asserts="1" />
                        </results>
                    </test-suite>
                </results>
            </test-suite>
        </results>
    </test-suite>
</test-results>
END
    my %result = NUnitXmlParser::parse($xml);
    my @testResults = @{ $result{testResults} };
    is(scalar(@testResults), 1);
    my $testResult = $testResults[0];
    is($testResult->getTest(), 'NUnitTests.Class1.GreatSuccess');
    is($testResult->getFixture(), undef);
    is($testResult->getState(), TestResult::SUCCESS);
    is($testResult->getTime(), 21);
    is($testResult->getMessage(), undef);
}

parse_TestResultsXmlWithOneFailedTest_ReturnsIt:
{
    my $xml = <<END;
<?xml version="1.0" encoding="utf-8" standalone="no"?>
<!--This file represents the results of running a test suite-->
<test-results name="C:\\unity\\lab\\NUnitTests\\NUnitTests\\bin\\Debug\\NUnitTests.dll" total="1" errors="0" failures="1" not-run="0" inconclusive="0" ignored="0" skipped="0" invalid="0" date="2015-08-31" time="18:15:30">
    <environment nunit-version="2.6.4.0" clr-version="2.0.50727.8009" os-version="Microsoft Windows NT 6.2.9200.0" platform="Win32NT" cwd="C:\\unity\\lab\\NUnitTests\\NUnitTests" machine-name="YAN-PC" user="yan" user-domain="YAN-PC" />
    <culture-info current-culture="en-US" current-uiculture="en-US" />
    <test-suite type="Assembly" name="C:\\unity\\lab\\NUnitTests\\NUnitTests\\bin\\Debug\\NUnitTests.dll" executed="True" result="Failure" success="False" time="0.092" asserts="0">
        <results>
            <test-suite type="Namespace" name="NUnitTests" executed="True" result="Failure" success="False" time="0.083" asserts="0">
                <results>
                    <test-suite type="TestFixture" name="Class1" executed="True" result="Failure" success="False" time="0.082" asserts="0">
                        <results>
                            <test-case name="NUnitTests.Class1.FailedTest" executed="True" result="Failure" success="False" time="0.075" asserts="1">
                                <failure>
                                    <message><![CDATA[  Expected: 1
    But was:    0
]]></message>
                                    <stack-trace><![CDATA[at NUnitTests.Class1.FailedTest() in c:\\unity\\lab\\NUnitTests\\NUnitTests\\Class1.cs:line 18
]]></stack-trace>
                                </failure>
                            </test-case>
                        </results>
                    </test-suite>
                </results>
            </test-suite>
        </results>
    </test-suite>
</test-results>
END

    my %result = NUnitXmlParser::parse($xml);
    my @testResults = @{ $result{testResults} };
    is(scalar(@testResults), 1);
    my $testResult = $testResults[0];
    is($testResult->getTest(), 'NUnitTests.Class1.FailedTest');
    is($testResult->getFixture(), undef);
    is($testResult->getState(), TestResult::FAILURE);
    is($testResult->getTime(), 75);
    like($testResult->getMessage(), qr/.*Expected:\s+[1]\n\s+But was:\s+[0]/);
    like($testResult->getStackTrace(), qr/NUnitTests.Class1.FailedTest\(\)/);
    is($testResult->getReason(), undef);
}

parse_TestResultsXmlWithOneInconclusiveTest_ReturnsIt:
{
    my $xml = <<END;
<?xml version="1.0" encoding="utf-8" standalone="no"?>
<!--This file represents the results of running a test suite-->
<test-results name="C:\\unity\\lab\\NUnitTests\\NUnitTests\\bin\\Debug\\NUnitTests.dll" total="1" errors="0" failures="0" not-run="0" inconclusive="1" ignored="0" skipped="0" invalid="0" date="2015-08-31" time="19:05:59">
    <environment nunit-version="2.6.4.0" clr-version="2.0.50727.8009" os-version="Microsoft Windows NT 6.2.9200.0" platform="Win32NT" cwd="C:\\unity\\lab\\NUnitTests\\NUnitTests" machine-name="YAN-PC" user="yan" user-domain="YAN-PC" />
    <culture-info current-culture="en-US" current-uiculture="en-US" />
    <test-suite type="Assembly" name="C:\\unity\\lab\\NUnitTests\\NUnitTests\\bin\\Debug\\NUnitTests.dll" executed="True" result="Inconclusive" success="False" time="0.035" asserts="0">
        <results>
            <test-suite type="Namespace" name="NUnitTests" executed="True" result="Inconclusive" success="False" time="0.025" asserts="0">
                <results>
                    <test-suite type="TestFixture" name="Class1" executed="True" result="Inconclusive" success="False" time="0.024" asserts="0">
                        <results>
                            <test-case name="NUnitTests.Class1.InconclussiveTest" executed="True" result="Inconclusive" success="False" time="0.017" asserts="0">
                                <reason>
                                    <message><![CDATA[Inconclusive Test]]></message>
                                </reason>
                            </test-case>
                        </results>
                    </test-suite>
                </results>
            </test-suite>
        </results>
    </test-suite>
</test-results>
END
    my %result = NUnitXmlParser::parse($xml);
    my @testResults = @{ $result{testResults} };
    is(scalar(@testResults), 1);
    my $testResult = $testResults[0];
    is($testResult->getTest(), 'NUnitTests.Class1.InconclussiveTest');
    is($testResult->getFixture(), undef);
    is($testResult->getState(), TestResult::INCONCLUSIVE);
    is($testResult->getTime(), 17);
    is($testResult->getMessage(), undef);
    is($testResult->getStackTrace(), undef);
    is($testResult->getReason(), 'Inconclusive Test');
}

parse_TestResultsXmlWithOneSkippedTest_ReturnsIt:
{
    my $xml = <<END;
<?xml version="1.0" encoding="utf-8" standalone="no"?>
<!--This file represents the results of running a test suite-->
<test-results name="C:\\unity\\lab\\NUnitTests\\NUnitTests\\bin\\Debug\\NUnitTests.dll" total="0" errors="0" failures="0" not-run="1" inconclusive="0" ignored="1" skipped="0" invalid="0" date="2015-08-31" time="19:25:43">
    <environment nunit-version="2.6.4.0" clr-version="2.0.50727.8009" os-version="Microsoft Windows NT 6.2.9200.0" platform="Win32NT" cwd="C:\\unity\\lab\\NUnitTests\\NUnitTests" machine-name="YAN-PC" user="yan" user-domain="YAN-PC" />
    <culture-info current-culture="en-US" current-uiculture="en-US" />
    <test-suite type="Assembly" name="C:\\unity\\lab\\NUnitTests\\NUnitTests\\bin\\Debug\\NUnitTests.dll" executed="True" result="Inconclusive" success="False" time="0.035" asserts="0">
        <results>
            <test-suite type="Namespace" name="NUnitTests" executed="True" result="Inconclusive" success="False" time="0.025" asserts="0">
                <results>
                    <test-suite type="TestFixture" name="Class1" executed="True" result="Inconclusive" success="False" time="0.023" asserts="0">
                        <results>
                            <test-case name="NUnitTests.Class1.Skipped" executed="False" result="Ignored">
                                <reason>
                                    <message><![CDATA[Skipped Test]]></message>
                                </reason>
                            </test-case>
                        </results>
                    </test-suite>
                </results>
            </test-suite>
        </results>
    </test-suite>
</test-results>
END
    my %result = NUnitXmlParser::parse($xml);
    my @testResults = @{ $result{testResults} };
    is(scalar(@testResults), 1);
    my $testResult = $testResults[0];
    is($testResult->getTest(), 'NUnitTests.Class1.Skipped');
    is($testResult->getFixture(), undef);
    is($testResult->getState(), TestResult::IGNORED);
    is($testResult->getTime(), 0);
    is($testResult->getMessage(), undef);
    is($testResult->getStackTrace(), undef);
    is($testResult->getReason(), 'Skipped Test');
}

parse_TestResultsXmlWithOneErrorTest_ReturnsIt:
{
    my $xml = <<END;
<?xml version="1.0" encoding="utf-8" standalone="no"?>
<!--This file represents the results of running a test suite-->
<test-results name="C:\\unity\\lab\\NUnitTests\\NUnitTests\\bin\\Debug\\NUnitTests.dll" total="1" errors="1" failures="0" not-run="0" inconclusive="0" ignored="0" skipped="0" invalid="0" date="2015-08-31" time="19:29:32">
    <environment nunit-version="2.6.4.0" clr-version="2.0.50727.8009" os-version="Microsoft Windows NT 6.2.9200.0" platform="Win32NT" cwd="C:\\unity\\lab\\NUnitTests\\NUnitTests" machine-name="YAN-PC" user="yan" user-domain="YAN-PC" />
    <culture-info current-culture="en-US" current-uiculture="en-US" />
    <test-suite type="Assembly" name="C:\\unity\\lab\\NUnitTests\\NUnitTests\\bin\\Debug\\NUnitTests.dll" executed="True" result="Failure" success="False" time="0.034" asserts="0">
        <results>
            <test-suite type="Namespace" name="NUnitTests" executed="True" result="Failure" success="False" time="0.024" asserts="0">
                <results>
                    <test-suite type="TestFixture" name="Class1" executed="True" result="Failure" success="False" time="0.023" asserts="0">
                        <results>
                            <test-case name="NUnitTests.Class1.ErrorTest" executed="True" result="Error" success="False" time="0.017" asserts="0">
                                <failure>
                                    <message><![CDATA[System.Exception : ERROR!!! interrupted]]></message>
                                    <stack-trace><![CDATA[at NUnitTests.Class1.ErrorTest() in c:\\unity\\lab\\NUnitTests\\NUnitTests\\Class1.cs:line 25
]]></stack-trace>
                                </failure>
                            </test-case>
                        </results>
                    </test-suite>
                </results>
            </test-suite>
        </results>
    </test-suite>
</test-results>
END
    my %result = NUnitXmlParser::parse($xml);
    my @testResults = @{ $result{testResults} };
    is(scalar(@testResults), 1);
    my $testResult = $testResults[0];
    is($testResult->getTest(), 'NUnitTests.Class1.ErrorTest');
    is($testResult->getFixture(), undef);
    is($testResult->getState(), TestResult::ERROR);
    is($testResult->getTime(), 17);
    is($testResult->getMessage(), 'System.Exception : ERROR!!! interrupted');
    like($testResult->getStackTrace(), qr/NUnitTests.Class1.ErrorTest\(\)/);
    is($testResult->getReason(), undef);
}

parse_RealityCheck_ReturnsIt:
{
    my $xml = <<END;
<?xml version="1.0" encoding="utf-8" standalone="no"?>
<!--This file represents the results of running a test suite-->
<test-results name="C:\\unity\\lab\\NUnitTests\\NUnitTests\\bin\\Debug\\NUnitTests.dll" total="3" errors="1" failures="1" not-run="0" inconclusive="0" ignored="0" skipped="0" invalid="0" date="2015-09-01" time="13:41:26">
    <environment nunit-version="2.6.4.0" clr-version="2.0.50727.8009" os-version="Microsoft Windows NT 6.2.9200.0" platform="Win32NT" cwd="C:\\unity\\lab\\NUnitTests\\NUnitTests" machine-name="YAN-PC" user="yan" user-domain="YAN-PC" />
    <culture-info current-culture="en-US" current-uiculture="en-US" />
    <test-suite type="Assembly" name="C:\\unity\\lab\\NUnitTests\\NUnitTests\\bin\\Debug\\NUnitTests.dll" executed="True" result="Failure" success="False" time="0.054" asserts="0">
    <results>
        <test-suite type="Namespace" name="NUnitTests" executed="True" result="Failure" success="False" time="0.044" asserts="0">
        <results>
            <test-suite type="TestFixture" name="Class1" executed="True" result="Failure" success="False" time="0.043" asserts="0">
            <results>
                <test-case name="NUnitTests.Class1.ErrorTest" executed="True" result="Error" success="False" time="0.017" asserts="0">
                <failure>
                    <message><![CDATA[System.Exception : ERROR!!! interrupted]]></message>
                    <stack-trace><![CDATA[at NUnitTests.Class1.ErrorTest() in c:\\unity\\lab\\NUnitTests\\NUnitTests\\Class1.cs:line 25
]]></stack-trace>
                </failure>
                </test-case>
                <test-case name="NUnitTests.Class1.FailedTest" executed="True" result="Failure" success="False" time="0.014" asserts="1">
                <failure>
                    <message><![CDATA[  Expected: 1
    But was:  0
]]></message>
                    <stack-trace><![CDATA[at NUnitTests.Class1.FailedTest() in c:\\unity\\lab\\NUnitTests\\NUnitTests\\Class1.cs:line 18
]]></stack-trace>
                </failure>
                </test-case>
                <test-case name="NUnitTests.Class1.GreatSuccess" executed="True" result="Success" success="True" time="0.000" asserts="1" />
            </results>
            </test-suite>
        </results>
        </test-suite>
    </results>
    </test-suite>
</test-results>
END
    my %result = NUnitXmlParser::parse($xml);
    my @testResults = @{ $result{testResults} };
    is(scalar(@testResults), 3);

    my $test3 = $testResults[2];

    my $errorTest = $testResults[0];
    is($errorTest->getTest(), 'NUnitTests.Class1.ErrorTest');
    is($errorTest->getFixture(), undef);
    is($errorTest->getState(), TestResult::ERROR);
    is($errorTest->getTime(), 17);
    is($errorTest->getMessage(), 'System.Exception : ERROR!!! interrupted');
    like($errorTest->getStackTrace(), qr/NUnitTests.Class1.ErrorTest\(\)/);
    is($errorTest->getReason(), undef);

    my $failedTest = $testResults[1];
    is($failedTest->getTest(), 'NUnitTests.Class1.FailedTest');
    is($failedTest->getFixture(), undef);
    is($failedTest->getState(), TestResult::FAILURE);
    is($failedTest->getTime(), 14);
    like($failedTest->getMessage(), qr/.*Expected:\s+[1]\n\s+But was:\s+[0]/);
    like($failedTest->getStackTrace(), qr/NUnitTests.Class1.FailedTest\(\)/);
    is($failedTest->getReason(), undef);

    my $test2 = $testResults[2];
    is($test2->getTest(), 'NUnitTests.Class1.GreatSuccess');
    is($test2->getFixture(), undef);
    is($test2->getState(), TestResult::SUCCESS);
    is($test2->getTime(), 0);
    is($test2->getMessage(), undef);
    is($test2->getReason(), undef);
    is($test2->getStackTrace(), undef);
}

# nunit 3.5 support
parse_NUnit35WithNoTests:
{
    my $xml = <<END;
<?xml version="1.0" encoding="utf-8" standalone="no"?>
<test-run id="2" testcasecount="0" result="Passed" total="0" passed="0" failed="0" inconclusive="0" skipped="0" asserts="0" engine-version="3.5.0.0" clr-version="4.0.30319.42000" start-time="2016-11-22 14:48:52Z" end-time="2016-11-22 14:48:52Z" duration="0.061768">
<command-line><![CDATA[c:\\bin\\nunit\\NUnit-3.5.0\\nunit3-console.exe  3_5_0Tests/bin/Debug/3_5_0Tests.dll --result=results/3_5_0.xml ]]></command-line>
<test-suite type="Assembly" id="0-1001" name="3_5_0Tests.dll" fullname="c:\\unity\\NUnitTests\\3_5_0Tests\\bin\\Debug\\3_5_0Tests.dll" runstate="Runnable" testcasecount="0" result="Passed" start-time="2016-11-22 14:48:52Z" end-time="2016-11-22 14:48:52Z" duration="0.028800" total="0" passed="0" failed="0" inconclusive="0" skipped="0" asserts="0">
<environment framework-version="3.5.0.0" clr-version="4.0.30319.42000" os-version="Microsoft Windows NT 6.3.9600.0" platform="Win32NT" cwd="c:\\unity\\NUnitTests" machine-name="YAN-PC" user="yan" user-domain="YAN-PC" culture="en-US" uiculture="en-US" os-architecture="x64" />
<settings>
    <setting name="DisposeRunners" value="True" />
    <setting name="WorkDirectory" value="c:\\unity\\NUnitTests" />
    <setting name="ImageRuntimeVersion" value="4.0.30319" />
    <setting name="ImageTargetFrameworkName" value=".NETFramework,Version=v4.5" />
    <setting name="ImageRequiresX86" value="False" />
    <setting name="ImageRequiresDefaultAppDomainAssemblyResolver" value="False" />
    <setting name="NumberOfTestWorkers" value="8" />
</settings>
<properties>
    <property name="_PID" value="21100" />
    <property name="_APPDOMAIN" value="test-domain-" />
</properties>
    <test-suite type="TestSuite" id="0-1002" name="NUnitTests" fullname="NUnitTests" runstate="Runnable" testcasecount="0" result="Passed" start-time="2016-11-22 14:48:52Z" end-time="2016-11-22 14:48:52Z" duration="0.011337" total="0" passed="0" failed="0" inconclusive="0" skipped="0" asserts="0">
        <test-suite type="TestFixture" id="0-1000" name="NUnitTests" fullname="NUnitTests.NUnitTests" classname="NUnitTests.NUnitTests" runstate="Runnable" testcasecount="0" result="Passed" start-time="2016-11-22 14:48:52Z" end-time="2016-11-22 14:48:52Z" duration="0.001638" total="0" passed="0" failed="0" inconclusive="0" skipped="0" asserts="0" />
        </test-suite>
    </test-suite>
</test-run>
END
    my %result = NUnitXmlParser::parse($xml);
    my @testResults = @{ $result{testResults} };
    is(scalar(@testResults), 0);
}

parse_NUnit35Passedest:
{
    my $xml = <<END;
<?xml version="1.0" encoding="utf-8" standalone="no"?>
    <test-run id="2" testcasecount="1" result="Passed" total="1" passed="1" failed="0" inconclusive="0" skipped="0" asserts="1" engine-version="3.5.0.0" clr-version="4.0.30319.42000" start-time="2016-11-22 14:58:21Z" end-time="2016-11-22 14:58:21Z" duration="0.170091">
    <command-line><![CDATA[c:\\bin\\nunit\\NUnit-3.5.0\\nunit3-console.exe  3_5_0Tests/bin/Debug/3_5_0Tests.dll --result=results/3_5_0.xml ]]></command-line>
    <test-suite type="Assembly" id="0-1002" name="3_5_0Tests.dll" fullname="C:\\unity\\NUnitTests\\3_5_0Tests\\bin\\Debug\\3_5_0Tests.dll" runstate="Runnable" testcasecount="1" result="Passed" start-time="2016-11-22 14:58:21Z" end-time="2016-11-22 14:58:21Z" duration="0.091190" total="1" passed="1" failed="0" inconclusive="0" skipped="0" asserts="1">
    <environment framework-version="3.5.0.0" clr-version="4.0.30319.42000" os-version="Microsoft Windows NT 6.3.9600.0" platform="Win32NT" cwd="C:\\unity\\NUnitTests" machine-name="YAN-PC" user="yan" user-domain="YAN-PC" culture="en-US" uiculture="en-US" os-architecture="x64" />
    <settings>
        <setting name="DisposeRunners" value="True" />
        <setting name="WorkDirectory" value="C:\\unity\\NUnitTests" />
        <setting name="ImageRuntimeVersion" value="4.0.30319" />
        <setting name="ImageTargetFrameworkName" value=".NETFramework,Version=v4.5" />
        <setting name="ImageRequiresX86" value="False" />
        <setting name="ImageRequiresDefaultAppDomainAssemblyResolver" value="False" />
        <setting name="NumberOfTestWorkers" value="8" />
    </settings>
    <properties>
        <property name="_PID" value="1152" />
        <property name="_APPDOMAIN" value="test-domain-" />
    </properties>
    <test-suite type="TestSuite" id="0-1003" name="NUnitTests" fullname="NUnitTests" runstate="Runnable" testcasecount="1" result="Passed" start-time="2016-11-22 14:58:21Z" end-time="2016-11-22 14:58:21Z" duration="0.060835" total="1" passed="1" failed="0" inconclusive="0" skipped="0" asserts="1">
        <test-suite type="TestFixture" id="0-1000" name="NUnitTests" fullname="NUnitTests.NUnitTests" classname="NUnitTests.NUnitTests" runstate="Runnable" testcasecount="1" result="Passed" start-time="2016-11-22 14:58:21Z" end-time="2016-11-22 14:58:21Z" duration="0.046252" total="1" passed="1" failed="0" inconclusive="0" skipped="0" asserts="1">
            <test-case id="0-1001" name="SuccessfullTest" fullname="NUnitTests.NUnitTests.SuccessfullTest" methodname="SuccessfullTest" classname="NUnitTests.NUnitTests" runstate="Runnable" seed="651518710" result="Passed" start-time="2016-11-22 14:58:21Z" end-time="2016-11-22 14:58:21Z" duration="0.021955" asserts="1" />
            </test-suite>
    </test-suite>
    </test-suite>
    </test-run>
END
    my %result = NUnitXmlParser::parse($xml);
    my @testResults = @{ $result{testResults} };
    is(scalar(@testResults), 1);
    my $testResult = $testResults[0];
    is($testResult->getTest(), 'NUnitTests.NUnitTests.SuccessfullTest');
    is($testResult->getFixture(), undef);
    is($testResult->getState(), TestResult::SUCCESS);
    is($testResult->getTime(), 21);
    is($testResult->getMessage(), undef);
}

parse_NUnit35FailedTest:
{
    my $xml = <<END;
<?xml version="1.0" encoding="utf-8" standalone="no"?>
<test-run id="2" testcasecount="1" result="Failed" total="1" passed="0" failed="1" inconclusive="0" skipped="0" asserts="1" engine-version="3.5.0.0" clr-version="4.0.30319.42000" start-time="2016-11-23 08:52:56Z" end-time="2016-11-23 08:52:56Z" duration="0.095233">
    <command-line><![CDATA[c:\\bin\\nunit\\NUnit-3.5.0\\nunit3-console.exe  3_5_0Tests/bin/Debug/3_5_0Tests.dll --result=results/3_5_0.xml ]]></command-line>
    <test-suite type="Assembly" id="0-1002" name="3_5_0Tests.dll" fullname="C:\\unity\\NUnitTests\\3_5_0Tests\\bin\\Debug\\3_5_0Tests.dll" runstate="Runnable" testcasecount="1" result="Failed" site="Child" start-time="2016-11-23 08:52:56Z" end-time="2016-11-23 08:52:56Z" duration="0.063030" total="1" passed="0" failed="1" inconclusive="0" skipped="0" asserts="1">
    <environment framework-version="3.5.0.0" clr-version="4.0.30319.42000" os-version="Microsoft Windows NT 6.3.9600.0" platform="Win32NT" cwd="C:\\unity\\NUnitTests" machine-name="YAN-PC" user="yan" user-domain="YAN-PC" culture="en-US" uiculture="en-US" os-architecture="x64" />
    <settings>
        <setting name="DisposeRunners" value="True" />
        <setting name="WorkDirectory" value="C:\\unity\\NUnitTests" />
        <setting name="ImageRuntimeVersion" value="4.0.30319" />
        <setting name="ImageTargetFrameworkName" value=".NETFramework,Version=v4.5" />
        <setting name="ImageRequiresX86" value="False" />
        <setting name="ImageRequiresDefaultAppDomainAssemblyResolver" value="False" />
        <setting name="NumberOfTestWorkers" value="8" />
    </settings>
    <properties>
        <property name="_PID" value="21576" />
        <property name="_APPDOMAIN" value="test-domain-" />
    </properties>
    <failure>
        <message><![CDATA[One or more child tests had errors]]></message>
    </failure>
    <test-suite type="TestSuite" id="0-1003" name="NUnitTests" fullname="NUnitTests" runstate="Runnable" testcasecount="1" result="Failed" site="Child" start-time="2016-11-23 08:52:56Z" end-time="2016-11-23 08:52:56Z" duration="0.046767" total="1" passed="0" failed="1" inconclusive="0" skipped="0" asserts="1">
            <failure>
            <message><![CDATA[One or more child tests had errors]]></message>
        </failure>
        <test-suite type="TestFixture" id="0-1000" name="NUnitTests" fullname="NUnitTests.NUnitTests" classname="NUnitTests.NUnitTests" runstate="Runnable" testcasecount="1" result="Failed" site="Child" start-time="2016-11-23 08:52:56Z" end-time="2016-11-23 08:52:56Z" duration="0.041038" total="1" passed="0" failed="1" inconclusive="0" skipped="0" asserts="1">
        <failure>
            <message><![CDATA[One or more child tests had errors]]></message>
        </failure>
        <test-case id="0-1001" name="FailedTest" fullname="NUnitTests.NUnitTests.FailedTest" methodname="FailedTest" classname="NUnitTests.NUnitTests" runstate="Runnable" seed="585566005" result="Failed" start-time="2016-11-23 08:52:56Z" end-time="2016-11-23 08:52:56Z" duration="0.030346" asserts="1">
        <failure>
            <message><![CDATA[  Expected: 1
    But was:  0
]]></message>
            <stack-trace><![CDATA[at NUnitTests.NUnitTests.FailedTest() in C:\\unity\\NUnitTests\\NUnitTests.cs:line 50
]]></stack-trace>
            </failure>
        </test-case>
        </test-suite>
    </test-suite>
    </test-suite>
</test-run>
END
    my %result = NUnitXmlParser::parse($xml);
    my @testResults = @{ $result{testResults} };
    is(scalar(@testResults), 1);
    my $testResult = $testResults[0];
    is($testResult->getTest(), 'NUnitTests.NUnitTests.FailedTest');

    # is ($testResult->getFixture (), undef);
    is($testResult->getState(), TestResult::FAILURE);
    is($testResult->getTime(), 30);
    is($testResult->getFixture(), undef);

    like($testResult->getMessage(), qr/.*Expected:\s+[1]\n\s+But was:\s+[0]/);
    like($testResult->getStackTrace(), qr/NUnitTests.NUnitTests.FailedTest\(\)/);
    is($testResult->getReason(), undef);
}

parse_NUnit35InconclusiveTest:
{
    my $xml = <<END;
<?xml version="1.0" encoding="utf-8" standalone="no"?>
<test-run id="2" testcasecount="1" result="Passed" total="1" passed="0" failed="0" inconclusive="1" skipped="0" asserts="0" engine-version="3.5.0.0" clr-version="4.0.30319.42000" start-time="2016-11-23 09:02:07Z" end-time="2016-11-23 09:02:07Z" duration="0.082648">
    <command-line><![CDATA[c:\\bin\\nunit\\NUnit-3.5.0\\nunit3-console.exe  3_5_0Tests/bin/Debug/3_5_0Tests.dll --result=results/3_5_0.xml ]]></command-line>
    <test-suite type="Assembly" id="0-1002" name="3_5_0Tests.dll" fullname="C:\\unity\\NUnitTests\\3_5_0Tests\\bin\\Debug\\3_5_0Tests.dll" runstate="Runnable" testcasecount="1" result="Passed" start-time="2016-11-23 09:02:07Z" end-time="2016-11-23 09:02:07Z" duration="0.049845" total="1" passed="0" failed="0" inconclusive="1" skipped="0" asserts="0">
        <environment framework-version="3.5.0.0" clr-version="4.0.30319.42000" os-version="Microsoft Windows NT 6.3.9600.0" platform="Win32NT" cwd="C:\\unity\\NUnitTests" machine-name="YAN-PC" user="yan" user-domain="YAN-PC" culture="en-US" uiculture="en-US" os-architecture="x64" />
        <settings>
            <setting name="DisposeRunners" value="True" />
            <setting name="WorkDirectory" value="C:\\unity\\NUnitTests" />
            <setting name="ImageRuntimeVersion" value="4.0.30319" />
            <setting name="ImageTargetFrameworkName" value=".NETFramework,Version=v4.5" />
            <setting name="ImageRequiresX86" value="False" />
            <setting name="ImageRequiresDefaultAppDomainAssemblyResolver" value="False" />
            <setting name="NumberOfTestWorkers" value="8" />
        </settings>
        <properties>
            <property name="_PID" value="19024" />
            <property name="_APPDOMAIN" value="test-domain-" />
        </properties>
        <test-suite type="TestSuite" id="0-1003" name="NUnitTests" fullname="NUnitTests" runstate="Runnable" testcasecount="1" result="Passed" start-time="2016-11-23 09:02:07Z" end-time="2016-11-23 09:02:07Z" duration="0.032332" total="1" passed="0" failed="0" inconclusive="1" skipped="0" asserts="0">
            <test-suite type="TestFixture" id="0-1000" name="NUnitTests" fullname="NUnitTests.NUnitTests" classname="NUnitTests.NUnitTests" runstate="Runnable" testcasecount="1" result="Passed" start-time="2016-11-23 09:02:07Z" end-time="2016-11-23 09:02:07Z" duration="0.026841" total="1" passed="0" failed="0" inconclusive="1" skipped="0" asserts="0">
                <test-case id="0-1001" name="InconclussiveTest" fullname="NUnitTests.NUnitTests.InconclussiveTest" methodname="InconclussiveTest" classname="NUnitTests.NUnitTests" runstate="Runnable" seed="1910632036" result="Inconclusive" start-time="2016-11-23 09:02:07Z" end-time="2016-11-23 09:02:07Z" duration="0.016416" asserts="0">
                    <reason>
                        <message><![CDATA[Inconclusive Test]]></message>
                    </reason>
                </test-case>
            </test-suite>
        </test-suite>
    </test-suite>
</test-run>
END
    my %result = NUnitXmlParser::parse($xml);
    my @testResults = @{ $result{testResults} };

    is(scalar(@testResults), 1);
    my $testResult = $testResults[0];
    is($testResult->getState(), TestResult::INCONCLUSIVE);
    is($testResult->getTest(), 'NUnitTests.NUnitTests.InconclussiveTest');
    is($testResult->getFixture(), undef);

    is($testResult->getTime(), 16);
    is($testResult->getMessage(), undef);
    is($testResult->getStackTrace(), undef);
    is($testResult->getReason(), 'Inconclusive Test');
}

parse_NUnit35IgnoredTest:
{
    my $xml = <<END;
<?xml version="1.0" encoding="utf-8" standalone="no"?>
<test-run id="2" testcasecount="1" result="Skipped" label="Ignored" total="1" passed="0" failed="0" inconclusive="0" skipped="1" asserts="0" engine-version="3.5.0.0" clr-version="4.0.30319.42000" start-time="2016-11-23 09:10:11Z" end-time="2016-11-23 09:10:11Z" duration="0.065226">
    <command-line><![CDATA[c:\\bin\\nunit\\NUnit-3.5.0\\nunit3-console.exe  3_5_0Tests/bin/Debug/3_5_0Tests.dll --result=results/3_5_0.xml ]]></command-line>
    <test-suite type="Assembly" id="0-1002" name="3_5_0Tests.dll" fullname="C:\\unity\\NUnitTests\\3_5_0Tests\\bin\\Debug\\3_5_0Tests.dll" runstate="Runnable" testcasecount="1" result="Skipped" label="Ignored" start-time="2016-11-23 09:10:11Z" end-time="2016-11-23 09:10:11Z" duration="0.032662" total="1" passed="0" failed="0" inconclusive="0" skipped="1" asserts="0">
        <environment framework-version="3.5.0.0" clr-version="4.0.30319.42000" os-version="Microsoft Windows NT 6.3.9600.0" platform="Win32NT" cwd="C:\\unity\\NUnitTests" machine-name="YAN-PC" user="yan" user-domain="YAN-PC" culture="en-US" uiculture="en-US" os-architecture="x64" />
        <settings>
            <setting name="DisposeRunners" value="True" />
            <setting name="WorkDirectory" value="C:\\unity\\NUnitTests" />
            <setting name="ImageRuntimeVersion" value="4.0.30319" />
            <setting name="ImageTargetFrameworkName" value=".NETFramework,Version=v4.5" />
            <setting name="ImageRequiresX86" value="False" />
            <setting name="ImageRequiresDefaultAppDomainAssemblyResolver" value="False" />
            <setting name="NumberOfTestWorkers" value="8" />
        </settings>
        <properties>
            <property name="_PID" value="23148" />
            <property name="_APPDOMAIN" value="test-domain-" />
        </properties>
        <reason>
            <message><![CDATA[One or more child tests were ignored]]></message>
        </reason>
        <test-suite type="TestSuite" id="0-1003" name="NUnitTests" fullname="NUnitTests" runstate="Runnable" testcasecount="1" result="Skipped" label="Ignored" start-time="2016-11-23 09:10:11Z" end-time="2016-11-23 09:10:11Z" duration="0.015654" total="1" passed="0" failed="0" inconclusive="0" skipped="1" asserts="0">
            <reason>
                <message><![CDATA[One or more child tests were ignored]]></message>
            </reason>
            <test-suite type="TestFixture" id="0-1000" name="NUnitTests" fullname="NUnitTests.NUnitTests" classname="NUnitTests.NUnitTests" runstate="Runnable" testcasecount="1" result="Skipped" label="Ignored" start-time="2016-11-23 09:10:11Z" end-time="2016-11-23 09:10:11Z" duration="0.010477" total="1" passed="0" failed="0" inconclusive="0" skipped="1" asserts="0">
                <reason>
                    <message><![CDATA[One or more child tests were ignored]]></message>
                </reason>
                <test-case id="0-1001" name="Ignored" fullname="NUnitTests.NUnitTests.Ignored" methodname="Ignored" classname="NUnitTests.NUnitTests" runstate="Ignored" seed="906839282" result="Skipped" label="Ignored" start-time="2016-11-23 09:10:11Z" end-time="2016-11-23 09:10:11Z" duration="0.001314" asserts="0">
                    <properties>
                        <property name="_SKIPREASON" value="Test is unstable" />
                    </properties>
                    <reason>
                        <message><![CDATA[Test is unstable]]></message>
                    </reason>
                </test-case>
            </test-suite>
        </test-suite>
    </test-suite>
</test-run>
END
    my %result = NUnitXmlParser::parse($xml);
    my @testResults = @{ $result{testResults} };

    is(scalar(@testResults), 1);
    my $testResult = $testResults[0];
    is($testResult->getState(), TestResult::IGNORED);
    is($testResult->getTest(), 'NUnitTests.NUnitTests.Ignored');
    is($testResult->getFixture(), undef);
    is($testResult->getTime(), 1);
    is($testResult->getMessage(), undef);
    is($testResult->getStackTrace(), undef);
    is($testResult->getReason(), 'Test is unstable');
}

parse_NUnit35ErrorTest:
{
    my $xml = <<END;
<?xml version="1.0" encoding="utf-8" standalone="no"?>
<test-run id="2" testcasecount="1" result="Failed" total="1" passed="0" failed="1" inconclusive="0" skipped="0" asserts="0" engine-version="3.5.0.0" clr-version="4.0.30319.42000" start-time="2016-11-23 09:16:20Z" end-time="2016-11-23 09:16:20Z" duration="0.074618">
    <command-line><![CDATA[c:\\bin\\nunit\\NUnit-3.5.0\\nunit3-console.exe  3_5_0Tests/bin/Debug/3_5_0Tests.dll --result=results/3_5_0.xml ]]></command-line>
    <test-suite type="Assembly" id="0-1002" name="3_5_0Tests.dll" fullname="C:\\unity\\NUnitTests\\3_5_0Tests\\bin\\Debug\\3_5_0Tests.dll" runstate="Runnable" testcasecount="1" result="Failed" site="Child" start-time="2016-11-23 09:16:20Z" end-time="2016-11-23 09:16:20Z" duration="0.041531" total="1" passed="0" failed="1" inconclusive="0" skipped="0" asserts="0">
        <environment framework-version="3.5.0.0" clr-version="4.0.30319.42000" os-version="Microsoft Windows NT 6.3.9600.0" platform="Win32NT" cwd="C:\\unity\\NUnitTests" machine-name="YAN-PC" user="yan" user-domain="YAN-PC" culture="en-US" uiculture="en-US" os-architecture="x64" />
        <settings>
            <setting name="DisposeRunners" value="True" />
            <setting name="WorkDirectory" value="C:\\unity\\NUnitTests" />
            <setting name="ImageRuntimeVersion" value="4.0.30319" />
            <setting name="ImageTargetFrameworkName" value=".NETFramework,Version=v4.5" />
            <setting name="ImageRequiresX86" value="False" />
            <setting name="ImageRequiresDefaultAppDomainAssemblyResolver" value="False" />
            <setting name="NumberOfTestWorkers" value="8" />
        </settings>
        <properties>
            <property name="_PID" value="5904" />
            <property name="_APPDOMAIN" value="test-domain-" />
        </properties>
        <failure>
            <message><![CDATA[One or more child tests had errors]]></message>
        </failure>
        <test-suite type="TestSuite" id="0-1003" name="NUnitTests" fullname="NUnitTests" runstate="Runnable" testcasecount="1" result="Failed" site="Child" start-time="2016-11-23 09:16:20Z" end-time="2016-11-23 09:16:20Z" duration="0.024242" total="1" passed="0" failed="1" inconclusive="0" skipped="0" asserts="0">
            <failure>
                <message><![CDATA[One or more child tests had errors]]></message>
            </failure>
            <test-suite type="TestFixture" id="0-1000" name="NUnitTests" fullname="NUnitTests.NUnitTests" classname="NUnitTests.NUnitTests" runstate="Runnable" testcasecount="1" result="Failed" site="Child" start-time="2016-11-23 09:16:20Z" end-time="2016-11-23 09:16:20Z" duration="0.018851" total="1" passed="0" failed="1" inconclusive="0" skipped="0" asserts="0">
                <failure>
                    <message><![CDATA[One or more child tests had errors]]></message>
                </failure>
                <test-case id="0-1001" name="ErrorTest" fullname="NUnitTests.NUnitTests.ErrorTest" methodname="ErrorTest" classname="NUnitTests.NUnitTests" runstate="Runnable" seed="816101925" result="Failed" label="Error" start-time="2016-11-23 09:16:20Z" end-time="2016-11-23 09:16:20Z" duration="0.008765" asserts="0">
                    <failure>
                        <message><![CDATA[System.Exception : ERROR!!! interrupted]]></message>
                        <stack-trace><![CDATA[   at NUnitTests.NUnitTests.ErrorTest() in C:\\unity\\NUnitTests\\NUnitTests.cs:line 56]]></stack-trace>
                    </failure>
                </test-case>
            </test-suite>
        </test-suite>
    </test-suite>
</test-run>
END
    my %result = NUnitXmlParser::parse($xml);
    my @testResults = @{ $result{testResults} };

    is(scalar(@testResults), 1);

    my $testResult = $testResults[0];
    is($testResult->getTest(), 'NUnitTests.NUnitTests.ErrorTest');
    is($testResult->getFixture(), undef);
    is($testResult->getState(), TestResult::ERROR);
    is($testResult->getTime(), 8);
    is($testResult->getMessage(), 'System.Exception : ERROR!!! interrupted');
    like($testResult->getStackTrace(), qr/NUnitTests.NUnitTests.ErrorTest\(\)/);
    is($testResult->getReason(), undef);
}

parse_NUnit35SkippedTest:
{
    my $xml = <<END;
<?xml version="1.0" encoding="utf-8" standalone="no"?>
<test-run id="2" testcasecount="1" result="Passed" total="1" passed="0" failed="0" inconclusive="0" skipped="1" asserts="0" engine-version="3.5.0.0" clr-version="4.0.30319.42000" start-time="2016-11-23 09:29:41Z" end-time="2016-11-23 09:29:41Z" duration="0.064844">
    <command-line><![CDATA[c:\\bin\\nunit\\NUnit-3.5.0\\nunit3-console.exe  3_5_0Tests/bin/Debug/3_5_0Tests.dll --result=results/3_5_0.xml ]]></command-line>
    <test-suite type="Assembly" id="0-1002" name="3_5_0Tests.dll" fullname="C:\\unity\\NUnitTests\\3_5_0Tests\\bin\\Debug\\3_5_0Tests.dll" runstate="Runnable" testcasecount="1" result="Passed" start-time="2016-11-23 09:29:41Z" end-time="2016-11-23 09:29:41Z" duration="0.031747" total="1" passed="0" failed="0" inconclusive="0" skipped="1" asserts="0">
        <environment framework-version="3.5.0.0" clr-version="4.0.30319.42000" os-version="Microsoft Windows NT 6.3.9600.0" platform="Win32NT" cwd="C:\\unity\\NUnitTests" machine-name="YAN-PC" user="yan" user-domain="YAN-PC" culture="en-US" uiculture="en-US" os-architecture="x64" />
        <settings>
            <setting name="DisposeRunners" value="True" />
            <setting name="WorkDirectory" value="C:\\unity\\NUnitTests" />
            <setting name="ImageRuntimeVersion" value="4.0.30319" />
            <setting name="ImageTargetFrameworkName" value=".NETFramework,Version=v4.5" />
            <setting name="ImageRequiresX86" value="False" />
            <setting name="ImageRequiresDefaultAppDomainAssemblyResolver" value="False" />
            <setting name="NumberOfTestWorkers" value="8" />
        </settings>
        <properties>
            <property name="_PID" value="24220" />
            <property name="_APPDOMAIN" value="test-domain-" />
        </properties>
        <test-suite type="TestSuite" id="0-1003" name="NUnitTests" fullname="NUnitTests" runstate="Runnable" testcasecount="1" result="Passed" start-time="2016-11-23 09:29:41Z" end-time="2016-11-23 09:29:41Z" duration="0.015451" total="1" passed="0" failed="0" inconclusive="0" skipped="1" asserts="0">
            <test-suite type="TestFixture" id="0-1000" name="NUnitTests" fullname="NUnitTests.NUnitTests" classname="NUnitTests.NUnitTests" runstate="Runnable" testcasecount="1" result="Passed" start-time="2016-11-23 09:29:41Z" end-time="2016-11-23 09:29:41Z" duration="0.010261" total="1" passed="0" failed="0" inconclusive="0" skipped="1" asserts="0">
                <test-case id="0-1001" name="Skipped" fullname="NUnitTests.NUnitTests.Skipped" methodname="Skipped" classname="NUnitTests.NUnitTests" runstate="Skipped" seed="1144254909" result="Skipped" start-time="2016-11-23 09:29:41Z" end-time="2016-11-23 09:29:41Z" duration="0.001272" asserts="0">
                    <properties>
                        <property name="_SKIPREASON" value="Only supported on Linux" />
                    </properties>
                    <reason>
                        <message><![CDATA[Only supported on Linux]]></message>
                    </reason>
                </test-case>
            </test-suite>
        </test-suite>
    </test-suite>
</test-run>
END
    my %result = NUnitXmlParser::parse($xml);
    my @testResults = @{ $result{testResults} };
    is(scalar(@testResults), 1);
    my $testResult = $testResults[0];
    is($testResult->getTest(), 'NUnitTests.NUnitTests.Skipped');
    is($testResult->getFixture(), undef);
    is($testResult->getState(), TestResult::SKIPPED);
    is($testResult->getTime(), 1);
    is($testResult->getMessage(), undef);
    is($testResult->getStackTrace(), undef);
    is($testResult->getReason(), 'Only supported on Linux');
}

parse_NUnit35NotRunnableTest:
{
    my $xml = <<END;
<?xml version="1.0" encoding="utf-8" standalone="no"?>
<test-run id="2" testcasecount="1" result="Failed" total="1" passed="0" failed="1" inconclusive="0" skipped="0" asserts="0" engine-version="3.5.0.0" clr-version="4.0.30319.42000" start-time="2016-11-23 09:57:23Z" end-time="2016-11-23 09:57:23Z" duration="0.068630">
    <command-line><![CDATA[c:\\bin\\nunit\\NUnit-3.5.0\\nunit3-console.exe  3_5_0Tests/bin/Debug/3_5_0Tests.dll --result=results/3_5_0.xml ]]></command-line>
    <test-suite type="Assembly" id="0-1002" name="3_5_0Tests.dll" fullname="C:\\unity\\NUnitTests\\3_5_0Tests\\bin\\Debug\\3_5_0Tests.dll" runstate="Runnable" testcasecount="1" result="Failed" site="Child" start-time="2016-11-23 09:57:23Z" end-time="2016-11-23 09:57:23Z" duration="0.036671" total="1" passed="0" failed="1" inconclusive="0" skipped="0" asserts="0">
        <environment framework-version="3.5.0.0" clr-version="4.0.30319.42000" os-version="Microsoft Windows NT 6.3.9600.0" platform="Win32NT" cwd="C:\\unity\\NUnitTests" machine-name="YAN-PC" user="yan" user-domain="YAN-PC" culture="en-US" uiculture="en-US" os-architecture="x64" />
        <settings>
            <setting name="DisposeRunners" value="True" />
            <setting name="WorkDirectory" value="C:\\unity\\NUnitTests" />
            <setting name="ImageRuntimeVersion" value="4.0.30319" />
            <setting name="ImageTargetFrameworkName" value=".NETFramework,Version=v4.5" />
            <setting name="ImageRequiresX86" value="False" />
            <setting name="ImageRequiresDefaultAppDomainAssemblyResolver" value="False" />
            <setting name="NumberOfTestWorkers" value="8" />
        </settings>
        <properties>
            <property name="_PID" value="6948" />
            <property name="_APPDOMAIN" value="test-domain-" />
        </properties>
        <failure>
            <message><![CDATA[One or more child tests had errors]]></message>
        </failure>
        <test-suite type="TestSuite" id="0-1003" name="NUnitTests" fullname="NUnitTests" runstate="Runnable" testcasecount="1" result="Failed" site="Child" start-time="2016-11-23 09:57:23Z" end-time="2016-11-23 09:57:23Z" duration="0.019583" total="1" passed="0" failed="1" inconclusive="0" skipped="0" asserts="0">
            <failure>
                <message><![CDATA[One or more child tests had errors]]></message>
            </failure>
            <test-suite type="TestFixture" id="0-1000" name="NotRunnableTestFixture" fullname="NUnitTests.NotRunnableTestFixture" classname="NUnitTests.NotRunnableTestFixture" runstate="NotRunnable" testcasecount="1" result="Failed" label="Invalid" site="SetUp" start-time="2016-11-23 09:57:23Z" end-time="2016-11-23 09:57:23Z" duration="0.011878" total="1" passed="0" failed="1" inconclusive="0" skipped="0" asserts="0">
                <properties>
                    <property name="_SKIPREASON" value="No suitable constructor was found" />
                </properties>
                <failure>
                    <message><![CDATA[No suitable constructor was found]]></message>
                </failure>
                <test-case id="0-1001" name="Test1" fullname="NUnitTests.NotRunnableTestFixture.Test1" methodname="Test1" classname="NUnitTests.NotRunnableTestFixture" runstate="Runnable" seed="1741877268" result="Failed" label="Invalid" site="Parent" start-time="0001-01-01 00:00:00Z" end-time="0001-01-01 00:00:00Z" duration="0.000000" asserts="0">
                    <failure>
                        <message><![CDATA[OneTimeSetUp: No suitable constructor was found]]></message>
                    </failure>
                </test-case>
            </test-suite>
        </test-suite>
    </test-suite>
</test-run>
END
    my %result = NUnitXmlParser::parse($xml);
    my @testResults = @{ $result{testResults} };
    is(scalar(@testResults), 1);
    my $testResult = $testResults[0];
    is($testResult->getTest(), 'NUnitTests.NotRunnableTestFixture.Test1');
    is($testResult->getFixture(), undef);
    is($testResult->getState(), TestResult::NOTRUNNABLE);
    is($testResult->getTime(), 0);
    is($testResult->getMessage(), 'OneTimeSetUp: No suitable constructor was found');
    is($testResult->getStackTrace(), undef);
    is($testResult->getReason(), undef);
}

parse_NUnit26NotRunnableTest:
{
    my $xml = <<END;
<?xml version="1.0" encoding="utf-8" standalone="no"?>
<!--This file represents the results of running a test suite-->
<test-results name="C:\\unity\\NUnitTests\\2_6_4Tests\\bin\\Debug\\2_6_4Tests.dll" total="0" errors="0" failures="0" not-run="1" inconclusive="0" ignored="0" skipped="0" invalid="1" date="2016-11-23" time="11:14:19">
    <environment nunit-version="2.6.4.14350" clr-version="2.0.50727.8009" os-version="Microsoft Windows NT 6.2.9200.0" platform="Win32NT" cwd="C:\\unity\\NUnitTests" machine-name="YAN-PC" user="yan" user-domain="YAN-PC" />
    <culture-info current-culture="en-US" current-uiculture="en-US" />
    <test-suite type="Assembly" name="C:\\unity\\NUnitTests\\2_6_4Tests\\bin\\Debug\\2_6_4Tests.dll" executed="True" result="Failure" success="False" time="0.024" asserts="0">
        <results>
            <test-suite type="Namespace" name="NUnitTests" executed="True" result="Failure" success="False" time="0.011" asserts="0">
                <results>
                    <test-suite type="TestFixture" name="NotRunnableTestFixture" executed="False" result="NotRunnable">
                        <reason>
                            <message><![CDATA[No suitable constructor was found]]></message>
                        </reason>
                        <results>
                            <test-case name="NUnitTests.NotRunnableTestFixture.Test1" executed="False" result="NotRunnable">
                                <reason>
                                    <message><![CDATA[No suitable constructor was found]]></message>
                                </reason>
                            </test-case>
                        </results>
                    </test-suite>
                </results>
            </test-suite>
        </results>
    </test-suite>
</test-results>
END
    my %result = NUnitXmlParser::parse($xml);
    my @testResults = @{ $result{testResults} };
    is(scalar(@testResults), 1);
    my $testResult = $testResults[0];
    is($testResult->getTest(), 'NUnitTests.NotRunnableTestFixture.Test1');
    is($testResult->getFixture(), undef);
    is($testResult->getState(), TestResult::NOTRUNNABLE);
    is($testResult->getTime(), 0);
    is($testResult->getMessage(), undef);
    is($testResult->getStackTrace(), undef);
    is($testResult->getReason(), 'No suitable constructor was found');
}

parse_AllInOneRunnerXml:
{
    my $xml = <<END;
<?xml version="1.0" encoding="utf-8" standalone="yes"?>
<test-results time="11:44 AM" date="11/23/2016" name="UNNAMED" invalid="0" skipped="0" ignored="0" total="1" failures="0" errors="0" inconclusive="0" not-run="0">
    <environment nunit-version="AllIn1Runner" machine-name="" platform="Editor" os-version="" clr-version="" user="" user-domain="" cwd="" />
    <culture-info current-uiculture="" current-culture="" />
    <test-suite name="RuntimeTests" result="" type="" executed="" success="true" time="14">
        <results>
            <test-case name="CanAddComponentByType" result="Success" executed="true" time="0.014" success="true" />
        </results>
    </test-suite>
</test-results>
END
    my %result = NUnitXmlParser::parse($xml);
    my @testResults = @{ $result{testResults} };
    is(scalar(@testResults), 1);
    my $testResult = $testResults[0];
    is($testResult->getTest(), 'CanAddComponentByType');
    is($testResult->getFixture(), undef);
    is($testResult->getState(), TestResult::SUCCESS);
    is($testResult->getTime(), 14);
    is($testResult->getMessage(), undef);
    is($testResult->getStackTrace(), undef);
    is($testResult->getReason(), undef);
}

parse_EditorTestsXml:
{
    my $xml = <<END;
<?xml version="1.0" encoding="utf-8"?>
<test-suite type="TestSuite" id="1000" name="SpriteEditorWindow" fullname="SpriteEditorWindow" runstate="Runnable" testcasecount="3" result="Passed" start-time="2016-11-23 14:42:17Z" end-time="2016-11-23 14:42:20Z" duration="3.225178" total="3" passed="3" failed="0" inconclusive="0" skipped="0" asserts="0">
    <test-suite type="Assembly" id="1005" name="Assembly-CSharp-Editor-testable.dll" fullname="C:/unity/lane4/Tests/EditorTests/2D/SpriteEditorWindow/Library/ScriptAssemblies/Assembly-CSharp-Editor-testable.dll" runstate="Runnable" testcasecount="3" result="Passed" start-time="2016-11-23 14:42:17Z" end-time="2016-11-23 14:42:20Z" duration="3.189712" total="3" passed="3" failed="0" inconclusive="0" skipped="0" asserts="0">
        <properties>
            <property name="_PID" value="13832" />
            <property name="_APPDOMAIN" value="Unity Child Domain" />
        </properties>
        <test-suite type="TestFixture" id="1001" name="SpriteEditorSelectionTest" fullname="SpriteEditorSelectionTest" classname="SpriteEditorSelectionTest" runstate="Runnable" testcasecount="3" result="Passed" start-time="2016-11-23 14:42:17Z" end-time="2016-11-23 14:42:20Z" duration="3.174123" total="3" passed="3" failed="0" inconclusive="0" skipped="0" asserts="0">
            <test-case id="1003" name="SelectedSpriteRectNameSameAsSelection" fullname="SpriteEditorSelectionTest.SelectedSpriteRectNameSameAsSelection" methodname="SelectedSpriteRectNameSameAsSelection" classname="SpriteEditorSelectionTest" runstate="Runnable" seed="1540731237" result="Passed" start-time="2016-11-23 14:42:17Z" end-time="2016-11-23 14:42:20Z" duration="2.938746" asserts="0" />
            <test-case id="1002" name="SpriteEditorWindowShowsCorrectSelectedTexture" fullname="SpriteEditorSelectionTest.SpriteEditorWindowShowsCorrectSelectedTexture" methodname="SpriteEditorWindowShowsCorrectSelectedTexture" classname="SpriteEditorSelectionTest" runstate="Runnable" seed="1435246645" result="Passed" start-time="2016-11-23 14:42:20Z" end-time="2016-11-23 14:42:20Z" duration="0.104315" asserts="0" />
            <test-case id="1004" name="SpriteNameChangePersistAfterSpriteSelectionInSameTexture" fullname="SpriteEditorSelectionTest.SpriteNameChangePersistAfterSpriteSelectionInSameTexture" methodname="SpriteNameChangePersistAfterSpriteSelectionInSameTexture" classname="SpriteEditorSelectionTest" runstate="Runnable" seed="214434665" result="Passed" start-time="2016-11-23 14:42:20Z" end-time="2016-11-23 14:42:20Z" duration="0.094579" asserts="0" />
        </test-suite>
    </test-suite>
</test-suite>
END
    my %result = NUnitXmlParser::parse($xml);
    my @testResults = @{ $result{testResults} };
    is(scalar(@testResults), 3);
}

parse_AssetImportTestResultsXml:
{
    my $xml = <<END;
<?xml version="1.0" encoding="utf-8" standalone="no"?>
<test-results time="5:01 PM" date="11/23/2016" name="UNNAMED" invalid="0" skipped="0" ignored="100" total="290" failures="172" errors="0" inconclusive="0" not-run="100">
    <environment nunit-version="AssetImport" machine-name="" platform="" os-version="" clr-version="" user="" user-domain="" cwd="" />
    <culture-info current-uiculture="" current-culture="" />
    <test-suite name="Model Import Test" result="" type="" executed="True" success="False" time="51.090">
        <results>
            <test-case name="Meshes/3DS/Uncategorized/hammerhead/hamhead.3ds" result="Success" executed="True" success="True" time="3.612">
            </test-case>
        </results>
    </test-suite>
</test-results>
END
    my %result = NUnitXmlParser::parse($xml);
    my @testResults = @{ $result{testResults} };
    is(scalar(@testResults), 1);
    my $testResult = $testResults[0];
    is($testResult->getTest(), 'Meshes/3DS/Uncategorized/hammerhead/hamhead.3ds');
    is($testResult->getFixture(), undef);
    is($testResult->getState(), TestResult::SUCCESS);
    is($testResult->getTime(), 3612);
    is($testResult->getMessage(), undef);
    is($testResult->getStackTrace(), undef);
    is($testResult->getReason(), undef);
}

done_testing();
