package VSSolution;

use warnings;
use strict;

use File::Basename qw (dirname);
use Carp qw (croak);
use Cwd qw (realpath);
use File::Spec::Functions qw (canonpath);
use CSProj;
use FileUtils qw (openForReadOrCroak closeOrCroak);
use GUIDUtils;

sub new
{
    my ($pkg, $solution) = @_;
    my $instance = { solution => $solution };
    return bless($instance, $pkg);
}

sub getPath
{
    my ($this) = @_;
    return $this->{solution};
}

sub getAllClassLibraries
{
    my ($this) = @_;
    my @projects = $this->getCSProjects();
    my @result;
    foreach my $prj (@projects)
    {
        if ($prj->getProjectType() eq CSProj::PROJECT_TYPE_CS_CLASS_LIBRARY)
        {
            push(@result, $prj);
        }
    }

    return @result;
}

sub getCSProjects
{
    my ($this, $filter) = @_;
    if (not defined($this->{projects}))
    {
        my $fh = openForReadOrCroak($this->{solution});
        my $solutionPath = dirname($this->{solution});
        my @projects = _readProjectPaths($fh, $solutionPath, $filter);
        closeOrCroak($fh, $this->{solution});
        $this->{projects} = \@projects;
    }
    return @{ $this->{projects} };
}

sub getProjectPaths
{
    my ($this, $filter) = @_;
    my $fh = openForReadOrCroak($this->{solution});
    my $root = Dirs::getRoot();
    my $solutionPath = dirname($this->{solution});
    my @projects = _readProjectPaths($fh, $solutionPath, $filter);
    closeOrCroak($fh, $this->{solution});
    return @projects;
}

sub _readProjectPaths
{
    my ($fh, $solutionPath, $filter) = @_;
    my %solutionFolders;
    my %nestedProjectsMap;
    my $parsingNextedProjects = 0;
    my @csProjects;
    while (<$fh>)
    {
        my $line = $_;

        # is a C# project
        if ($line =~ m/^Project.*"(.*csproj)"/i)
        {
            my @components = split(m/[,]/, $line);
            my ($projectPath) = $components[1] =~ m/\"(.*)"/;
            $projectPath =~ s[\\][/]g;
            my $isAbsolute = File::Spec->file_name_is_absolute($projectPath);
            my $path;
            if ($isAbsolute)
            {
                $path = $projectPath;
            }
            else
            {
                $path = realpath(File::Spec->catfile($solutionPath, $projectPath));
            }
            if (defined($filter))
            {
                if ($path =~ $filter)
                {
                    push(@csProjects, new CSProj($path));
                }
            }
            else
            {
                push(@csProjects, new CSProj($path));
            }
            next;
        }

        if ($line =~ m/^Project[(]["][{]2150E333-8FDC-42A3-9474-1A3956D46DE8/i)
        {
            my @components = split(m/[,]/, $line);
            my ($name) = $components[1] =~ m/\"(.*)"/;
            my ($guid) = GUIDUtils::extractGUID($components[2]);
            $solutionFolders{$guid} = $name;
            next;
        }

        if ($line =~ m/GlobalSection[(]NestedProjects[)]/)
        {
            $parsingNextedProjects = 1;
            next;
        }

        if ($parsingNextedProjects)
        {
            my @components = split(m/[=]/, $line);
            if (not @components or scalar(@components) != 2)
            {
                next;
            }
            my $guid1 = GUIDUtils::extractGUID($components[0]);
            if (!$guid1)
            {
                next;
            }
            my $guid2 = GUIDUtils::extractGUID($components[1]);
            if (!$guid1)
            {
                next;
            }
            $nestedProjectsMap{$guid1} = $guid2;
            next;
        }

        if ($parsingNextedProjects and $line =~ /EndGlobalSection/)
        {
            $parsingNextedProjects = 0;
            next;
        }
    }

    #do the second pass and assign solution folders to cs projects
    foreach my $prj (@csProjects)
    {
        my $solutionFolderGUID = $nestedProjectsMap{ $prj->getGUID() };
        if (not $solutionFolderGUID)
        {
            next;
        }
        my @folders;
        my $tmpGuid = $solutionFolderGUID;
        while ($tmpGuid)
        {
            my $folder = $solutionFolders{$tmpGuid};
            if ($folder)
            {
                unshift(@folders, $folder);
                $tmpGuid = $nestedProjectsMap{$tmpGuid};
            }
            else
            {
                last;
            }
        }
        my $result = join('/', @folders);
        $prj->setSolutionFolder($result);
    }

    return @csProjects;
}

1;
