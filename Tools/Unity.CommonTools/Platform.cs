using System;
using System.Runtime.InteropServices;

namespace Unity.CommonTools
{
    public class Platform
    {
        static bool _isOSX;
        static bool _checkedOSX;

        [DllImport("libc")]
        static extern int uname(IntPtr buf);

        //From Managed.Windows.Forms/XplatUI
        static bool IsRunningOnMac()
        {
            if (_checkedOSX)
                return _isOSX;

            var buf = IntPtr.Zero;
            try
            {
                buf = Marshal.AllocHGlobal(8192);
                // This is a hacktastic way of getting sysname from uname ()
                if (uname(buf) == 0)
                {
                    var os = Marshal.PtrToStringAnsi(buf);
                    if (os == "Darwin")
                        return (_isOSX = true);
                }
            }
            catch
            {
            }
            finally
            {
                _checkedOSX = true;
                if (buf != IntPtr.Zero)
                    Marshal.FreeHGlobal(buf);
            }
            return false;
        }

        public static bool IsLinux
        {
            get { return Environment.OSVersion.Platform == PlatformID.Unix && !IsOSX; }
        }

        public static bool IsOSX
        {
            get { return !IsWindows && IsRunningOnMac(); }
        }

        public static bool IsWindows
        {
            get
            {
                switch (Environment.OSVersion.Platform)
                {
                    case PlatformID.Win32Windows:
                    case PlatformID.Win32NT:
                    case PlatformID.Win32S:
                        return true;
                    default:
                        return false;
                }
            }
        }
    }
}
