package ProfileArgsReader;

use warnings;
use strict;

use File::Basename qw (fileparse);

sub resolveProfilePath
{
    my ($profile) = @_;
    my ($filename, $directories, $suffix) = fileparse($profile);
    if ($filename eq $profile)
    {
        my $root = Dirs::getRoot();
        return "$root/Tools/UnifiedTestRunner/Profiles/$profile";
    }
    return $profile;
}

sub read
{
    my ($fh) = @_;
    my @lines = <$fh>;
    foreach (@lines)
    {
        chomp;
    }
    my $line = join(' ', @lines);
    return split(' ', $line);
}

1;
