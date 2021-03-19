package ResultSubmitter;

use warnings;
use strict;

use Dirs;
use lib Dirs::external_root();

use File::Which qw (which);
use File::Spec::Functions qw (canonpath catfile);

sub new
{
    my ($pkg) = shift @_;
    my %params = @_;
    my $object = {};
    bless($object, __PACKAGE__);
    $object->{systemCall} = $params{'system'};
    $object->{hoarderUri} = $params{'hoarder-uri'};
    $object->{artifactsRoot} = $params{'artifacts-root'};
    return $object;
}

sub getSystemCall
{
    my ($this) = @_;
    return $this->{systemCall};
}

sub getHoarderUri
{
    my ($this) = @_;
    return $this->{hoarderUri};
}

sub submit
{
    my ($this, $format, $headerFile, @dataFiles) = @_;
    my $hoarderUri = $this->getHoarderUri();
    my $submitScript = canonpath(Dirs::UTR_root() . "/submit.pl");
    my $perl = which('perl');
    my (@dataFileArgs);
    foreach my $dataFile (@dataFiles)
    {
        push(@dataFileArgs, "--data-file=$dataFile");
    }
    my @args = ("$submitScript", "--hoarder-uri=$hoarderUri", "--format=$format", "--header-file=$headerFile");

    if ($this->{artifactsRoot})
    {
        push(@args, "--log-file=" . catfile($this->{artifactsRoot}, "submitToHoarder.log"));
    }

    push(@args, @dataFileArgs);
    $this->getSystemCall()->fireAndForget(
        executable => $perl,
        args => \@args
    );
}

1;
