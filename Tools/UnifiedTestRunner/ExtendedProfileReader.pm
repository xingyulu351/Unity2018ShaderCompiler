package ExtendedProfileReader;

use warnings;
use strict;

use ExtendedProfile::Profile;
use FileUtils;
use lib Dirs::external_root();
use JSON;
use Logger;
use File::Basename qw (fileparse);
use ArgumentNamesExtractor;
use List::MoreUtils qw (any);
use File::Spec::Functions qw (canonpath);

sub read
{
    my ($profile, $argv) = @_;
    if (not $profile)
    {
        return undef;
    }

    my ($fileName) = fileparse($profile);
    if ($fileName eq $profile)
    {
        my $root = Dirs::getRoot();
        $fileName = "$root/Tools/UnifiedTestRunner/Profiles/$profile";
    }
    else
    {
        $fileName = $profile;
    }

    $fileName = canonpath($fileName);
    Logger::minimal("Reading run profile from $fileName");
    my $text = join(q (), FileUtils::readAllLines($fileName));
    my $obj = jsonToObj($text);
    my $result = new ExtendedProfile::Profile($obj);
    if (defined($argv))
    {
        _embedCombinedArgs($result, $argv);
    }
    return $result;
}

sub _embedCombinedArgs
{
    my ($profile, $argv) = @_;

    my @suites = $profile->getSuites();
    foreach my $s (@suites)
    {
        my @suiteArgs = $s->getArgs();
        my @combinedArgs;
        if ($s->getCommandLineHasPriority())
        {
            @combinedArgs = _combine($argv, \@suiteArgs);
        }
        else
        {
            @combinedArgs = _combine(\@suiteArgs, $argv);
        }
        $s->setCombinedArgs(@combinedArgs);
    }
}

sub _combine
{
    my ($suiteArgs, $argv) = @_;
    my @suiteArgsNames = ArgumentNamesExtractor::extract(@{$suiteArgs});
    my @argvNames = ArgumentNamesExtractor::extract(@{$argv});
    my @result = @{$suiteArgs};
    my @argvCopy = @{$argv};
    for (my $i = 0; $i < scalar(@argvCopy); $i++)
    {
        my $argvName = $argvNames[$i];
        if (any { $_ eq $argvName } @suiteArgsNames)
        {
            next;
        }
        push(@result, $argvCopy[$i]);
    }
    return @result;
}

1;
