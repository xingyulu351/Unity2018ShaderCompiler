package PrepareWorkingCopy;

use warnings;
use strict;
use File::Basename qw(dirname basename );
use File::Spec;
use File::Copy;
use File::Path;
use Cwd qw(realpath);
use File::Touch;
use Digest::MD5 qw(md5);
use Tools qw(UCopy Jam SetupMono);
use File::Find;
use Time::HiRes;

require Exporter;
our @ISA = qw(Exporter);
our @EXPORT_OK = qw(PrepareWorkingCopy PrepareExternalDependency UnzipExternalDependency UnzipToDirectory GetSmartFilter);

my ($modulePath, $root);

BEGIN
{
    $modulePath = realpath(File::Spec->rel2abs(dirname(__FILE__)) . '/../../External/Perl/lib');
    $root = realpath(File::Spec->rel2abs(dirname(__FILE__)) . '/../..');
}

sub PrepareWorkingCopy
{
    my ($preparingForHuman, $platform, $jamArgs) = @_;
    if ($^O eq 'linux')
    {
        chomp(my $arch = `uname -m`);
        if (index($arch, "arm") != -1)
        {
            $platform = 'linux-armel';
        }
        else
        {
            $platform = 'linux64';
        }
    }

    # Jam will be installed by jam.pl if not present.
    # .\jam.bat (for win) and ./jam (bash shell) simply forwards to jam.pl
    my $jam = "$root/jam";
    if ($^O eq 'MSWin32')
    {
        $jam = "$jam.bat";
    }

    my $jamArgsForPrepareWorkingCopy = $jamArgs;
    if (defined $platform and $platform)
    {
        $jamArgsForPrepareWorkingCopy .= "-sPLATFORM=$platform";
    }

    system("$jam PrepareWorkingCopy $jamArgsForPrepareWorkingCopy") eq 0 or die("Failed jamming PrepareWorkingCopy");

    SetupMono($platform);

    if ($preparingForHuman)
    {
        system("$jam NativeProjectFiles $jamArgs") eq 0 or die("Jam failed generating native IDE projects");
    }
}

sub GetFileMD5
{
    my $file = shift;
    open(FILE, $file) or die "Can't open '$file': $!";
    binmode(FILE);
    my $md5 = Digest::MD5->new->addfile(*FILE)->hexdigest;
    close(FILE);
    return $md5;
}

sub ReadMD5FromFile
{
    my $md5cachefile = shift;
    open A, $md5cachefile or return "";
    my @rows = <A>;
    close A;
    if (@rows > 0)
    {
        $rows[0] =~ s/\015?\012?$//;
        return $rows[0];
    }
    else
    {
        return "";
    }
}

sub ReadSmartFilterFromFile
{
    my $md5cachefile = shift;
    open A, $md5cachefile or return "";
    my @rows = <A>;
    close A;
    if (@rows > 1)
    {
        $rows[1] =~ s/\015?\012?$//;
        return $rows[1];
    }
    else
    {
        return "";
    }
}

sub WriteMD5AndSmartFilterToFile
{
    my $md5cachefile = shift;
    my $md5 = shift;
    my $smartFilter = shift;
    open A, ">$md5cachefile";
    print A $md5 . "\n" . $smartFilter;
    close A;
}

sub GetListOfMD5AndSmartFilterFiles
{
    my $targetdir = shift;
    opendir(DIR, $targetdir);
    my @lastExtractedFiles = grep(/\.last_extracted_md5$/, readdir(DIR));
    closedir(DIR);

    return @lastExtractedFiles;
}

sub DeleteOldMD5AndSmartFilterFiles
{
    my $targetdir = shift;
    my @lastExtractedFiles = GetListOfMD5AndSmartFilterFiles($targetdir);

    foreach my $file (@lastExtractedFiles)
    {
        unlink("$targetdir/$file") or warn("Failed to delete old .last_extracted_md5 file $targetdir/$file\n");
    }
}

sub GetNumberOfMD5AndSmartFilterFiles
{
    my $targetdir = shift;
    my @lastExtractedFiles = GetListOfMD5AndSmartFilterFiles($targetdir);

    return scalar(@lastExtractedFiles);
}

sub PrepareExternalDependency
{
    my ($basepath) = $_[0];
    my $verbosity = @_ >= 2 ? $_[1] : 0;
    my $targetdir = "$root/$basepath/builds";
    my $binaryfile = "";
    if (-e "$root/$basepath/builds.7z")
    {
        $binaryfile = "$root/$basepath/builds.7z";
    }
    elsif (-e "$root/$basepath/builds.tar.gz")
    {
        $binaryfile = "$root/$basepath/builds.tar.gz";
    }
    elsif (-e "$root/$basepath/builds.zip")
    {
        $binaryfile = "$root/$basepath/builds.zip";
    }
    return UnzipExternalDependency($binaryfile, undef, $verbosity);
}

# While preparing external "builds" dependencies, we can skip extracting
# files from them which are not relevant on current platform (e.g. if we're running
# on Mac, there's no point extracting Windows Visual Studio libraries etc.).
#
# This is achieved by having a *.smart_filter file next to the builds file,
# which contains one folder per line. Within that folder inside the builds file,
# subfolders matching particular patterns (e.g. "win*") are skipped as needed.
sub GetSmartFilter
{
    my $file = shift;
    $file .= ".smart_filter";
    open(FILE, $file) or return ();
    my @rows = <FILE>;
    close(FILE);

    $rows[0] =~ s/\015?\012?$//;
    @rows = grep /\S/, @rows;    # remove empty lines
    if (!@rows)
    {
        # if all lines ended up empty, push back one empty line;
        # this will remove platform-named folders right inside the builds file root
        push @rows, '';
    }
    chomp(@rows);

    # Windows specific paths:
    my @windows = ("win*", "vs2008", "vs2010", "vs2012", "vs2015", "WIN32*", "WIN64*");
    my @windowsApps = ("metro*", "uap_*", "uwp", "wsa81");
    my @windowsPhone8 = ("wp8*", "WP8", "wpBlue*");
    my @allWindows = (@windows, @windowsApps, @windowsPhone8);

    # OS X specific paths:
    my @osx = ("macos*", "osx*", "gcc4");
    my @osxIOS = ("iphone", "ios*", "tvos*");
    my @allOSX = (@osx, @osxIOS);

    # Linux specific paths:
    my @linux = ("linux*", "LINUX*", "gcc4");
    my @allLinux = @linux;

    # Build exclusion filter based on which platform we're running on
    my @filter = ();
    if ($^O eq 'MSWin32')
    {
        # Running on Windows
        @filter = (@filter, @allOSX);    # skip all OS X parts
        @filter = (@filter, @allLinux);  # skip all Linux parts
        @filter = (@filter, @windowsPhone8);    # skip all WinPhone parts (no longer needed)
    }
    elsif ($^O eq 'darwin')
    {
        # Running on Mac
        @filter = (@filter, @allWindows);       # skip all Windows parts
        @filter = (@filter, @allLinux);         # skip all Linux parts

        # Handle collisions in OS filters (e.g. 'gcc4')
        my %actualFilters = map { $_ => 1 } @allOSX;
        my @tmp = grep { not $actualFilters{$_} } @filter;
        @filter = @tmp;
    }
    else
    {
        # Assuming that running on Linux
        @filter = (@filter, @allWindows);       # skip all Windows parts
        @filter = (@filter, @allOSX);           # skip all OS X parts

        # Handle collisions in OS filters (e.g. 'gcc4')
        my %actualFilters = map { $_ => 1 } @allLinux;
        my @tmp = grep { not $actualFilters{$_} } @filter;
        @filter = @tmp;
    }

    # cartersian product of all folders x all filters
    my @product = ();
    foreach my $a (@rows)
    {
        foreach my $b (@filter)
        {
            push @product, "-x!$a$b";
        }
    }
    return @product;
}

sub UnzipToDirectory
{
    my $root = shift(@_);
    my $binaryfile = shift(@_);
    my $targetdir = shift(@_);
    my $verbosity = shift(@_);
    my @smart = @_;

    print "Extracting $binaryfile into $targetdir\n";
    if ($^O eq 'MSWin32')
    {
        my $ziptool = "$root/External/7z/win64/7za.exe";
        if ($binaryfile =~ /\.zip$/ || $binaryfile =~ /\.7z$/)
        {
            system($ziptool, "x", "-aoa", "-bd", "-ba", "-o$targetdir", @smart, $binaryfile) eq 0 or die("Failed to unzip $binaryfile\n");
        }
        elsif ($binaryfile =~ /\.tar$|\.tar.gz$/)
        {
            system($ziptool, "x", "-bd", "-ba", "-o$targetdir", $binaryfile) eq 0 or die("Failed to decompress $binaryfile\n");
            system($ziptool, "x", "-aoa", "-bd", "-ba", "-o$targetdir", @smart, "$targetdir/builds.tar") eq 0 or die("Failed to extract $binaryfile\n");
            unlink("$targetdir/builds.tar");
        }
    }
    else
    {
        if ($binaryfile =~ /\.tar$|\.tar.gz$/)
        {
            system("tar", "-xf", "$binaryfile", "-C", $targetdir) eq 0 or die("failed to extract $binaryfile");
        }
        elsif ($binaryfile =~ /\.zip$/ || $binaryfile =~ /\.7z$/)
        {
            if ($^O eq 'darwin')
            {
                system("chmod +x '$root/External/7z/osx/7za'");    # ensure it has executable bit (sometimes lost due to hg bugs)
                my @args = ('x');
                if ($verbosity >= 1)
                {
                    push @args, '-bb3';
                }
                else
                {
                    push @args, '-bb0';
                    push @args, '-bd';
                    push @args, '-ba';
                }
                system("$root/External/7z/osx/7za", @args, "$binaryfile", "-o$targetdir", @smart) eq 0 or die("failed to '7za x' $binaryfile");
            }
            else
            {
                system("$root/External/7z/linux64/7za", "x", "-bd", "-ba", "-o$targetdir", $binaryfile, @smart) eq 0 or die("failed to un p7zip $binaryfile");
            }
        }
    }
    return 1;
}

sub UnzipExternalDependency
{
    my $start_time = [Time::HiRes::gettimeofday()];
    my $binaryfile = $_[0];
    my $targetdir = dirname($binaryfile);

    my $postfix = (@_ >= 2 and defined $_[1]) ? $_[1] : "builds";
    my $verbosity = @_ >= 3 ? $_[2] : 0;
    my $cachefile = @_ >= 4 ? $_[3] : $binaryfile . ".last_extracted_md5";
    touch($cachefile);

    $targetdir .= "/$postfix";

    if (-e $binaryfile)
    {
        my $md5 = GetFileMD5($binaryfile);
        my @smart = GetSmartFilter($binaryfile);
        my $smartstr = join(" ", @smart);
        my $lastmd5 = ReadMD5FromFile($cachefile);
        my $lastsmart = ReadSmartFilterFromFile($cachefile);
        my $numberOfLastExtractedFiles = GetNumberOfMD5AndSmartFilterFiles(dirname($binaryfile));

        if (($lastmd5 eq $md5) and ($lastsmart eq $smartstr) and -e $targetdir and ($numberOfLastExtractedFiles == 1))
        {
            if ($verbosity >= 1 || $ENV{UNITY_THISISABUILDMACHINE})
            {
                print "Skipping extraction of $binaryfile: Nothing changed (md5=$md5, filter='$smartstr', dest folder exists).";
                if ($verbosity >= 1)
                {
                    print " Files in dest folder:\n";
                    my @files;
                    find sub
                    {
                        return if -d;
                        push @files, $File::Find::name;
                    }, $targetdir;
                    for my $f (@files)
                    {
                        print "  $f\n";
                    }
                }
                else
                {
                    print "\n";
                }
            }

            return 0;
        }

        # Log status of showing why unpacking is necessary
        print "Preparing $binaryfile:\n MD5=$md5\n LastMD5=$lastmd5\n";
        if (@smart)
        {
            print " SMART=$smartstr\n";
        }

        unlink($cachefile);
        rmtree $targetdir;
        mkpath $targetdir;

        UnzipToDirectory($root, $binaryfile, $targetdir, $verbosity, @smart);

        if (-e $targetdir)
        {
            DeleteOldMD5AndSmartFilterFiles(dirname($binaryfile));
            WriteMD5AndSmartFilterToFile($cachefile, $md5, $smartstr);
        }
        else
        {
            die "Unpacking builds zip failed";
        }

        printf("Unzip elapsed time: %.2fs [%s] \n", Time::HiRes::tv_interval($start_time), $binaryfile);

        my @all_file_names;

        find sub
        {
            return if -d;
            push @all_file_names, $File::Find::name;
        }, $targetdir;

        print "Extracted files:\n" if $verbosity >= 1;
        for my $file (@all_file_names)
        {
            print "  $file\n" if $verbosity >= 1;

            # On Unixish systems, we might end up with broken symlinks (also known as the MacOSX SDK)
            # so skip every file that doesn't resolve to anything valid.
            next unless -e $file;

            if ($file =~ 'InterLok-5.8b7-Mac-Files/InterLok' && $^O eq 'MSWin32')
            {
                print "WARNING: touching $file on Windows8 will make 'Invalid argument' pop up";
            }
            else
            {
                touch($file);
            }
        }

        return 1;
    }
}

1;
