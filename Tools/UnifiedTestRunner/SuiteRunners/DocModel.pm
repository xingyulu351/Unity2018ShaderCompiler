package SuiteRunners::DocModel;

use parent NUnitConfigBasedSuiteRunner;

use warnings;
use strict;

use File::Spec::Functions qw (canonpath);

use Environments::Win;
use Dirs;

my $root = Dirs::getRoot();
my $proj1 = canonpath("$root/Tools/DocTools/UnderlyingModel.Tests/UnderlyingModel.Tests.csproj");
my $proj2 = canonpath("$root/Tools/DocTools/ScriptRefBase.Tests/ScriptRefBase.Tests.csproj");
my $proj3 = canonpath("$root/Tools/DocTools/XmlFormatter.Tests/XmlFormatter.Tests.csproj");

sub getConfig
{
    my ($this) = @_;
    my $config = {
        name => 'docmodel',
        prepareEnv => sub { $this->jam('DocGen') },
        targetFrameworkVersion => '4.5',
        nunitInputFiles => [$proj1, $proj2, $proj3],
        environment => new Environments::Win(),
        help => {
            description => "Documenation generation tests hosted in $proj1, $proj2, $proj3",
            examples => [
                { description => "Run all tests matching 'Member'", command => '--testfilter=Member' },
                { description => "List all matching 'Member'", command => '--list --testfilter=Member' }
            ]
        }
    };
}

1;
