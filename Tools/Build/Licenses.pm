package Licenses;

#usage:
# perl -MLicenses=BuildLicensesFile -e 'BuildLicensesFile("legal.txt")'
# perl -MLicenses=BuildLicensesHTMLFile -e 'BuildLicensesHTMLFile("legal.html")'
# perl -MLicenses=VerifyExternalLicenses -e 'VerifyExternalLicenses()'
# perl -MLicenses=VerifyExternalLicenses -e 'VerifyExternalLicenses(undef, "report.csv")'

use warnings;
use strict;
use File::Basename qw (dirname basename fileparse);
use File::Spec;
use lib File::Spec->rel2abs(dirname($0) . "/../..") . '/External/Perl/lib';
BEGIN { $ENV{PERL_JSON_BACKEND} = 0 }
use JSON;
use Data::Dumper;
use File::chdir;
use Encode;

our @ISA = qw (Exporter);

our @EXPORT_OK = qw (BuildLicensesFile BuildLicensesHTMLFile VerifyExternalLicenses ExportExternalLicenses);
my $root = File::Spec->rel2abs(dirname(__FILE__)) . '/../..';

my $license_list;    # Gets loaded once

my $crlf = $^O eq 'MSWin32' ? "\n" : "\r\n";

# read JSON file, and return the resultin object, or undef on fail
sub ReadJSONFile
{
    my $filename = shift;

    open(F, "<$filename") or do
    {

        #warn ("Failed to open $filename\n");
        return undef;
    };
    local $/;
    my $filedata = <F>;
    close F;

    my $json = JSON->new;
    $json->relaxed([1]);
    $json->allow_singlequote([1]);
    $json->allow_barekey([1]);

    my $data;
    eval { $data = $json->decode($filedata); };
    if ($@)
    {
        print("Failed to read $filename with the following error:\n$@\n");
        return undef;
    }
    return $data;
}

sub prioritize
{
    my $val = shift;

    if ($val =~ /Licenses\/THIRD PARTY NOTICES/)
    {
        $val = '!' . $val;
    }
    elsif ($val =~ /Appendix A:/)
    {
        $val = 'zzzz' . $val;
    }
    elsif ($val =~ /Licenses\// && $val !~ /NodeModules/)
    {
        $val = 'zzzzzzzz' . $val;
    }
    return $val;
}

# Find unity_license.json file in the specified dir, append the path to listref, then recurse into subfolders
sub FindLicenses
{
    my $dir = shift;
    my $listref = shift;

    if (-f "$dir/unity_license.json")
    {
        my $json = ReadJSONFile("$dir/unity_license.json");
        if (!$json || $json->{ignore})
        {
            return;
        }

        my $out = $dir;
        $out =~ s/\Q$root\/External\/\E//;
        push @{$listref}, $out;
    }

    opendir(my $dh, $dir) or die("Can't open $dir\n");
    my @dirs = grep { /^[^\.]/ && -d "$dir/$_" } readdir($dh);
    closedir($dh);

    for my $d (@dirs)
    {
        FindLicenses("$dir/$d", $listref);
    }
}

# Returns a list of unique licenses from the list of license objects, using
# 'license' field for comparison.
sub uniq
{
    my %seen;
    return grep { !$seen{ $_->{license} }++ } @_;
}

# Get list of all unity_license.json files in the proper order.
sub GetOrder
{
    my $order = ReadJSONFile("$root/External/order.json");
    if (!$order)
    {
        $order = { order => [] };
        my @list = ();
        FindLicenses("$root/External", \@list);
        @list = sort ({ return prioritize($a) cmp prioritize($b); } @list);

        $order->{order} = \@list;
    }
    return $order;
}

# Load all licenses, return as a list
sub LoadAllLicenses
{
    my @list = ();
    my $order = GetOrder();
    foreach my $path (@{ $order->{order} })
    {
        my $json = ReadJSONFile("$root/External/$path/unity_license.json");
        push @list, $json;
    }
    return \@list;
}

# Returns a list of properly filtered licenses, for producing the final license
# file
sub GetPlainUniqLicenseList
{

    # License files in External may contain duplicates, because some of the
    # modules are duplicate in the source tree.
    # Therefore, make a list of unique licenses first, then build the result.
    if (!$license_list)
    {
        $license_list = LoadAllLicenses();
    }
    my @plain_list = ();
    foreach my $json (@{$license_list})
    {
        foreach my $lic (@{ $json->{licenses} })
        {
            if (exists($lic->{license}) && $lic->{license} && $lic->{license} ne '' && !$lic->{exclude})
            {
                push @plain_list, $lic;
            }
        }
    }

    my @uniq_licenses = uniq(@plain_list);
    return @uniq_licenses;
}

# Build legal.txt file, write to the specified filename
sub BuildLicensesFile
{
    my $fname = shift;

    my @uniq_licenses = GetPlainUniqLicenseList();

    my $licenses = '';
    foreach my $lic (@uniq_licenses)
    {
        my $text;
        eval { $text = decode('utf8', $lic->{license}); };
        if ($@)
        {
            die "Error: Non UTF8 JSON detected in '$lic->{name}'\n";
        }
        $licenses .= $text;
        if ($text !~ /\n$/)
        {
            $licenses .= "\n";
        }
        $licenses .= "++++++++++++++++++++++++++++++\n";
    }
    open(F, '>:encoding(utf8)', $fname) or die("Can't open $fname\n");
    print(F $licenses . "\n");
    close(F);
    return 0;
}

# build legal.html file, write to the specified filename
sub BuildLicensesHTMLFile
{
    my $fname = shift;
    my @uniq_licenses = GetPlainUniqLicenseList();
    my $licenses = '';
    foreach my $lic (@uniq_licenses)
    {
        my $text = $lic->{license};

        $text =~ s/\n/<br>/g;
        $licenses .= "<h2>$lic->{name}</h2>\n";
        $licenses .= "<p>$text</p>\n";
        if ($text !~ /\n$/)
        {
            $licenses .= "\n";
        }
    }
    open(F, ">$fname") or die("Can't open $fname\n");
    print F "<html><head></head><body>\n";
    print(F $licenses . "\n");
    print F "</body></html>\n";
    close F;
}

my @ignore_dirs = ('nodejs', 'AssetStore', 'CacheServer', 'DirectX', 'Resources', 'Packages', 'Windows10', 'NuGet',);

# Verify the license correctness in the specified json file
sub VerifyLicense
{
    my $json = shift;
    my $fname = shift;
    my $shortname = substr($fname, length($root . '/External/'));
    if (!$json->{licenses} || !scalar(@{ $json->{licenses} }))
    {
        print("$shortname : No licenses defined\n");
        return -1;
    }
    for my $l (@{ $json->{licenses} })
    {
        if ($l->{exclude})
        {
            next;    # do not report excluded from build licenses in the verification
        }

        if (!$l->{name})
        {
            print("$shortname : License name is missing\n");
            return -1;
        }

        if (!$l->{license})
        {
            print("$shortname : License text is missing\n");
            return -1;
        }

        if (!-d "$root/External/$l->{name}")
        {
            print("$shortname : License name $l->{name} is not pointing to an existing directory in External\n");
            return -1;
        }
    }
    return 0;
}

# Report generation data
# + prefix means that the field is boolean
my @idxmap = (
    'name', 'formal_name', 'version', 'modified',
    '+in_legal_txt', 'legal_txt_note', 'source_made_available', 'analysis_status',
    'notes', 'action', 'oss_licenses', '+in_editor_about',
    'purpose', '+used_for_build', '+used_in_editor', '+used_in_desktop_players',
    '+used_in_mobile_players', '+has_sources', '+has_binaries', 'owner',
    'unity_team', '+redistribution_allowed',
);

sub ReportWriteMissing
{
    my $rf = shift;
    my $name = shift;
    print $rf $name;
    for (my $i = 0; $i < scalar(@idxmap); $i++)
    {
        print $rf ',' unless $i == scalar(@idxmap) - 1;
    }
    print $rf $crlf;
}

sub ReportWritePresent
{
    my $rf = shift;
    my $l = shift;
    if ($l->{license} && !$l->{exclude})
    {
        $l->{in_legal_txt} = 1;
    }
    for (my $i = 0; $i < scalar(@idxmap); $i++)
    {
        my $val = get_csv_value_for_idx($l, $i);
        print $rf $val;
        print $rf ',' unless $i == scalar(@idxmap) - 1;
    }
    print $rf $crlf;
}

# Find all node.js modules recursively, and verify their licenses
sub VerifyExternalNodeModulesLicenses
{
    my $dir = shift;
    my $rf = shift;    # filehandle for report writing
    my $toplevel;
    if (!$dir)
    {
        $dir = "$root/External";
        $toplevel = 1;
    }

    if (-f "$dir/unity_license.json")
    {
        my $json = ReadJSONFile("$dir/unity_license.json");
        if (!$json || $json->{ignore})
        {
            return;
        }
    }

    my $found_missing;
    opendir(my $dh, $dir) or die("Can't open $dir\n");
    my @dirs = grep { /^[^\.]/ && -d "$dir/$_" } readdir($dh);
    closedir($dh);
    for my $d (@dirs)
    {
        if ($toplevel && grep ({ $_ eq $d } @ignore_dirs))
        {
            next;
        }
        if ($d eq 'builds')
        {
            next;
        }
        my $path = "$dir/$d";
        my $fname = "$path/unity_license.json";
        if (!-e $fname && -e "$path/package.json")
        {
            my $l = CheckLicenseName($path);
            if (!$l)
            {
                my $shortpath = substr($path, length($root . "/External/"));
                print("$shortpath : License for node_module missing\n");

                # write missing module to report
                if ($rf)
                {
                    ReportWriteMissing($rf, $shortpath);
                }
            }
            else
            {
                # write node module to report
                if ($rf)
                {
                    ReportWritePresent($rf, $l);
                }
            }
        }
        VerifyExternalNodeModulesLicenses($path, $rf);
    }
}

# returns license ref if the license name is present in the license_list
sub CheckLicenseName
{
    my $path = shift;
    my $list = $license_list;

    # unity_license.json is not present at the module path,
    # but may be specified elsewhere in another file.
    my $found = 0;
    my $relpath = substr($path, length($root . "/External/"));
    for my $l (@{$list})
    {
        next if (!exists($l->{licenses}));
        for my $ll (@{ $l->{licenses} })
        {
            if ($ll->{name} eq $relpath)
            {
                return $ll;
            }
        }
    }
    return undef;
}

sub get_csv_value_for_idx
{
    my ($data, $idx) = @_;

    my $key = $idxmap[$idx];
    my $is_bool = 0;

    if (substr($key, 0, 1) eq '+')
    {
        $key = substr($key, 1);
        $is_bool = 1;
    }

    return '' unless exists $data->{$key};

    if ($is_bool)
    {
        return $data->{$key} ? 'X' : '';
    }

    my $val = $data->{$key};
    $val =~ s/"/""/g;

    if ($val =~ /[,\n]+/)
    {
        return "\"$val\"";
    }
    return $val;
}

# Verify all external licenses recursively at the specified path
sub VerifyExternalLicenses
{
    my $dir = shift;
    my $report_fname = shift;
    my $report_fhandle = shift;

    my $toplevel;
    if (!$dir)
    {
        $dir = "$root/External";
        $toplevel = 1;
    }
    if (!$license_list)
    {
        $license_list = LoadAllLicenses();
    }

    my $rf;    # report file handle

    if ($toplevel && $report_fname)
    {
        open $rf, '>:encoding(utf-8)', $report_fname or die "Failed to open file: $report_fname\n";

        print $rf
            "Directory Path (please don't change),Formal name of 3rd party code,Version,Modified,In legal.txt,Legal.txt note,Source made available?,Analysis (R/Y/G),Notes,Action,Open Source Licenses,In Editor About,Purpose,Used for build,Editor,Desktop Players,Mobile Players,Has Sources,Has Binaries,Owner,Unity Team,Redistribution Allowed$crlf";
    }
    elsif ($report_fhandle)
    {
        $rf = $report_fhandle;
    }

    #print "scanning $dir...\n";
    my $found_missing;
    opendir(my $dh, $dir) or die("Can't open $dir\n");
    my @dirs = grep { /^[^\.]/ && -d "$dir/$_" } readdir($dh);
    closedir($dh);
    for my $d (@dirs)
    {
        if ($toplevel && grep ({ $_ eq $d } @ignore_dirs))
        {
            next;
        }
        if (-e "$dir/$d/package.json")
        {
            # skip node modules -- will get checked in the next pass
            next;
        }
        if ($d eq 'builds')
        {
            next;
        }
        my $path = "$dir/$d";
        my $fname = "$path/unity_license.json";
        if (!-e $fname)
        {
            # Check if the license is defined in another unity_license.json file
            my $l = CheckLicenseName($path);

            my $shortpath = substr($path, length($root . "/External/"));
            if (!$l)
            {
                print("$shortpath : unity_license.json missing\n");
                $found_missing = 1;

                # write missing info to report
                if ($rf)
                {
                    ReportWriteMissing($rf, $shortpath);
                }
            }
            else
            {
                # write present license info to report
                next;
                if ($rf)
                {
                    ReportWritePresent($rf, $l);
                }
            }

            next;
        }

        my $json = ReadJSONFile($fname) or die("Failed to read $fname\n");
        if ($json->{recurse})
        {
            #print "recursing into $path\n";
            if (VerifyExternalLicenses($path, undef, $rf))
            {
                $found_missing = 1;
                next;
            }
        }
        else
        {

            if ($rf)
            {
                for my $l (@{ $json->{licenses} })
                {
                    ReportWritePresent($rf, $l);
                }
            }
            if (VerifyLicense($json, $fname))
            {
                $found_missing = 1;
                next;
            }
        }
    }

    if ($found_missing && $toplevel)
    {
        print("3rd party notice coverage: some 3rd party notices are missing or incomplete.\n");
    }

    if ($toplevel)
    {
        # 2nd pass, find node modules
        VerifyExternalNodeModulesLicenses(undef, $rf);
    }

    if ($toplevel && $report_fname)
    {
        close $rf;
    }

    return $found_missing;
}

sub ExportExternalLicenses
{
    my $out_path = shift;

    local $CWD = $root;
    system('zip', '--symlinks', '-i', 'External/**/unity_license.json', '-r', $out_path, '.') && die("Failed to zip External/**/unity_license.json\n");
}

1;
