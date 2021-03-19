package SuiteRunners::Framework;

use parent NUnitConfigBasedSuiteRunner;

use warnings;
use strict;
use Dirs;
use CSProj;
use File::Spec::Functions qw (canonpath);

my $root = Dirs::getRoot();
my @projects = (
    canonpath("$root/Tests/UnityEngine.Common.Tests/UnityEngine.Common.Tests.csproj"),
    canonpath("$root/Tests/Unity.PerformanceTests.Framework.UnitTests/Unity.PerformanceTests.Framework.UnitTests.csproj"),
    canonpath("$root/Tests/Unity.Artifacts.Tests/Unity.Automation.Artifacts.Tests.csproj"),
    canonpath("$root/Tests/Unity.TestProtocol.Tests/Unity.TestProtocol.Tests.csproj"),
    canonpath("$root/Tests/Unity.TestResultXmlWriterTests/Unity.TestResultXmlWriterTests.csproj"),
    canonpath("$root/Tests/Unity.GraphicsTestsRunner.Tests/Unity.GraphicsTestsRunner.Tests.csproj"),
    canonpath("$root/Tests/Unity.NativeTestsRunner.Tests/Unity.NativeTestsRunner.Tests.csproj"),
    canonpath("$root/Tests/Unity.PerformanceTests.Runner.Tests/Unity.PerformanceTests.Runner.Tests.csproj"),
    canonpath("$root/Tests/Unity.RuntimeTestsRunner.Tests/Unity.RuntimeTestsRunner.Tests.csproj"),
    canonpath("$root/Tools/Unity.CommonTools.JsonWrapper.Tests/Unity.CommonTools.JsonWrapper.Tests.csproj"),
    canonpath("$root/Tools/Unity.CommonTools.ZipWrapper.Tests/Unity.CommonTools.ZipWrapper.Tests.csproj"),
    canonpath("$root/Tests/Mono.Options.Tests/Mono.Options.Tests.csproj"),
    canonpath("$root/Tests/Unity.Automation.Logger.Tests/Unity.Automation.Logger.Tests.csproj"),
    canonpath("$root/Tests/Unity.TestRunner.Common.Tests/Unity.TestRunner.Common.Tests.csproj"),
    canonpath("$root/Tests/Unity.EditorTestsRunner.Tests/Unity.EditorTestsRunner.Tests.csproj"),
    canonpath("$root/Tests/Unity.UnifiedTestRunner.Tests/Unity.UnifiedTestRunner.Tests.csproj"),
    canonpath("$root/Tests/Unity.IntegrationTestsRunner.Tests/Unity.IntegrationTestsRunner.Tests.csproj"),
    canonpath("$root/Tests/Unity.ApiUpdaterTestsRunner.Tests/Unity.ApiUpdaterTestsRunner.Tests.csproj"),
    canonpath("$root/Tests/Unity.BuildSystemTestsRunner.Tests/Unity.BuildSystemTestsRunner.Tests.csproj"),
    canonpath("$root/Tests/Unity.FrameworkTestsRunner.Tests/Unity.FrameworkTestsRunner.Tests.csproj"),
    canonpath("$root/Tests/Unity.PlaymodeTestsRunner/Unity.PlaymodeTestsRunner.csproj"),
    canonpath("$root/Tests/Unity.UnityTestFramework.PluginBase.Tests/Unity.UnityTestFramework.PluginBase.Tests.csproj"),
    canonpath("$root/Tests/Unity.TestRunner.Common.Nunit.Tests/Unity.TestRunner.Common.Nunit.Tests.csproj"),
);

sub getConfig
{
    my ($this) = @_;
    my $config = {
        name => 'framework',
        nunitInputFiles => \@projects,
        environment => new Environments::Mono(),
        help => {
            description => "Runs all the unittests for the test runners and libraries",
            prerequisites => "perl build.pl --prepare",
            examples => [
                { description => "Run all tests matching 'Options'", command => '--testfilter=Options' },
                { description => "List all tests matching 'Options'", command => '--list --testfilter=Options' },
            ],
        }
    };

    return $config;
}

1;
