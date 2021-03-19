use warnings;
use strict;

use FindBin qw ($Bin);
use lib $Bin;

use Dirs;

use lib Dirs::getRoot() . "/Configuration";
use BuildConfig;

BEGIN { $| = 1 }

print($BuildConfig::unityVersion);
