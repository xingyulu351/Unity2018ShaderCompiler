use Test::More;

use File::Spec::Functions qw (canonpath);

use Dirs;
use lib Dirs::external_root();
use Test::Trap;

use warnings;
use strict;

BEGIN { use_ok('ArtifactsNameBuilder'); }

SUITE_FOLDER:
{
    my $name = ArtifactsNameBuilder::getSuiteRunArtifactsFolderName(
        suite => 'runtime',
        platform_or_target => 'editor',
    );
    is($name, canonpath('editor/runtime'));

ERROR_HANDLING:
    {
        my @trap = trap
        {
            ArtifactsNameBuilder::getSuiteRunArtifactsFolderName(platform_or_target => 'editor');
        };
        like($trap->die, qr/suite/, 'suite must be specified');

        @trap = trap
        {
            ArtifactsNameBuilder::getSuiteRunArtifactsFolderName(suite => 'runtime');
        };
        like($trap->die, qr/platform/, 'platform must be specified');
    }
}

SUITE_SUB_FOLDER:
{
    my $name = ArtifactsNameBuilder::getFolderNameForArtifactType(type => 'PlayerBuild');
    is($name, 'PlayerBuild');

ERROR_HANDLING:
    {
        my @trap = trap
        {
            ArtifactsNameBuilder::getFolderNameForArtifactType();
        };
        like($trap->die, qr/type/, 'type must be specified');
    }
}

NAME_BUILDING:
{
    my $name = ArtifactsNameBuilder::getName(
        type => 'stdout',
        extension => 'txt'
    );

    is($name, 'stdout.txt');

    $name = ArtifactsNameBuilder::getName(type => 'stdout');
    is($name, 'stdout');

ERROR_HANDLING:
    {
        my @trap = trap
        {
            ArtifactsNameBuilder::getName(extension => 'txt');
        };
        like($trap->die, qr/type/, 'type must be specified');
    }
}

done_testing();
