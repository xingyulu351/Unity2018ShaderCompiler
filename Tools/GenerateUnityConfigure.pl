#!/usr/bin/perl
# Will generate:
# artifacts/generated/Configuration/UnityConfigure*.gen.h
# artifacts/generated/Configuration/Property folder
# based on Configuration/BuildConfig.pm contents and version control info

use strict;
use warnings;
use FindBin qw($Bin);
use lib "$Bin/Build";
use Tools qw (GenerateUnityConfigure);

GenerateUnityConfigure();
