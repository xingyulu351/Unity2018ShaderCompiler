package SuiteRunners::ApiUpdater;

use parent NUnitConfigBasedSuiteRunner;

use warnings;
use strict;
use File::Spec::Functions qw (canonpath);
use Dirs;
use CSProj;
use Environments::Win;

my $root = Dirs::getRoot();
my $testProj1 = canonpath("$root/Editor/Tools/ScriptUpdater/IntegrationTests/IntegrationTests.csproj");
my $slnFile = canonpath("$root/Editor/Tools/ScriptUpdater/ScriptUpdater.sln");

sub getConfig
{
    my ($this) = @_;
    my $config = {
        name => 'apiupdater',
        prepareEnv => sub { $this->jam('ScriptUpdater') },
        CSProjectToCleanup => [$slnFile],
        nunitInputFiles => [$testProj1],
        targetFrameworkVersion => '4.5',
        environment => new Environments::Win(),
        help => {
            description => "Script API updater integration tests hosted in $testProj1",
            examples => [
                { description => "Run all tests matching 'Members'", command => '--testfilter=Members' },
                { description => "List all matching 'Members'", command => '--list --testfilter=Members' }
            ]
        }
    };
}

1;
