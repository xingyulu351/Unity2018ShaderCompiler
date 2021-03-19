use warnings;
use strict;

use FindBin qw ($Bin);
use lib "$Bin";

use File::Spec::Functions qw (catdir);
use Carp qw (croak);
use Dirs;

sub getExecutable
{
    my ($module) = @_;
    my $root = Dirs::getRoot();
    my $nullfh;

    @INC = (@INC, catdir($root, "Tools/Build"), catdir($root, "Tools/Build/TargetBuildRecipes"), catdir($root, "External/Perl/lib"));
    require "$module.pm";
    my $regData = $module->GetRegistrationData();
    my $executable = $regData->{executable};
    return $executable;
}

if (scalar(@ARGV) == 0)
{
    croak('module name expected');
}

my $executable;
{    #silent scope
    local *STDOUT;
    local *STDERR;
    open(STDOUT, '>', File::Spec->devnull());
    open(STDERR, '>', File::Spec->devnull());
    $executable = getExecutable($ARGV[0]);
}

print($executable);
