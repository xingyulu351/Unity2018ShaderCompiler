package AutoComplete;

use warnings;
use strict;

use Option;
use Options;
use StringUtils;
use Getopt::Long qw (GetOptionsFromArray :config pass_through);
use Dirs;
use lib Dirs::external_root();
use List::MoreUtils qw (uniq);

sub new
{
    my ($pkg, %args) = @_;
    my $instance = {
        globalOptions => $args{globalOptions},
        suiteOptions => $args{suiteOptions}
    };

    return bless($instance, $pkg);
}

sub complete
{
    my ($this, $inputInfo) = @_;

    my $wordIdx = $inputInfo->{wordIdx};
    my @args = @{ $inputInfo->{argv} };
    my $wordToComplete = $args[$wordIdx];
    if (not defined($wordToComplete))
    {
        $wordToComplete = '';
    }

    my $globalOptions = $this->{globalOptions};
    my $suiteOptions = $this->_getSelectedSuiteOptions(@args);

    my ($name, $value) = ($wordToComplete =~ m/-{1,2}(.*)=(.*)/);
    if ($name)
    {    #wants options value
        my $o = _getOptionByName($name, $suiteOptions, $globalOptions);
        if (not $o)
        {
            return ();
        }
        my @values = $o->getAllowedValues();
        return _filterValues($value, @values);
    }

    my @options = $globalOptions->getOptions();
    if ($suiteOptions)
    {
        push(@options, $suiteOptions->getOptions());
    }
    my @names = _optionsToNames(@options);
    @names = _removeOptionsThatAlreadyAddedToTheCommandLine($inputInfo, @names);
    return _filterValues($wordToComplete, @names);
}

sub _removeOptionsThatAlreadyAddedToTheCommandLine
{
    my ($inputInfo, @names) = @_;
    my @result;
    my @argv = @{ $inputInfo->{argv} };
    my @left = @argv[0 .. $inputInfo->{wordIdx} - 1];
    foreach my $name (@names)
    {
        my @alreadyAdded = grep (/^$name/, @left);
        if (scalar(@alreadyAdded) > 0)
        {
            next;
        }
        push(@result, $name);
    }
    return @result;
}

sub _filterValues
{
    my ($pattern, @values) = @_;
    if (StringUtils::isNullOrEmpty($pattern))
    {
        return @values;
    }
    my @result = grep { $_ =~ /$pattern/ } @values;
    return uniq(@result);
}

sub _optionsToNames
{
    my (@options) = @_;

    my @names;
    foreach my $o (@options)
    {
        my @optNames = $o->getNames();
        if ($o->isHidden())
        {
            next;
        }
        if ($o->getValueType() == Option::ValueTypeNone)
        {
            push(@names, map { '--' . $_ } @optNames);
        }
        else
        {
            push(@names, map { '--' . $_ . "=" } @optNames);
        }
    }
    return @names;
}

sub _getSelectedSuiteOptions
{
    my ($this, @args) = @_;
    my $suite = _getSelectedSuite(@args);
    if ($suite and $this->{suiteOptions})
    {
        return $this->{suiteOptions}->{$suite};
    }
    return;
}

sub _getSelectedSuite
{
    my (@argv) = @_;
    if (not @argv)
    {
        return undef;
    }
    my $suite;
    GetOptionsFromArray(\@argv, 'suite:s' => \$suite);
    return $suite;
}

sub _getOptionByName
{
    my ($name, @optionSets) = @_;
    foreach my $oset (@optionSets)
    {
        if (not $oset)
        {
            next;
        }
        my $o = Options::getOptionByName($name, $oset);
        if ($o)
        {
            return $o;
        }
    }
    return undef;
}

1;
