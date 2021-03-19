my $arg = shift;
while ($arg =~ m/(.*)=(.*)/)
{
    $ENV{$1} = $2;
    $arg = shift;
}
$exit_status = system($arg, @ARGV) >> 8;
exit $exit_status;
