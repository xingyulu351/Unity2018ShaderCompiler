package PlatformTarget;

use warnings;
use strict;

use File::Basename qw (basename);
use Dirs;
use FileWatcher;
use lib Dirs::external_root();
use Switch;
use Logger;

my $buildTargetInfo = undef;

sub new
{
    my ($pkg, $modulePath, $platform) = @_;

    my $buildTarget = _loadBuildTarget($modulePath);
    $buildTargetInfo = undef;
    my $instance = {
        prepareFunc => $buildTarget->{prepareFunc},
        disposeFunc => $buildTarget->{disposeFunc},
        getTestSettingFunc => $buildTarget->{getTestSettingFunc},
        name => $buildTarget->{name},
        platform => $platform,
        module => $modulePath,
    };
    return bless($instance, $pkg);
}

sub prepare
{
    my ($this, $options) = @_;
    my $root = Dirs::getRoot();
    if ($this->{prepareFunc})
    {
        $this->{prepareFunc}->($root, undef, $options);
    }
}

sub dispose
{
    my ($this, $options) = @_;
    my $root = Dirs::getRoot();
    if ($this->{disposeFunc})
    {
        $this->{disposeFunc}->($root, undef, $options);
    }
}

sub getTestSettings
{
    my ($this, $options) = @_;
    my $root = Dirs::getRoot();
    if ($this->{getTestSettingFunc})
    {
        return $this->{getTestSettingFunc}->($root, undef, $options);
    }
    return undef;
}

sub _loadBuildTarget
{
    my ($modulePath) = @_;
    return _loadPlatformTargetPlugin($modulePath);
}

sub getName
{
    my ($this) = @_;
    return $this->{name};
}

sub getPlatform
{
    my ($this) = @_;
    return $this->{platform};
}

sub getModule
{
    my ($this) = @_;
    return $this->{module};
}

sub _loadPlatformTargetPlugin
{
    my ($modulePath) = @_;
    my $module = basename($modulePath);
    require $modulePath;
    $module =~ s/\.[^.]+$//;
    $module->Register();
    return $buildTargetInfo;
}

sub _registerTarget
{
    my ($name, $data) = @_;
    if (defined($buildTargetInfo))
    {
        return;
    }
    my $prepareFunc = _getPrepareFunc($data);

    # do not register targets which are not relevant for the current platform
    if (not $prepareFunc)
    {
        return;
    }

    $buildTargetInfo = {};
    $buildTargetInfo->{name} = $name;
    $buildTargetInfo->{prepareFunc} = $prepareFunc;
    $buildTargetInfo->{disposeFunc} = _getDisposeFunc($data);
    $buildTargetInfo->{getTestSettingFunc} = _getGetTestSettingsFunc($data);
}

sub _getPrepareFunc
{
    my ($data) = @_;
    switch ($^O)
    {
        case 'MSWin32' { return $data->{windowspreparefunction}; }
        case 'darwin' { return $data->{macpreparefunction}; }
        case 'linux' { return $data->{linuxpreparefunction}; }
        else { return undef; }
    }
}

sub _getDisposeFunc
{
    my ($data) = @_;
    return $data->{disposefunction};
}

sub _getGetTestSettingsFunc
{
    my ($data) = @_;
    return $data->{testsettingfunction};
}

package Main;

sub RegisterTarget
{
    PlatformTarget::_registerTarget(@_);
}

sub ProgressMessage
{
    my ($message) = @_;
    Logger::verbose($message);
}

1;
