package GitRevision;

use warnings;
use strict;

use Dirs;
use StringUtils qw (trim);
use lib Dirs::external_root();

use JSON;

sub new
{
    my ($pkg, $systemCall) = @_;
    my $instance = { systemCall => $systemCall };
    $instance = bless($instance, $pkg);

    my @command = (
        'git', 'log', '--max-count=1',
        '--pretty=format:"{\"revision\":\"%h\", \"fullRevision\":\"%H\", \"author\":\"%ae\", \"date\":\"%cI\", \"branch\":\"%D\" }"'
    );

    my $stdout = $instance->{systemCall}->executeAndGetStdOut(@command);

    my $obj = jsonToObj($stdout);

    $instance->{revision} = $obj->{revision};
    $instance->{fullRevision} = $obj->{fullRevision};
    $instance->{date} = $obj->{date};
    $instance->{branch} = parseBranch($obj->{branch});
    $instance->{branches} = parseBranches($obj->{branch});
    return $instance;
}

# returns short revision number
sub getId
{
    my ($this) = @_;
    return $this->{revision};
}

sub getLongRevision
{
    my ($this) = @_;
    return $this->{fullRevision};
}

sub getDate
{
    my ($this) = @_;
    return $this->{date};
}

sub getBranch
{
    my ($this) = @_;
    return $this->{branch};
}

sub getBranches
{
    my ($this) = @_;
    return $this->{branches};
}

sub parseBranch
{
    my ($refOutput) = @_;
    my ($branch) = ($refOutput =~ m/HEAD\s?->\s?(.*?),/);
    return $branch;
}

sub parseBranches
{
    my ($branch) = @_;
    my @values = split(',', $branch);

    #"branch":"HEAD -> foobar, baz"
    #"HEAD -> foobar, baz, origin/HEAD"
    my @result;
    my $activeBranch = parseBranch($values[0]);
    shift(@values);
    foreach my $v (@values)
    {
        $v = trim($v);
        push(@result, $v);
    }

    return \@result;
}

sub getVcsType
{
    return 'git';
}

1;
