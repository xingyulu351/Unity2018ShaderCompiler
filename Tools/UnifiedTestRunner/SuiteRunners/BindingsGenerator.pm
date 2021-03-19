package SuiteRunners::BindingsGenerator;

use parent NUnitConfigBasedSuiteRunner;

use warnings;
use strict;
use Dirs;
use CSProj;
use File::Spec::Functions qw (canonpath);

my $root = Dirs::getRoot();
my $slnFile = canonpath("$root/Tools/BindingsGenerator/BindingsGenerator.sln");
my $proj1 = canonpath("$root/Tools/BindingsGenerator/Unity.BindingsGenerator.TestFramework/Unity.BindingsGenerator.TestFramework.csproj");
my $proj2 = canonpath("$root/Tools/BindingsGenerator/Unity.BindingsGenerator.Tests/Unity.BindingsGenerator.Tests.csproj");

sub getConfig
{
    my ($this) = @_;
    my $config = {
        name => 'bindingsgenerator',
        prepareEnv => sub { $this->jam('BindingsGenerator') },
        csProjectToCleanup => [$slnFile],
        csDependencies => [$slnFile],
        nunitInputFiles => [$proj1, $proj2],
        environment => new Environments::MonoBleedingEdge(),
        monoArgs => ['--runtime=v4.0'],
        help => {
            description => "Bindings generator tests hosted in\n$proj1\n$proj2",
            examples => [
                { description => "Run all tests matching 'StaticMethod'", command => '--testfilter=StaticMethod' },
                { description => "List all tests matching 'StaticMethod'", command => '--list --testfilter=StaticMethod' },
            ],
        }
    };

    return $config;
}

1;
