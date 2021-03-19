use warnings;
use strict;

use FindBin qw ($Bin);
use lib $Bin;

use Capabilities;

use Dirs;
use lib Dirs::external_root();

use JSON;

# this is script is use by katana to query UTR's capabilities
sub main ()
{
    my $capabilities = Capabilities::getCapabilities();
    print(objToJson($capabilities));
}

main();
