package MDHelpGenerator;

use warnings;
use strict;

use Option;
use StringUtils;

sub getHelpText
{
    my ($usage, $options) = @_;
    my $prerequisites = getPrerequisites(%{$usage});
    my $usageText = getUsageText(%{$usage});
    my $optionsText = getOptionsText(@{$options});
    return "$prerequisites$usageText\n**All options**:\n$optionsText";
}

sub getUsageText
{
    my (%params) = @_;
    my $result = "$params{description}\n";
    $result .= "\n**Usage**:\n";

    my $suiteName = $params{name};
    foreach my $x (@{ $params{examples} })
    {
        if ($suiteName)
        {
            $result .= ":\t`perl utr.pl --suite=$suiteName $x->{command}` - $x->{description}\n";
        }
        else
        {
            $result .= ":\t`perl utr.pl $x->{command}` - $x->{description}\n";
        }

    }
    $result .= "\n";
    return $result;
}

sub getOptionsText
{
    my @options = @_;
    if (scalar(@options) == 0)
    {
        return '';
    }
    my @result;
    foreach my $o (@options)
    {
        if ($o->isHidden())
        {
            next;
        }

        my $name = $o->getName();
        my $description = $o->getDescription();

        my $optionStr = "`--$name`";
        if ($o->getValueType() != Option::ValueTypeNone)
        {
            $optionStr .= '=';
        }

        $optionStr .= "\n: $description\n";
        my $allowedValuesStr = _getAllowedValuesString($o->getAllowedValues());
        if (not StringUtils::isNullOrEmpty($allowedValuesStr))
        {
            $optionStr .= "\n\tAllowed values: $allowedValuesStr\n";
        }

        my $defaultValue = $o->getDefaultValue();
        if (ref($defaultValue) eq 'ARRAY')
        {
            $optionStr .= "\n: Default:`";
            foreach my $v (@$defaultValue)
            {
                $optionStr .= "--$name=$v ";
            }
            $optionStr .= "`\n";

        }
        elsif (not StringUtils::isNullOrEmpty($defaultValue))
        {
            $optionStr .= "\n: Default: `$defaultValue`\n";
        }

        push(@result, $optionStr);
    }
    return join("\n", @result);
}

sub _getAllowedValuesString
{
    my (@allowedValues) = @_;
    my @processedAllowedValues;
    my $isValueDescriptionPair = 0;
    foreach my $v (@allowedValues)
    {
        if (ref($v) eq 'ARRAY')
        {
            $isValueDescriptionPair = 1;
            my $value = @$v[0];
            my $description = @$v[1];
            push(@processedAllowedValues, "\n: `$value` - $description");
        }
        else
        {
            push(@processedAllowedValues, "`$v`");
        }
    }
    if ($isValueDescriptionPair)
    {
        return join('', @processedAllowedValues);
    }
    return join(', ', @processedAllowedValues);
}

sub getPrerequisites
{
    my (%params) = @_;
    my $result = '';

    my $prerequisites = $params{prerequisites};
    if ($prerequisites)
    {
        $result .= "\n**Prerequisites**: $prerequisites\n";
    }
    return $result;
}

sub _writeLine
{
    my ($array, $line) = @_;
    if ($line)
    {
        push(@$array, $line);
    }
}

1;
