use File::Basename;

my $target = shift;
my $sourceDir = shift;

my $dir = dirname($0);
my $root = "$dir/../..";
my $sevenZ;

if ($^O eq "darwin")
{
    $sevenZ = "$root/External/7z/osx/7za";
}
elsif ($^O eq "MSWin32")
{
    $sevenZ = "$root/External/7z/win64/7za.exe";
}
elsif ($^O eq "linux")
{
    $sevenZ = "$root/External/7z/linux64/7za";
}
else
{
    die("Unknown platform $^O\n");
}

my $tmp = $target;
$tmp =~ s/\.tgz$/\.tar/;
unlink($target);
system($sevenZ, "a", "-r", "-ttar", "-bd", "-bso0", $tmp, "$sourceDir") && die("failed running $sevenZ to compress $tmp\n");
system($sevenZ, "a", "-tgzip", "-bd", "-bso0", $target, $tmp) && die("failed running $sevenZ to compress $target\n");
unlink($tmp);
