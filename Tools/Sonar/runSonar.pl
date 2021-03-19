#!/usr/bin/env perl

package Main;

my $dir = File::Spec->rel2abs(dirname($0));
chdir("$dir/../..");

use strict;
use warnings;
use File::Basename qw (dirname basename fileparse);
use File::Spec;
use File::Path;
use File::Glob;
use File::Find;
use File::Basename;
use File::Copy;
use lib ('.', "./Configuration");
use BuildConfig;
use lib ('.', "./Tools/Build");
use PrepareWorkingCopy qw(PrepareExternalDependency);

my $branch = `hg branch`;
chomp($branch);

my $version = "$BuildConfig::unityVersion";
my $projectSettings = "Tools/Sonar/sonar-project.properties";
my $scannerBasePath = "External/SonarQube";
my $scannerPath = "$scannerBasePath/builds/sonar-scanner-2.6.1";

PrepareExternalDependency($scannerBasePath);

my $flags = "";    # use -e -X for debugging

print "Analysing $version ($branch)\n";

system("\"$scannerPath/bin/sonar-scanner\" -Dproject.settings=$projectSettings -Dsonar.projectVersion=$version -Dsonar.branch=$branch $flags");
