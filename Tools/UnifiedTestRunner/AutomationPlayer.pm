package AutomationPlayer;

use warnings;
use strict;

use File::Spec::Functions qw (canonpath catfile);
use FileUtils qw (readAllLines);

use Dirs;
use lib Dirs::external_root();
use JSON;

sub resolve
{
    my ($platform) = @_;
    $platform = lc($platform);
    my $config = _readConfig();
    if (not defined($config->{$platform}))
    {
        return ();
    }
    my @projects = @{ $config->{$platform} };
    my $root = Dirs::getRoot();
    my @pathes = map { canonpath("$root/$_") } @projects;
    return @pathes;
}

sub _readConfig
{
    my $utrRoot = Dirs::UTR_root();
    my $configPath = canonpath(catfile($utrRoot, 'Configs', 'Automation.Players.json'));
    my $text = join(q (), readAllLines($configPath));
    return jsonToObj($text);
}

1;
