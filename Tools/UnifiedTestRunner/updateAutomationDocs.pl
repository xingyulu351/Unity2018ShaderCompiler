#!/usr/bin/perl

# This script is responsible for updating UTR generated help for local version of
# http://internaldocs.hq.unity3d.com/automation/
# If --suite=<name> is specified - only help for a specific test suite will be updated.
use warnings;
use strict;

use FindBin qw ($Bin);
use lib $Bin;

use SuiteRunners;

use AppHelper;
use Carp qw 'croak';
use Logger;
use File::Spec::Functions qw (canonpath);
use Logger;
use MDHelpGenerator;
use UtrUsage;

my $result = AppHelper::parseOptions(@ARGV);
my $options = $result->{options};
my $runContext = RunContext::new($options);
my $root = Dirs::getRoot();
Logger::initialize($options->getOptionValue('loglevel'));

my @runners = SuiteRunners::getRunners();
my $suites = $options->getOptionValue('suite');
if ($suites)
{
    @runners = AppHelper::getRunners(@$suites);
}
else
{

    # generate 1-st level of help
    my $helpFile = canonpath("$root/Tests/Docs/docs/UnifiedTestRunner/RunningTests.generated.md");
    my %utrUsage = UtrUsage::get();
    my @globalOptions = GlobalOptions::getOptions();
    _generateHelp($helpFile, \%utrUsage, \@globalOptions);
}

# generate 2-nd level of help (runners specific)
foreach my $runner (@runners)
{
    my $name = $runner->getName();
    my @options = $runner->getOptionsWithoutNamespace();
    my $autoDocHelpFile = _getOptionValue('autodoc-file', @options);
    if (not $autoDocHelpFile)
    {
        Logger::warning("There is no place in autodocs for '$name' suite runner");
        next;
    }
    my %usage = $runner->usage();
    $usage{name} = $name;
    _generateHelp($autoDocHelpFile, \%usage, \@options);
}

sub _getOptionValue
{
    my ($name, @options) = @_;
    my $options = new Options(options => \@options);
    return $options->getOptionValue($name);
}

sub _generateHelp
{
    my ($file, $usage, $options) = @_;
    my $text = MDHelpGenerator::getHelpText($usage, $options);
    FileUtils::writeAllLines($file, $text);
    Logger::minimal("Generated $file");
}
