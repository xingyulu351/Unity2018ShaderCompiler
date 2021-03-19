package CSProj;

use warnings;
use strict;

use Dirs;
use lib Dirs::external_root();

use XML::Simple;

use File::Spec;
use File::Basename;
use Carp qw (croak);
use File::Spec::Functions qw (canonpath catfile file_name_is_absolute );
use GUIDUtils;

use constant {
    PROJECT_TYPE_UNKNOWN => 0,
    PROJECT_TYPE_CS_CLASS_LIBRARY => 1,
    PROJECT_TYPE_CS_EXE => 2,
};

sub new
{
    my ($pkg, $project, $guid) = @_;

    my $xmlParser = new XML::Simple();
    my $nodes = $xmlParser->XMLin($project);

    my $instance = {
        project => canonpath($project),
        guid => $guid,
        xml => $xmlParser->XMLin($project)
    };
    return bless($instance, $pkg);
}

sub getGUID
{
    my ($this) = @_;
    if ($this->{guid})
    {
        return $this->{guid};
    }
    for my $pg (@{ $this->{xml}->{PropertyGroup} })
    {
        my $projectGuid = $pg->{ProjectGuid};
        if (defined($projectGuid))
        {
            $this->{guid} = GUIDUtils::extractGUID($projectGuid);
            last;
        }
    }
    return $this->{guid};
}

sub getProjectPath
{
    my ($this) = @_;
    return $this->{project};
}

sub setSolutionFolder
{
    my ($this, $value) = @_;
    $this->{solutionFolder} = $value;
}

sub getSolutionFolder
{
    my ($this) = @_;
    return $this->{solutionFolder};
}

sub getAssemblyFullPath
{
    my ($this, $config) = @_;

    if (not defined($config))
    {
        $config = 'Debug|AnyCPU';
    }

    my $cachedValueKey = "AssemblyFullPath$config";
    if (defined($this->{$cachedValueKey}))
    {
        return $this->{$cachedValueKey};
    }

    my $outputPath = $this->getOutputPath($config);
    if (defined($outputPath))
    {
        my $assemblyNameWithExtention = $this->getAssemblyNameWithExtenstion();
        my $projectPath = dirname($this->{project});
        my $path = File::Spec->catfile($projectPath, $outputPath, $assemblyNameWithExtention);
        $this->{$cachedValueKey} = canonpath($path);
        return $this->{$cachedValueKey};
    }
    croak("can not find OutputPath for project $this->{project} and config $config");
}

sub getProjectType
{
    my ($this) = @_;
    if (defined($this->{projectType}))
    {
        return $this->{projectType};
    }

    $this->{projectType} = PROJECT_TYPE_UNKNOWN;
    for my $pg (@{ $this->{xml}->{PropertyGroup} })
    {
        my $outputType = $pg->{OutputType};
        if (defined($outputType) and lc($outputType) eq 'library')
        {
            $this->{projectType} = PROJECT_TYPE_CS_CLASS_LIBRARY;
            last;
        }

        if (defined($outputType) and lc($outputType) eq 'exe')
        {
            $this->{projectType} = PROJECT_TYPE_CS_EXE;
            last;
        }
    }

    return $this->{projectType};
}

sub getAssemblyNameWithExtenstion
{
    my ($this, $config) = @_;
    my $assemblyNameWithExtention = $this->getAssemblyName();
    my $projectType = $this->getProjectType();
    if ($projectType eq PROJECT_TYPE_CS_CLASS_LIBRARY)
    {
        $assemblyNameWithExtention .= ".dll";
    }
    elsif ($projectType eq PROJECT_TYPE_CS_EXE)
    {
        $assemblyNameWithExtention .= ".exe";
    }
    else
    {
        croak("Project $this->{project} is not a class library");
    }
    return $assemblyNameWithExtention;
}

sub getOutputPath
{
    my ($this, $config) = @_;
    my $cachedValueKey = "OutputPath$config";
    if (defined($this->{$cachedValueKey}))
    {
        return $this->{$cachedValueKey};
    }
    for my $pg (@{ $this->{xml}->{PropertyGroup} })
    {
        my $condition = $pg->{Condition};
        next if not defined($condition);
        next if $condition !~ qr/$config/i;

        my $outputPath = canonpath($pg->{OutputPath});
        $outputPath =~ s/\\/\//g;
        $this->{$cachedValueKey} = $outputPath;
        return $this->{$cachedValueKey};
    }
}

sub getFullOutputPath
{
    my ($this, $config) = @_;
    my $outputPath = $this->getOutputPath($config);
    if (not file_name_is_absolute($outputPath))
    {
        my $projectPath = dirname($this->{project});
        $outputPath = catfile($projectPath, $outputPath);
    }
    return canonpath($outputPath);
}

sub getAssemblyName
{
    my ($this, @propertyGroups) = @_;
    my $cachedValueKey = 'AssemblyName';
    if (defined($this->{$cachedValueKey}))
    {
        return $this->{$cachedValueKey};
    }

    for my $pg (@{ $this->{xml}->{PropertyGroup} })
    {
        my $assemblyName = $pg->{AssemblyName};
        if (defined($assemblyName))
        {
            $this->{$cachedValueKey} = $assemblyName;
            return $this->{$cachedValueKey};
        }
    }
    croak("can not resolve assembly name for project $this->{project}");
}

1;
