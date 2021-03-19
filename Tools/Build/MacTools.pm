package MacTools;

use warnings;
use strict;
use Carp qw(croak carp);
use File::Find;
use File::Path qw(mkpath rmtree);
use File::Basename qw(dirname basename );
use File::Spec;

my $modulePath;

BEGIN
{
    $modulePath = File::Spec->rel2abs(dirname(__FILE__)) . '/../../External/Perl/lib';
}
use lib "$modulePath/../../External/Perl/lib";
use OTEE::BuildUtils qw(copy svnGetRevision);
use Tools qw (UCopy ReplaceText ReadFile);

require Exporter;
our @ISA = qw(Exporter);
our @EXPORT_OK =
    qw( PostbuildCleanup BuildPackage BuildPackageNoRelocate BuildInstallerFromPackages CodeSignBundle NotarizeApp SignPackage Cleanup BuildImage BuildDmg StripEditor LinkFileOrDirectory LinkFlattenedDirs SetCustomFileIcon BuildAndPatchComponentPlist BuildMacProductInstaller SetDeviceIdEnvironmentVariableIfNotSet);

use lib "$modulePath/../../Configuration";
use BuildConfig;
our $unityVersion = $BuildConfig::unityVersion;
our $copyright = $BuildConfig::installer_copyright;
our $isBetaBuild = $BuildConfig::unityIsBeta;

sub Cleanup
{

    # Cleanup Xcode build cache folder
    rmtree("Projects/build");
}

# When using this function the cwd must be relative
sub BuildImage($$$$$;$)
{
    my ($pkgName, $dmg_name, $srcPkgPath, $targetPath, $root, $template) = @_;

    my $dmgsource = "disk_image_source_$dmg_name";

    mkdir $dmgsource;
    system('mv', "$srcPkgPath", "$dmgsource/$pkgName");
    $? == 0 or die "Could not move mpkg into disk image\n";
    ##############FIXUP PERMISSIONS CORRECTLY
    system("chmod -R ugo+rw $dmgsource");
    $? == 0 or croak "Failed";

    BuildDmg($dmgsource, $dmg_name, $template);

    system("hdiutil internet-enable -yes $dmg_name.dmg");
    $? == 0 or croak "Failed";
    system("mv $dmg_name.dmg $targetPath/$dmg_name.dmg");
    $? == 0 or croak "Failed";
}

sub BuildDmg($$;$)
{
    my ($file, $volname, $templateDmg) = @_;
    my $target = "$volname.dmg";
    my $tempdmg;

    system("killall hdiejectd");
    sleep 1;

    # for insane log output from hdiutil use -debug and -verbose

    if ($templateDmg eq '')
    {
        print "* Creating DMG called $volname as $target containing $file\n";
        $tempdmg = "$volname-$$.sparsebundle";
        system("hdiutil", "create", "-ov", "-fs", "HFS+", "-volname", $volname, "-srcfolder", $file, "$tempdmg");
        $? == 0 or die "hdiutil create error: $!\n";
        print "* Converting the disk image to distro format\n";
    }
    else
    {
        print "* Creating DMG called $volname as $target containing $file from template $templateDmg\n";
        $tempdmg = "$volname-$$.sparsebundle";
        my $mountPath = "build/temp/MountTemp";
        UCopy($templateDmg, $tempdmg);
        system("hdiutil", "resize", "-size", "10g", $tempdmg);
        $? == 0 or die "hdiutil resize error: $!\n";

        for (my $n = 1; $n <= 10; $n++)
        {
            system("hdiutil", "attach", $tempdmg, "-mountpoint", $mountPath) or last;

            warn("hdiutil attach error: $!\n");
            sleep(5) unless $n == 10;
        }
        $? == 0 or die("hdiutil attach is still failing after multiple attempts\n");

        UCopy("$file/*", $mountPath);

        for (my $n = 1; $n <= 10; $n++)
        {
            system("hdiutil", "detach", $mountPath);
            if ($? == 0)
            {
                last;
            }
            print("hdiutil detach failed, waiting before retry $n/10...\n");
            sleep(5);
        }
        $? == 0 or die "hdiutil detach error: $!\n";
        system("hdiutil", "compact", $tempdmg);
        $? == 0 or die "hdiutil compact error: $!\n";
    }

    print "* Converting the disk image to distro format\n";
    system("hdiutil", "convert", "-format", "UDZO", "-imagekey", "zlib-level=9", $tempdmg, "-o", "$target");
    $? == 0 or die "hdiutil convert error: $!\n";
    print "* Removing temporary image\n";
    rmtree($tempdmg);
}

sub BuildPackageNoRelocate
{
    my ($packageName, $sourceDir, $targetPath, $scriptsPath, $identifier) = @_;

    my $componentplist = "/tmp/temp_component-plist.plist";

    system("pkgbuild --analyze --root '$sourceDir' $componentplist");

    my $index = 0;
    while (system("/usr/libexec/PlistBuddy -c 'Set :$index:BundleIsRelocatable bool false' $componentplist 2>/dev/null") == 0)
    {
        $index++;
    }

    BuildPackage($packageName, $sourceDir, $targetPath, $scriptsPath, $identifier, $componentplist);
}

sub BuildPackage($$$;$$$)
{
    my ($packageName, $sourceDir, $targetPath, $scriptsPath, $identifier, $componentplist) = @_;

    PostbuildCleanup($sourceDir);

    if (defined $identifier)
    {
        my $args = "--root '$sourceDir' --install-location '$targetPath'  --scripts '$scriptsPath' --identifier $identifier";
        $args .= " --component-plist '$componentplist'" if (defined $componentplist);
        system("pkgbuild $args '$packageName'");
    }
    else
    {
        if (defined $scriptsPath)
        {
            system("pkgbuild --root '$sourceDir' --install-location '$targetPath' --scripts '$scriptsPath' '$packageName'");
        }
        else
        {
            system("pkgbuild --root '$sourceDir' --install-location '$targetPath' '$packageName'");
        }
    }
    $? == 0 or die "pkgbuild failed\n";
}

sub SetCustomFileIcon ($$)
{
    my ($file, $icon) = @_;
    my $tempr = "temp.r";

    # Set the custom icon flag on the file
    system("SetFile", "-a", "C", "$file");

    # Remove file extension from default Finder display.
    system("SetFile", "-a", "E", "$file");
    $? == 0 or die "SetFile error: $!\n";

    # Create a .r source file for the icon data for Rez
    system("echo \"read 'icns' (-16455) \\\"$icon\\\";\" > $tempr");

    # Run Rez to add the icon data to the resource fork of the target file
    system("Rez", "-append", "$tempr", "-o", $file);
    $? == 0 or die "Rez error: $!\n";

    system("rm", "-f", "$tempr");
}

sub SignPackage
{
    my $targetPath = shift;

    if ($ENV{MAC_INSTALLER_SIGNING_CERTIFICATE})
    {
        my $retryCount = 0;

        # We often see this failing due to connectivity issues with the timestamp server, so retry several times.
        while (system("productsign --sign '$ENV{MAC_INSTALLER_SIGNING_CERTIFICATE}' '$targetPath' '$targetPath.signed'"))
        {
            print("Signing (productsign) failed. Retrying...\n");
            if ($retryCount++ == 20)
            {
                die("Gave up signing.\n");
            }
            sleep 1;
        }

        system("rm '$targetPath'");
        system("mv '$targetPath.signed' '$targetPath'");
    }
    else
    {
        print("Not signing $targetPath because certificate is not available\n");
    }
}

sub BuildInstallerFromPackages
{
    my $packageTmpDir = "/tmp/installerpackages";
    system("rm -r $packageTmpDir");
    system("mkdir $packageTmpDir");

    my $targetPath = shift(@_);
    my $sourceFiles = shift(@_);

    return BuildMacProductInstaller(targetPath => $targetPath, sourceFiles => $sourceFiles, packageList => \@_);
}

sub BuildMacProductInstaller
{
    my %args = @_;

    my $targetPath = $args{targetPath};
    my $sourceFiles = $args{sourceFiles};
    my $packageList = $args{packageList};

    my $plugins = "";
    if (exists($args{pluginsPath}))
    {
        $plugins = "--plugins $args{pluginsPath}";
    }

    my $packageTmpDir = "/tmp/installerpackages";
    rmtree($packageTmpDir);
    mkpath($packageTmpDir);

    my $packages = "";
    foreach (@{$packageList})
    {
        UCopy($_, $packageTmpDir);
        $packages = $packages . " --package '$_'";
    }

    if (!-e "$sourceFiles/distribution.plist")
    {
        system("productbuild $plugins --synthesize $packages '$sourceFiles/distribution.plist'");
    }
    my $cmd =
        "productbuild $plugins --version $unityVersion --distribution '$sourceFiles/distribution.plist' --resources '$sourceFiles/Resources' --package-path '$packageTmpDir' '$targetPath'";

    # this print is very important for tracking installer problems
    print("$cmd\n");
    system($cmd);

    if ($ENV{MAC_INSTALLER_SIGNING_CERTIFICATE})
    {
        my $retryCount = 0;

        # We often see this failing due to connectivity issues with the timestamp server, so retry several times.
        $cmd = "productsign --sign '$ENV{MAC_INSTALLER_SIGNING_CERTIFICATE}' '$targetPath' '$targetPath.signed'";
        print("$cmd\n");
        while (system($cmd))
        {
            print("Signing (productsign) failed. Retrying...\n");
            if ($retryCount++ == 20)
            {
                die("Gave up signing.\n");
            }
            sleep 1;
        }

        system("rm '$targetPath'");
        system("mv '$targetPath.signed' '$targetPath'");
    }

    die "Could not create $targetPath package.\n" if $?;
}

sub CodeSignBundle($$)
{
    my ($bundlePath, $enableHardenedRuntime) = @_;
    print("CodeSignBundle: $bundlePath\n");
    if ($ENV{MAC_APP_SIGNING_CERTIFICATE})
    {
        print("Code signing '$bundlePath'\n");

        my $osxvers = `sw_vers`;
        print("sw_vers:\n$osxvers\n");

        my $runtime = '';
        if ($enableHardenedRuntime)
        {
            ($osxvers =~ /^ProductVersion:\s*\d+\.(\d+)\.(\d+).*$/m) or die("Did not understand sw_vers");
            if ($1 >= 14)    # hardened runtime option available in 10.14+
            {
                $runtime = '--options runtime';
            }
            else
            {
                warn "Signing with Hardened Runtime is not available in this macOS version.\n";
            }
        }

        # use --deep option on osx 10.9.5+
        # use --force to force signing dylib files
        my $deep = '--force --deep';

        my $retryCount = 0;
        my $cmd = "codesign $deep $runtime -vvv -s '$ENV{MAC_APP_SIGNING_CERTIFICATE}' '$bundlePath'";
        print "running $cmd\n";

        # We often see this failing due to connectivity issues with the timestamp server, so retry several times.
        while (system($cmd))
        {
            print("Signing (codesign) failed. Retrying...\n");
            if ($retryCount++ == 20)
            {
                die("Gave up signing.\n");
            }
            sleep 1;
        }
    }
    else
    {
        warn "MAC_APP_SIGNING_CERTIFICATE is not set, signing is not possible.\n";
    }
}

sub NotarizeApp($$)
{
    my ($file, $bundleId) = @_;
    print("NotarizeApp: $file\n");

    my $username = $ENV{MAC_NOTARIZE_USERNAME} or die "MAC_NOTARIZE_USERNAME is not set, notarization is not possible.\n";
    my $password = $ENV{MAC_NOTARIZE_PASSWORD} or die "MAC_NOTARIZE_PASSWORD is not set, notarization is not possible.\n";

    my $ascProvider = $ENV{MAC_NOTARIZE_ASC_PROVIDER} ? "--asc-provider $ENV{MAC_NOTARIZE_ASC_PROVIDER}" : "";

    my $cmd = "xcrun altool -u $username -p \"$password\" --notarize-app -f \"$file\" --primary-bundle-id \"$bundleId\" $ascProvider";

    print "running $cmd\n";
    my $output = `$cmd 2>&1`;

    print "altool output: $output\n";
    if ($?)
    {
        croak "altool notarize-app failed";
    }

    my $requestUUID;
    if ($output =~ /^\s*RequestUUID\s*=\s*(.+)$/m)
    {
        $requestUUID = $1;
    }
    else
    {
        croak "failed to obtain RequestUUID";
    }

    # check status
    my $numAttempts = 100;
    for (my $i = 1; $i <= $numAttempts; $i++)
    {
        print "waiting 30s before checking notarization status\n";
        sleep(30);

        $cmd = "xcrun altool -u $username -p \"$password\" --notarization-info $requestUUID";
        print "attempt $i of $numAttempts\n";
        print "running $cmd\n";
        $output = `$cmd 2>&1`;
        print "altool output: $output";

        if ($?)
        {
            warn "altool notarization-info failed with code $?";
        }
        else
        {
            # Interupt if either log ur status code is present.
            # Ignore the actual status value, instead rely on the stapler tool to determine success.

            if ($output =~ /\s*LogFileURL\s*:\s*(.+)$/m)
            {
                if ($1 ne '(null)')
                {
                    print "fetching notarization log file\n";
                    system("curl $1");
                    print "\n";
                    last;
                }
            }

            if ($output =~ /\s*Status Code\s*:\s*(\d+)$/m)
            {
                last;
            }
        }
    }

    # staple the ticket
    $cmd = "xcrun stapler staple \"$file\"";
    print "running $cmd\n";
    if (system($cmd))
    {
        croak "stapler failed";
    }

    print "Notarization successful!\n";
}

sub StripEditor($)
{
    my ($toStripFileName) = @_;
    if (!-f $toStripFileName)
    {
        die("Strip: file not found: $toStripFileName\n");
    }
    if ($isBetaBuild == 0)
    {
        system("strip", "-x", $toStripFileName);
        $? == 0 or die "Failed to strip $toStripFileName\n";
    }
}

sub LinkFileOrDirectory($$)
{
    my ($src, $dst) = @_;

    # Enable verbose to see exactly what directories are created and what files are linked
    my $verbose = '';    #"-v";

    if (-d $src)
    {

        # If the source directory is being sent to another directory which does not exist, then assume the target is a renamed destination
        if (-d $dst)
        {
            my $basename = basename($src);
            $dst .= "/$basename";
            if (!-d $dst)
            {
                mkdir($dst, 0777) or croak "Could not create destination path $dst: $!\n";
            }
        }

        my (@dirs, @files, @symlinks);

        # Find out what's there
        find(
            {
                wanted => sub
                {
                    return if $File::Find::dir =~ m#/\.svn(/|$)#;
                    return if $File::Find::name =~ m#/\.DS_Store$#i;
                    my ($local_path) = $File::Find::name =~ m#\Q$src\E(.*)$#;
                    $local_path =~ s#^/+##;
                    if (-l $File::Find::name)
                    {
                        push @symlinks, $local_path;
                    }
                    elsif (-d $File::Find::name)
                    {
                        push @dirs, $local_path;
                    }
                    elsif (-f $File::Find::name)
                    {
                        push @files, $local_path;
                    }
                },
                no_chdir => 1,
            },
            $src
        );

        # Then recreate directory structure
        for my $dir (@dirs)
        {
            if (!-d "$dst/$dir")
            {
                mkdir "$dst/$dir", 0777 or croak "Could not create path $dst/$dir: $!\n";
            }
        }

        # Hardlink normal files
        for my $file (@files)
        {
            if (-e "$dst/$file")
            {
                if (!-l "$dst/$file" && -d "$dst/$file")
                {
                    system rm => -rf => "$dst/$file" and croak "Could not clean up destination path $dst/$file before creating hardlink.\n";
                }
                else
                {
                    unlink "$dst/$file" or croak "Could not delete $dst/$file: $!\n";
                }
            }
            link "$src/$file", "$dst/$file" or croak "Could not link $src/$file to $dst/$file: $!\n";
        }

        # Copy symlinks
        for my $symlink (@symlinks)
        {
            my $orig = readlink "$src/$symlink" or croak "Could not read symlink at $src/$symlink: $!";
            if (-e "$dst/$symlink")
            {
                if (!-l "$dst/$symlink" && -d "$dst/$symlink")
                {
                    system rm => -rf => "$dst/$symlink" and croak "Could not clean up destination path $dst/$symlink before creating symlink.\n";
                }
                else
                {
                    unlink "$dst/$symlink" or croak "Could not delete $dst/$symlink: $!\n";
                }
            }
            symlink $orig, "$dst/$symlink" or croak "Could not link $orig to $dst/$symlink: $!\n";
        }
    }
    elsif (-f $src)
    {
        mkpath dirname($dst) if (!-d dirname($dst));
        system("ln -f \Q$src\E \Q$dst\E") && croak("Failed to link $src to $dst");
    }
    else
    {
        croak("$src does not exist\n");
    }
}

sub LinkFlattenedDirs
{

    #my @sources = @_[0..$#_-1];
    #my $target = $_[-1];
    my ($sources, $target, $useSymlink) = @_;

    my $unwanted_re = '/.svn|/CVS|.DS_Store|.nib/?';

    # TODO: These dirs should maybe not be there in the first place?
    my $skip_re = '/Standard Packages|ScriptTemplates|WebPlayerTemplates/?';

    my %originals;
    find(
        {
            wanted => sub
            {
                if ((/\.nib$/ or (not -d $_ and $File::Find::name !~ /$unwanted_re/)) and ($File::Find::name !~ /$skip_re/))
                {
                    if ($File::Find::name =~ m/\/(?:info|objects|classes|keyedobjects)\.nib$/)
                    {

                        # skip inner nibs
                    }
                    else
                    {
                        if (exists $originals{$_})
                        {
                            if (Main::AmRunningOnBuildServer())
                            {
                                die "... name collision, '$File::Find::name' overwrites '$originals{$_}' as '$_' in '$target'\n";
                            }
                            else
                            {
                                warn "... name collision, '$File::Find::name' overwrites '$originals{$_}' as '$_' in '$target'\n";
                            }
                        }
                        $originals{$_} = $File::Find::name;
                    }
                }
            },
            no_chdir => 0
        },
        $sources
    );

    find(
        {
            wanted => sub
            {
                if (not -d $_ and (not exists $originals{$_} or not -l $_))
                {
                    print "Removing '$File::Find::name'\n";
                    unlink $_
                        or die "... failed to unlink $File::Find::name: $!\n";
                }
            },
            no_chdir => 0
        },
        $target
    );

    my $dirs = ($target =~ tr|/|/|) + 1;
    for my $original (sort keys %originals)
    {

        #my $type = $useSymlink ? "Sym" : "Hard";
        #print $type . "Linking '$originals{$original}' to '$target'\n";
        if ($useSymlink)
        {
            system("ln -s \"$originals{$original}\" \"$target\"") && croak("Failed to symlink $originals{$original} to $target");
        }
        else
        {
            LinkFileOrDirectory($originals{$original}, $target);
        }
    }
}

# TODO: Make a generic platform independent cleanup function
sub PostbuildCleanup
{
    my $targetPath = shift;
    system("find \"$targetPath\" -type d -name \".svn\" -print0 | xargs -0 rm -rf");
    system("find \"$targetPath\" -iname \".DS_Store\" -print0 | xargs -0 rm -rf");
}

sub BuildAndPatchComponentPlist
{
    my ($assemblyPath, $comp_plist) = @_;

    system("pkgbuild --analyze --root $assemblyPath $comp_plist") == 0 or die("pkgbuild --analyze failed\n");

    print("Patching the $comp_plist\n");

    my $cont = ReadFile($comp_plist) or die("Failed to read $comp_plist\n");
    $cont =~ s/(<key>BundleIsVersionChecked<\/key>\s*)<true\/>/$1<false\/>/gs;
    $cont =~ s/(<key>BundleIsRelocatable<\/key>\s*)<true\/>/$1<false\/>/gs;

    open(my $plist, ">$comp_plist") or die("Failed to write $comp_plist\n");
    print $plist ($cont);
    close($plist);
}

sub SetDeviceIdEnvironmentVariableIfNotSet
{
    my ($root) = @_;

    if (defined $ENV{'IOS_DEVICE_ID'} && defined $ENV{'TVOS_DEVICE_ID'})
    {
        return;
    }

    my $platformDeviceIds = GetConnectedDeviceIds($root);
    $ENV{'IOS_DEVICE_ID'} = @{ $platformDeviceIds->{iOS} }[0];
    $ENV{'TVOS_DEVICE_ID'} = @{ $platformDeviceIds->{tvOS} }[0];
}

sub GetConnectedDeviceIds
{
    my ($root) = @_;
    my @output = `$root/PlatformDependent/iPhonePlayer/Tools/ios-deploy -c -t 1`;
    my $platformsDeviceIds = {
        iOS => [],
        tvOS => []
    };
    foreach my $line (@output)
    {

        #look at Tools/Build/tests/test-get-ios-devices.t
        my ($name, $deviceId) = $line =~ m/^^[^']*'(?'name'[^']*)'[^\(]\((?'id'[0-9a-fA-F]+)\).*/;

        if (!defined $name)
        {
            next;
        }

        if ($name =~ m/iPhone/ || $name =~ m/iPad/ || $name =~ m/iPod/)
        {
            push(@{ $platformsDeviceIds->{iOS} }, $deviceId);
        }
        elsif ($name =~ m/TV/)
        {
            push(@{ $platformsDeviceIds->{tvOS} }, $deviceId);
        }
    }

    return $platformsDeviceIds;
}

1;
