package SuiteRunners::PS4Il2CppPlugin;

use parent NUnitConfigBasedSuiteRunner;

use File::Spec::Functions qw (catdir);

use warnings;
use strict;

use Dirs;

my $proj = catdir(Dirs::getRoot(), 'PlatformDependent/PS4/Tools/il2cpp/PS4Il2CppPlugin.sln');

sub getConfig
{
    my ($this) = @_;
    my $config = {
        name => 'PS4Il2CppPlugin',
        prepareEnv => sub
        {
            $this->jam('PS4Il2CppPlugin');
        },
        nunitInputFiles => [$proj],
        targetFrameworkVersion => '4.6.1',
        environment => new Environments::Win(),
        help => {
            description => "PS4 IL2CPP plugin tests from the $proj",
            examples => [
                { description => "Run all tests matching 'Member'", command => '--testfilter=Member' },
                { description => "List all matching 'Member'", command => '--list --testfilter=Member' }
            ]
        }
    };
}

1;
