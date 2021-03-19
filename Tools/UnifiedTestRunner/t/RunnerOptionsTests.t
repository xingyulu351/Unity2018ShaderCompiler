use warnings;
use strict;

use Test::More;

use Dirs;
use lib Dirs::external_root();

use Test::Deep;

use Option;

BEGIN { use_ok('RunnerOptions'); }

can_ok('RunnerOptions', 'read');

read_NoOptions_ReturnsEmptyOption:
{
    my @options = RunnerOptions::read('[]');
    is_deeply(\@options, []);
}

read_OneStringOption_ReturnsEmptyOption:
{
    my $json = <<END_JSON;
[
    {
        "names": [
            "api_level"
        ],
        "description": ".NET framework API level",
        "valueType": 2,
        "defaultValue": "NET_2_0_Subset",
        "allowedValues": [
            "v1",
            "v2"
        ],
        "hidden":true
    }
]
END_JSON

    my @options = RunnerOptions::read($json);

    my $expected = new Option(
        names => ['api_level'],
        description => '.NET framework API level',
        valueType => Option->ValueTypeRequired,
        typeSpec => 's',
        allowedValues => ['v1', 'v2'],
        defaultValue => 'NET_2_0_Subset',
        hidden => 1
    );

    is_deeply([$expected], \@options);
    isa($options[0], 'Option');
}

read_OneOptionWichAllowsMultipleValues_ReturnsExpectedOption:
{
    my $json = <<END_JSON;
[
    {
        "names": [
            "api_level"
        ],
        "valueType": 2,
        "acceptsMultipleValues":true
    }
]
END_JSON

    my @options = RunnerOptions::read($json);

    my $expected = new Option(
        names => ['api_level'],
        valueType => Option->ValueTypeRequired,
        typeSpec => 's@',
    );

    is_deeply(\@options, [$expected]);
}

done_testing();
