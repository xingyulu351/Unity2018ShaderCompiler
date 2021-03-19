package Coverage::Coverage;
use File::Spec;
use lib ('.');
use Coverage::NoCoverage;
use Coverage::BullsEyeCoverage;

sub init
{
    if ($ENV{'BULLSEYEBIN'})
    {
        return Coverage::BullsEyeCoverage::new();
    }
    else
    {
        return Coverage::NoCoverage::new();
    }
}

1;
