use warnings;
use strict;

use Test::More;

use AppHelper;

use Dirs;
use lib Dirs::external_root();
use Test::MockModule;

CanExpandProfileArgs:
{
    my @profileCommandLine = ('--suite=native', '--category=Unit', '--category=Integration', '--category=Stress', '--category=Regression');
    my $profileReaderMock = new Test::MockModule('ProfileArgsReader');
    $profileReaderMock->mock(
        read => sub
        {
            return @profileCommandLine;
        }
    );

    my $result = AppHelper::parseOptions('--profile=katana-native-player');

    is_deeply($result->{expandedCommandLine}, \@profileCommandLine, 'remove profile from string after expanded');
}

CanReadHiddenArgs:
{
    $ENV{'UTR_HIDDEN_ARGS'} = '["--submitter=submitterName"]';
    my $result = AppHelper::parseOptions('--suite=native');
    is_deeply($result->{expandedCommandLine}, ['--suite=native', '--submitter=submitterName']);
    delete $ENV{'UTR_HIDDEN_ARGS'};
}

done_testing();
