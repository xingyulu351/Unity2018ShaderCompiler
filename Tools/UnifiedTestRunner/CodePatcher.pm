package CodePatcher;

use strict;
use warnings;

use Carp qw (croak);
use File::Copy;
use File::pushd;
use File::Spec::Functions qw (canonpath catfile);
use File::Temp;
use IO::File;
use Params::Check qw (check);

use SystemCall;

sub new
{
    my ($package, %args) = @_;
    my $default_config = [
        [
            'Tools/Build/Tools.pm', 'ShouldRunNativeTests',
            'push (@args, "-sUNITY_RUN_NATIVE_TESTS_DURING_BUILD=1");', '# push (@args, "-sUNITY_RUN_NATIVE_TESTS_DURING_BUILD=1");'
        ]
    ];
    my $arg_scheme = {
        code_path => { required => 1 },
        log_file_writer => { required => 0 },
        bisect_custom_patch_command => { required => 0 },
        system_call => { default => new SystemCall() },
        config => { default => $default_config, strict_type => 1 }
    };
    my $obj = check($arg_scheme, \%args, 1) or croak('Failed to parse arguments');
    return bless($obj, $package);
}

sub applyPatches
{
    my ($this) = @_;
    if ($this->{bisect_custom_patch_command})
    {
        my $patched = $this->executeApplyCustomPatch();
        if (!$patched)
        {
            return 0;
        }
    }
    my $not_failed = 1;
    for my $patch (@{ $this->{config} })
    {
        my $result = $this->applyPatch(@{$patch});
        if ($result eq 'failed')
        {
            $not_failed = 0;
        }
    }
    return $not_failed;
}

sub applyPatch
{
    my ($this, $file_relative_path, $skip_test_re, $patch_find_re, $patch_replace_re) = @_;
    my $file_path = canonpath(catfile($this->{code_path}, $file_relative_path));
    if (!-e $file_path)
    {
        return 'skipped';
    }
    if ($this->fileMatchesRE($file_path, $skip_test_re))
    {
        return 'skipped';
    }
    else
    {
        if ($this->patchFile($file_path, $patch_find_re, $patch_replace_re))
        {
            return 'patched';
        }
        else
        {
            return 'failed';
        }
    }
}

sub fileMatchesRE
{
    my ($this, $file_path, $re) = @_;
    my $file = new IO::File($file_path, O_RDONLY) or croak("Failed to open $file_path");
    if (grep (/\Q$re\E/, <$file>))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

sub patchFile
{
    my ($this, $file_path, $find_re, $replace_re) = @_;
    my $out_file = new File::Temp() or croak("Failed to create temp file");
    my $replaced = 0;
    {
        my $in_file = new IO::File($file_path, O_RDONLY) or croak("Failed to open $file_path");
        while (defined(my $line = <$in_file>))
        {
            if ($line =~ s/\Q$find_re\E/$replace_re/)
            {
                $replaced = 1;
            }
            print($out_file $line);
        }
    }
    if ($replaced)
    {
        $out_file->unlink_on_destroy(0);
        $out_file->close();
        move($out_file->filename(), $file_path) or croak("Failed to replace file $file_path");
    }

    return $replaced;
}

sub executeApplyCustomPatch
{
    my ($this) = @_;
    my $saved_dir = pushd($this->{code_path});
    my $command = $this->{bisect_custom_patch_command};
    my ($exitCode, $output) = $this->{system_call}->executeAndGetAllOutput($command);
    if ($exitCode and $this->{log_file_writer})
    {
        $this->{log_file_writer}->writeLog(module => 'patch', contents => [$output]);
    }
    return $exitCode == 0;
}

1;
