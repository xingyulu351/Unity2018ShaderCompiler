use strict;
use warnings;
use File::Find qw (find);
use File::Spec::Functions qw (catfile);
use Carp qw (croak);
use Cwd qw (abs_path);

sub PatchQtLibs
{
    my ($qtPath, $diagOutput, $printOnly) = @_;
    $qtPath = abs_path($qtPath);
    my $qtLibPath = catfile($qtPath, 'lib');
    if (!-d $qtLibPath)
    {
        croak("There is no lib subdir under $qtPath\n");
    }
    my %options = (wanted => sub { ProcessDylib($qtLibPath, $printOnly, $diagOutput); });
    find(\%options, $qtPath);
}

sub ProcessDylib
{
    my ($qtLibPath, $printOnly, $diagOutput) = @_;
    my $dylib = $File::Find::name;
    if (-x $dylib && !-d $dylib && !-l $dylib && -B $dylib)
    {
        ChangeDylibId($dylib, $printOnly, $diagOutput);
        open(my $otool, '-|', "otool -L $dylib");
        while (my $line = <$otool>)
        {
            $line =~ /\w\/[^\/]*Qt[^\/]*\ \(/ or next;
            $line =~ /(^\s*)(.+)(\s\(.+\))/ or next;
            my $oldPath = $2;
            my $versString = $3;
            next if ($oldPath =~ /^$dylib/i);
            $oldPath =~ /(.*\/)([^\/]*$)/;
            my $newPath = catfile($qtLibPath, GetNewDylibName($2, GetDylibVersion($versString)));
            ChangeDepDylibPath($dylib, $oldPath, $newPath, $printOnly, $diagOutput);
        }
    }
}

sub ChangeDylibId
{
    my ($dylib, $printOnly, $diagOutput) = @_;
    if ($diagOutput or $printOnly)
    {
        print("Inspecting $dylib\n");
    }
    if (!$printOnly)
    {
        system("install_name_tool -id $dylib $dylib");
    }
}

sub ChangeDepDylibPath
{
    my ($dylib, $oldPath, $newPath, $printOnly, $diagOutput) = @_;
    if ($diagOutput or $printOnly)
    {
        print("old path:\t$oldPath\n");
        print("new path:\t$newPath\n");
    }
    if (!$printOnly)
    {
        system("install_name_tool -change $oldPath $newPath $dylib");
    }
}

sub GetNewDylibName
{
    my ($name, $version) = @_;
    $name =~ /(\w*\.)(.*)(\.dylib$)/;
    my $newName = $1 . $version . $3;
    return $newName;
}

sub GetDylibVersion
{
    my ($versString) = @_;
    $versString =~ /(.*current\sversion\s)([0-9\.]*)(.*)/i;
    return $2;
}

1;
