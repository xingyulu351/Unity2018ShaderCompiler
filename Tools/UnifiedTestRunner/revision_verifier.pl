use warnings;
use strict;

use Cwd;
use File::Spec::Functions qw (canonpath);
use File::Basename;
use FindBin qw ($Bin);
use Getopt::Long qw (:config pass_through);

use lib "$Bin";

use RevisionVerifier;
use Params::Check qw (check);

use Data::Dumper;

sub main
{
    my %options = (
        repo_dir => cwd(),
        utr_dir => dirname(canonpath($0))
    );
    GrabArgs(\%options);

    my $revision_verifier = new RevisionVerifier(%options);

    my $verification_result = $revision_verifier->verify();
    if ($verification_result eq 'abort')
    {
        abortBisection();
    }
    elsif ($verification_result eq 'skip')
    {
        skipThisRevision();
    }
    elsif ($verification_result eq 'good')
    {
        markRevisionGood();
    }
    elsif ($verification_result eq 'bad')
    {
        markRevisionBad();
    }
    else
    {
        print(STDERR "Unexpected verification result: $verification_result");
        abortBisection();
    }
}

sub GrabArgs
{
    my ($options) = @_;
    GetOptions($options, 'suite=s', 'platform=s', 'bisect_custom_patch_command=s', 'artifacts_path=s', 'verbose+') or printUsage();
    if (not exists $options->{suite})
    {
        printUsage();
    }
    my @utr_args = @ARGV;
    push(@utr_args, "--suite=$options->{suite}");
    push(@utr_args, "--platform=$options->{platform}") if exists $options->{platform};
    $options->{utr_args} = \@utr_args;
}

sub printUsage
{
    print("You must supply --suite and optionally --bisect_custom_patch_command and --artifacts_path\n");
    abortBisection();
}

# Exit statuses are on http://www.selenic.com/hg/help/bisect
sub abortBisection { exit(127); }

sub skipThisRevision { exit(125); }

sub markRevisionGood { exit(0); }

sub markRevisionBad { exit(1); }

sub revisionId { return $ENV{'HG_NODE'}; }

$SIG{__DIE__} = sub
{
    print(STDERR @_);
    abortBisection();
    return 1;
};

main();
