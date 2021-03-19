package SysInfo;

BEGIN
{
    if ($^O eq "MSWin32")
    {
        require Win32;
        Win32::->import();
        require Win32::API;
        Win32::API::->import();
        require Win32::TieRegistry;
        Win32::TieRegistry::->import();
    }
}
use List::Util 'first';

our $cpu = 'Unknown';
our $cpus = 0;
our $cpufreq = 0;
our $ram = 0;
our $os = 'Unknown';

sub trim { my $s = shift; $s =~ s/^\s+|\s+$//g; return $s }

if ($^O eq "MSWin32")
{
    # 0.09s
    # Note: could use "wmic" to query that stuff, but that is a bit slower (e.g. whole sysinfo.pl script takes 0.38s)
    #
    # OS: 'Windows 10.0.15063'
    # CPU: 'Intel(R) Core(TM) i7-5820K CPU @ 3.30GHz'
    # CPU count: 12
    # CPU freq: 3.3 GHz
    # RAM: 16 GB

    $cpus = $ENV{'NUMBER_OF_PROCESSORS'};

    my $cpuKey = $Registry->Open("LMachine/HARDWARE/DESCRIPTION/System/CentralProcessor/0", { Access => Win32::TieRegistry::KEY_READ(), Delimiter => "/" });
    if ($cpuKey)
    {
        $cpu = $cpuKey->{"/ProcessorNameString"};
        $cpufreq = hex($cpuKey->{"/~MHz"});
        $cpufreq = int(($cpufreq + 50) / 100) * 100;    # round to hundreds of MHz
    }

    my $canUse64bit = sub ()
    {
        eval { my $foo = pack("Q", 1234) };
        return ($@) ? 0 : 1;
    };
    my $use64bit = &$canUse64bit;
    if ($use64bit)
    {
        Win32::API::Struct->typedef(
            MEMORYSTATUSEX => qw{
                DWORD dwLength;
                DWORD MemLoad;
                ULONGLONG TotalPhys;
                ULONGLONG AvailPhys;
                ULONGLONG TotalPage;
                ULONGLONG AvailPage;
                ULONGLONG TotalVirtual;
                ULONGLONG AvailVirtual;
                ULONGLONG AvailExtendedVirtual;
                }
        );

        if (Win32::API->Import('kernel32', 'BOOL GlobalMemoryStatusEx(LPMEMORYSTATUSEX lpMemoryStatusEx)'))
        {
            my $memstatus = Win32::API::Struct->new('MEMORYSTATUSEX');
            $memstatus->{dwLength} = $memstatus->sizeof();
            $memstatus->{MemLoad} = 0;
            $memstatus->{TotalPhys} = 0;
            $memstatus->{AvailPhys} = 0;
            $memstatus->{TotalPage} = 0;
            $memstatus->{AvailPage} = 0;
            $memstatus->{TotalVirtual} = 0;
            $memstatus->{AvailVirtual} = 0;
            $memstatus->{AvailExtendedVirtual} = 0;
            GlobalMemoryStatusEx($memstatus);
            $ram = $memstatus->{TotalPhys};
        }
    }
    else
    {
        my $strRAM = trim(scalar(`wmic computersystem get TotalPhysicalMemory /Value`));
        $ram = $1 if $strRAM =~ /TotalPhysicalMemory=(\d+)/;
    }

    ($osString, $osMajor, $osMinor, $osBuild, $osID) = Win32::GetOSVersion();
    $os = "Windows $osMajor.$osMinor.$osBuild";
}
elsif ($^O eq 'darwin')
{
    # 0.03s
    # OS: 'macOS 10.12.6'
    # CPU: 'Intel(R) Core(TM) i7-4850HQ CPU @ 2.30GHz'
    # CPU count: 8
    # CPU freq: 2.3 GHz
    # RAM: 16 GB

    $os = 'macOS Unknown';
    my $strOS = trim(scalar(`sw_vers`));
    $os = 'macOS ' . $1 if $strOS =~ /ProductVersion:\s*(.+)/;

    $cpu = trim(`sysctl -n machdep.cpu.brand_string`);
    $cpus = trim(`sysctl -n hw.ncpu`);
    $cpufreq = trim(`sysctl -n hw.cpufrequency`) / 1000000.0;
    $ram = trim(`sysctl -n hw.memsize`);
}
else
{
    # 0.03s
    # OS: 'Linux Ubuntu 16.04.2 LTS'
    # CPU: 'Intel(R) Core(TM) i7-5820K CPU @ 3.30GHz'
    # CPU count: 12
    # CPU freq: 3.301 GHz
    # RAM: 16 GB

    open my $h, "/proc/cpuinfo";
    if ($h)
    {
        my @info = <$h>;
        close $h;
        $cpus = scalar(map /^processor/, @info);
        my $strCPU = first { /^model name/ } @info;
        $cpu = $1 if ($strCPU && $strCPU =~ /:\s+(.*)/);
        my $strFreq = first { /^cpu MHz/ } @info;
        $cpufreq = $1 if ($strFreq && $strFreq =~ /:\s+(.*)/);
    }
    open $h, "/proc/meminfo";
    if ($h)
    {
        my @info = <$h>;
        close $h;
        my $strRAM = first { /^MemTotal/ } @info;
        $ram = $1 * 1024 if ($strRAM && $strRAM =~ /:\s+(\d+)/);
    }
    $os = 'Linux Unknown';
    open $h, "/etc/lsb-release";
    if ($h)
    {
        my @info = <$h>;
        close $h;
        my $strOS = first { /^DISTRIB_DESCRIPTION/ } @info;
        $os = 'Linux ' . $1 if ($strOS && $strOS =~ /=\"(.*)\"/);
    }
}
$ram = int($ram / 1024 / 1024 / 1024 + 0.5);
$cpufreq = int($cpufreq) / 1000;

# print "OS: '$os'\n";
# print "CPU: '$cpu'\n";
# print "CPU count: $cpus\n";
# print "CPU freq: $cpufreq GHz\n";
# print "RAM: $ram GB\n";

1;
