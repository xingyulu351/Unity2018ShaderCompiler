use warnings;
use strict;

use Option;

package OptionsHelpPrinter;

sub getText
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

        my $str = _getNamesString($o->getNames());
        $str .= _getValueTypeString($o->getValueType());

        my $defaultValue = $o->getDefaultValue();
        if ($defaultValue)
        {
            my $isArray = ref($defaultValue) eq 'ARRAY';
            if ($isArray)
            {
                $str .= " default: " . join(',', @{$defaultValue});
            }
            else
            {
                $str .= " default: $defaultValue";
            }
        }

        my $description = $o->getDescription();
        if (not $description)
        {
            $description = '';
        }
        $str .= "\n    $description";
        my @allowedValues = $o->getAllowedValues();
        if (scalar(@allowedValues) > 0)
        {
            if (not $description eq '')
            {
                $str .= ' ';
            }
            $str .= _getAllowedValuesString(@allowedValues);
        }

        push(@result, $str);
    }
    return join("\n", @result);
}

sub _getNamesString
{
    my (@names) = @_;
    my @namesWithDashes;
    foreach my $n (@names)
    {
        if (length($n) > 1)
        {
            push(@namesWithDashes, "--$n");
            next;
        }
        push(@namesWithDashes, "-$n");
    }
    return join(', ', @namesWithDashes);
}

sub _getValueTypeString
{
    my ($valueType) = @_;
    if ($valueType == Option::ValueTypeRequired)
    {
        return "=value";
    }

    if ($valueType == Option::ValueTypeOptional)
    {
        return '[=value]';
    }

    return '';
}

sub _getAllowedValuesString
{
    my (@allowedValues) = @_;
    my @processedAllowedValues;
    foreach my $v (@allowedValues)
    {
        if (ref($v) eq 'ARRAY')
        {
            my $value = @$v[0];
            my $description = @$v[1];
            push(@processedAllowedValues, "$value - $description");
        }
        else
        {
            push(@processedAllowedValues, $v);
        }
    }

    return "Values: " . join(', ', @processedAllowedValues);
}

1;
