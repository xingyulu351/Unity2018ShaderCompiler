package BuildTargetResolver;

use warnings;
use strict;

use Dirs;
use FileUtils qw (readAllLines);
use File::Spec::Functions qw (canonpath catfile);
use Carp qw (croak);
use lib Dirs::external_root();
use JSON;
use Dirs;
use Logger;
use PlatformTarget;

sub resolveTarget
{
    my ($platform) = @_;
    my $module = resolveModule($platform);
    if (not defined($module))
    {
        return undef;
    }
    return new PlatformTarget($module, $platform);
}

sub resolveModule
{
    my ($platform) = @_;

    $platform = lc($platform);
    my $config = _readConfig();

    my $module = undef;
    if ($config->{$platform})
    {
        $module = _resolve($platform, @{ $config->{$platform} });
        return $module;
    }

    return _resolve($platform, @{ $config->{search_patterns} });
}

sub _resolve
{
    my ($platform, @patterns) = @_;
    my @modules = _resolveModules(@patterns);
    if (scalar(@modules) == 1 && -e $modules[0])
    {
        return $modules[0];
    }
    return _matchPlatform($platform, @modules);
}

sub _readConfig
{
    my $utrRoot = Dirs::UTR_root();
    my $configPath = canonpath(catfile($utrRoot, 'Configs', 'BuildTargets.json'));
    my $text = join(q (), readAllLines($configPath));
    return jsonToObj($text);
}

sub _resolveModules
{
    my (@search_patterns) = @_;
    my $root = Dirs::getRoot();
    my @patterns = map { canonpath("$root/$_") } @search_patterns;
    my @modules = glob(join(' ', @patterns));
    return @modules;
}

sub _matchPlatform
{
    my ($platform, @modules) = @_;
    foreach my $m (@modules)
    {
        if ($m =~ /$platform.*/i)
        {
            Logger::minimal("Resolved build target for $platform --> $m");
            return $m;
        }
    }
    return;
}

1;
