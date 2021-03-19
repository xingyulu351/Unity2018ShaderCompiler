use warnings;
use strict;
use Test::More;
use File::Basename qw(dirname);
use File::Spec;
use lib File::Spec->rel2abs (dirname($0) . "../../");

our $mock_qx       = sub { return RetunMockedStdOutput(); };
use_ok ('MacTools');

sub RetunMockedStdOutput
{
	return (
		"Random stuff that should not be included",
		"[....] Found J85AP 'iPad Mini 2 9.3.1' (1990d6a555f5f0ccd81d9e5547b33c4f91d53a7d) connected through USB.",
		"'iPod' (2990d6a555f5f0ccd81d9e5547b33c4f91d53a7d)",
		"whaaaat",
		"'TVos' (3990d6a555f5f0ccd81d9e5547b33c4f91d53a7d)",
		"'iPhone' (4990d6a555f5f0ccd81d9e5547b33c4f91d53a7d)"
	);
}

BEGIN {
	 *CORE::GLOBAL::readpipe   = sub { $mock_qx->(@_); };
}

GetConnectedDeviceIds_WhenGettingiOS_Then3Hits: {
	my $results = MacTools::GetConnectedDeviceIds ('rootPath', 'iOS');

	is (scalar(@{$results->{iOS}}), 3);
}

GetConnectedDeviceIds_WhenGettingiOS_ThenDontContaintvOSId: {
	my $results = MacTools::GetConnectedDeviceIds ('rootPath', 'iOS');

	is (@{$results->{iOS}}[0], '1990d6a555f5f0ccd81d9e5547b33c4f91d53a7d');
	is (@{$results->{iOS}}[1], '2990d6a555f5f0ccd81d9e5547b33c4f91d53a7d');
	is (@{$results->{iOS}}[2], '4990d6a555f5f0ccd81d9e5547b33c4f91d53a7d');
}

GetConnectedDeviceIds_WhenGettingtvOS_ThenCorrectIdReturned: {
	my $results = MacTools::GetConnectedDeviceIds ('rootPath', 'tvOS');

	is (scalar(@{$results->{tvOS}}), 1);
	is (@{$results->{tvOS}}[0], '3990d6a555f5f0ccd81d9e5547b33c4f91d53a7d');
}

done_testing ();