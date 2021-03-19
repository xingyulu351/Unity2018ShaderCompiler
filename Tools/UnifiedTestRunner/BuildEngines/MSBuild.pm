package BuildEngines::MSBuild;

use parent Builder;

use warnings;
use strict;

use Carp qw (croak);
use File::Spec::Functions qw (canonpath catdir);

use Logger;
use FileUtils;
use Dirs;

sub new
{
    my ($pkg) = @_;
    my $instance = $pkg->SUPER::new();

    $instance->{msbuildPathProvider} = sub
    {
        my $this = shift();
        return $this->getMSBuildPath(@_);
    };

    return bless($instance, $pkg);
}

sub getName
{
    return 'msbuild';
}

sub setMSBuildPathProvider
{
    my ($this, $value) = @_;
    $this->{msbuildPathProvider} = $value;
}

sub build
{
    my ($this, %args) = @_;

    my $project = canonpath($args{'project'});
    Logger::minimal("Building $project");

    my $framework = $args{framework};
    my $msbuild_path = $this->{msbuildPathProvider}($this, $framework);

    my @callArgs = ();

    push(@callArgs, '"' . canonpath($msbuild_path . 'MSBuild.exe') . '"');

    push(@callArgs, $project);
    my $rebuild = $args{'rebuild'};
    if ($rebuild)
    {
        push(@callArgs, "/target:Rebuild");
    }
    else
    {
        push(@callArgs, "/target:Build");
    }
    my $config = $args{'config'};
    push(@callArgs, "/property:Configuration=$config");
    if ($args{'platform'})
    {
        push(@callArgs, "/property:Platform=$args{'platform'}");
    }
    push(@callArgs, "/verbosity:quiet");
    push(@callArgs, "/nologo");

    my $outputPath = $args{'outputPath'};
    if (defined($outputPath))
    {
        push(@callArgs, "/property:OutputPath=$outputPath");
        my $intermPath = canonpath("$outputPath/Intermediate") . canonpath("/");
        push(@callArgs, "/property:BaseIntermediateOutputPath=$intermPath");
    }

    if (defined($framework))
    {
        push(@callArgs, "/property:TargetFrameworkVersion=$framework");
    }

    my $projectArgs = $args{'project_args'};
    if (defined($projectArgs))
    {
        push(@callArgs, $projectArgs);
    }

    my $logfile = $args{'logfile'};
    my $filesToWatch;
    if ($logfile)
    {
        $filesToWatch = [{ file => $logfile, callback => \&Plugin::printLogMessage, callbackArg => $this }];
        push(@callArgs, "1>$logfile");
    }

    my $exitCode = $this->executeAndWatchFiles(command => \@callArgs, filesToWatch => $filesToWatch);
    if ($exitCode != 0)
    {
        $this->dumpBuildLogAndCroak($project, $logfile);
    }
}

sub clean
{
    my ($this, $project) = @_;
    Logger::minimal("Cleaning $project");
    my $msbuild_path = $this->{msbuildPathProvider}($this);
    my @callArgs = ();

    push(@callArgs, '"' . canonpath($msbuild_path . 'MSBuild.exe') . '"');
    push(@callArgs, canonpath($project));
    push(@callArgs, "/target:Clean");
    push(@callArgs, "/noconsolelogger");
    push(@callArgs, "/nologo");

    return $this->executeAndWatchFiles(command => \@callArgs, filesToWatch => []);
}

sub getMSBuildPathInVSInstallation
{
    my $msbuild_path_in_external = catdir(Dirs::getRoot(), "External/Microsoft/MSBuild15/builds");
    return $msbuild_path_in_external . "/Windows_NT_Deployment/";
}

sub getMSBuildPath
{
    my ($this, $framework) = @_;

    my $msbuild_path_vs = $this->getMSBuildPathInVSInstallation();
    if ($msbuild_path_vs ne '')
    {
        return $msbuild_path_vs;
    }

    $framework = '4.0' if (not defined($framework));

    # convert framework to a 3 digit version number (e.g. 4.5.1 to 451)
    my $framework_version = _getMSBuildFrameworkVersion($framework);

    # find the oldest msbuild that can handle the framework
    my @tools_versions;
    my $default_tools_version;
    if ($framework_version <= 462)
    {
        $default_tools_version = '14.0';
        unshift @tools_versions, $default_tools_version;
    }
    for my $tools_version (@tools_versions)
    {
        my $msbuild_path = $this->_getMSBuildPath($tools_version);
        if ($msbuild_path ne '')
        {
            if ($tools_version ne $default_tools_version)
            {
                Logger::minimal("MSBuild tools version $default_tools_version for .NET Framework $framework not found, falling back to $tools_version.\n");
            }
            else
            {
                Logger::verbose("MSBuild tools version $tools_version found for .NET Framework $framework.");
            }
            return $msbuild_path;
        }
    }
    croak("MSBuild tools version $default_tools_version for .NET Framework $framework not found");
}

sub _getMSBuildFrameworkVersion
{
    my ($framework) = @_;
    my $version = 0;
    my @parts = split(/\./, $framework);
    for (my $i = 0; $i < 3; ++$i)
    {
        $version *= 10;
        my $part = $parts[$i];
        if (defined($part))
        {
            $version += $part;
        }
    }
    return $version;
}

sub _getMSBuildPath
{
    my ($this, $version) = @_;
    my $regKey = "\"HKLM\\SOFTWARE\\Microsoft\\MSBuild\\ToolsVersions\\$version\"";
    my ($exit_code, @lines) = $this->callSystemAndGetStdOut('reg.exe', 'query', $regKey, '/v', 'MSBuildToolsPath');
    if ($exit_code == 0)
    {
        my $queryRes = $lines[0];
        my ($msbuild_path) = $queryRes =~ m/REG_SZ\s+(.*)/i;
        return $msbuild_path if (defined($msbuild_path));
    }
    return '';
}

1;
