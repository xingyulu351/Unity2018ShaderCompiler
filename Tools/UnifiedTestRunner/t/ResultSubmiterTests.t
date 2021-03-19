use Test::More;

use warnings;
use strict;

use SystemCall;

use TestResult;
use Dirs;
use lib Dirs::external_root();
use TestHelpers::ArgsGrabber;
use TestHelpers::ArrayAssertions qw (arrayContains);

use File::Which qw (which);
use File::Spec::Functions qw (canonpath);

BEGIN { use_ok('ResultSubmitter'); }

can_ok('ResultSubmitter', 'new');

my $submitter = new ResultSubmitter();
isa_ok($submitter, 'ResultSubmitter');

Constructor_InitializeObjectProperly:
{
    my $systemCall = new Test::MockObject();
    my $submitter = new ResultSubmitter(
        'system' => $systemCall,
        'hoarder-uri' => 'http://dev.null'
    );

    is($submitter->getSystemCall(), $systemCall);
    is($submitter->getHoarderUri(), 'http://dev.null');
}

Submit_BuildCorrectCommandLine:
{
    my %params;
    my $systemCall = new Test::MockObject();
    my $submitter = new ResultSubmitter(
        'system' => $systemCall,
        'hoarder-uri' => 'http://dev.null'
    );
    $systemCall->mock(fireAndForget => sub { shift(); %params = @_; });

    my $submitScript = canonpath(Dirs::UTR_root() . "/submit.pl");
    $submitter->submit('F', "H", ('A', 'B'));
    my $executable = $params{executable};
    my @args = @{ $params{args} };

    is($executable, which('perl'));
    arrayContains(\@args, [$submitScript, '--hoarder-uri=http://dev.null', '--format=F', '--header-file=H', '--data-file=A', '--data-file=B']);
}

done_testing();
