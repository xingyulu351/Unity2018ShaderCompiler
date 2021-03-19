use Test::More;

use warnings;
use strict;

use File::Spec::Functions qw (canonpath);

use Dirs;
use lib Dirs::external_root();
use Test::MockObject;
use Test::Trap;

BEGIN { use_ok('ArtifactsFolderDeleterGuard') }

can_ok('ArtifactsFolderDeleterGuard', 'safeFolderDelete');

ERROR_HANDLING:
{
    my @trap = trap
    {
        ArtifactsFolderDeleterGuard::safeFolderDelete();
    };
    ok($trap->die, 'croak when created without params');

    @trap = trap
    {
        ArtifactsFolderDeleterGuard::safeFolderDelete(
            folder => '/tmp/runtime/editor',
            suite => 'runtime',
            platform_or_target => 'editor'
        );
    };
    like($trap->die, qr/deleter/, 'croaks when deleter not be specified');

    my $deleter = new Test::MockObject();
    $deleter->mock('delete' => sub { });
    @trap = trap
    {
        ArtifactsFolderDeleterGuard::safeFolderDelete(
            ffolder => '/tmp/runtime/editor',
            platform_or_target => 'editor',
            deleter => $deleter
        );
    };
    like($trap->die, qr/suite/, 'croaks when suite not be specified');

    @trap = trap
    {
        ArtifactsFolderDeleterGuard::safeFolderDelete(
            folder => '/tmp/runtime/editor',
            suite => 'runtime',
            deleter => $deleter
        );
    };
    like($trap->die, qr/platform/, 'croaks when platform not be specified');

    @trap = trap
    {
        ArtifactsFolderDeleterGuard::safeFolderDelete(
            folder => '/tmp/runtime/editor',
            suite => 'runtime',
            deleter => $deleter
        );
    };
    like($trap->die, qr/folder/, 'croaks when folder not be specified');

    @trap = trap
    {
        my $driveRoot = '/';
        if ($^O =~ /mswin32|mswin64|cygwin/i)
        {
            $driveRoot = 'd:/';
        }
        ArtifactsFolderDeleterGuard::safeFolderDelete(
            folder => $driveRoot,
            platform_or_target => 'editor',
            suite => 'runtime',
            deleter => $deleter
        );
    };
    like($trap->die, qr/root/, 'croaks when folder not be specified');

    @trap = trap
    {
        my $repositoryRoot = Dirs::repositoryRoot();
        ArtifactsFolderDeleterGuard::safeFolderDelete(
            folder => $repositoryRoot,
            platform_or_target => 'editor',
            suite => 'runtime',
            deleter => $deleter
        );
    };
    like($trap->die, qr/repository/, 'croaks if repository root is specified');
}

BASIC_SUCCESULL_SCENARIO:
{
    my $folder_being_deleted = undef;
    my $deleter = new Test::MockObject();
    $deleter->mock(
        'delete' => sub
        {
            shift();
            $folder_being_deleted = shift();
        }
    );

    ArtifactsFolderDeleterGuard::safeFolderDelete(
        folder => '/tmp/editor/runtime',
        suite => 'runtime',
        platform_or_target => 'editor',
        deleter => $deleter
    );

    is($folder_being_deleted, canonpath("/tmp/editor/runtime/"));
}

done_testing();
