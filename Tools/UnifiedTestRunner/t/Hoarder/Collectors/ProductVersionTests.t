use warnings;
use strict;

use Test::More;

use Dirs;
use lib Dirs::external_root();
use Test::MockObject;

my @originalInc;

BEGIN
{
    @originalInc = @INC;
    use_ok('Hoarder::Collectors::ProductVersion');
}

RECEIVE_DATA:
{
    my $revision = new Test::MockObject();
    $revision->mock(
        getId => sub
        {
            return 'revisionId';
        }
    );

    $revision->mock(
        getDate => sub
        {
            return '123';
        }
    );

    $revision->mock(
        getBranch => sub
        {
            return 'branchName';
        }
    );

    $revision->mock(
        getVcsType => sub
        {
            return 'vcsName';
        }
    );

    $revision->mock(
        getBranches => sub
        {
            return ['branch1', 'branch2'];
        }
    );

    my $pi = new Hoarder::Collectors::ProductVersion($revision);
    is($pi->name(), 'product-version');

    my %data = %{ $pi->data() };

    is($data{revision}, 'revision_revisionId');
    is($data{revision_date}, '123');
    is($data{branch}, 'branchName');
    is($data{unity_version}, $BuildConfig::unityVersion);
    is($data{vcs}, 'vcsName');
    is_deeply($data{branches}, ['branch1', 'branch2']);
    is(scalar(keys(%data)), 6);
    is_deeply(\@originalInc, \@INC);
}

done_testing();
