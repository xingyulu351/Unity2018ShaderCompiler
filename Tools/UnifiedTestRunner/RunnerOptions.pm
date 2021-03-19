package RunnerOptions;

use warnings;
use strict;

use Dirs;
use lib Dirs::external_root();
use JSON;
use Option;

sub readFromFile
{
    my ($fileName) = @_;
    my $json = join(q (), FileUtils::readAllLines($fileName));
    return RunnerOptions::read($json);
}

sub read
{
    my ($json) = @_;
    local $JSON::UnMapping = 1;    #  null, true, false --> undef,1,0
    my $options = jsonToObj($json);
    my @result;
    foreach my $o (@{$options})
    {

        #assuming everything is a string, when read runner options
        $o->{typeSpec} = 's';
        if ($o->{acceptsMultipleValues})
        {
            $o->{typeSpec} .= '@';
        }
        push(@result, new Option(%{$o}));
    }
    return @result;
}

1;
