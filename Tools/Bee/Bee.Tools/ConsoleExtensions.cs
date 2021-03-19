using System;
using System.Runtime.InteropServices;
using System.Security.Cryptography.X509Certificates;

namespace Unity.BuildTools
{
    public static class ConsoleColors
    {
        public const ConsoleColor Success = ConsoleColor.Green;
        public const ConsoleColor Warning = ConsoleColor.Yellow;
        public const ConsoleColor Error = ConsoleColor.Red;

        public static string EnvironmentVariableNameIndicatingCanHandleColors => "DOWNSTREAM_STDOUT_CONSUMER_SUPPORTS_COLOR";

        public static bool ShouldWeOutputColors { get; } = DetermineCanHandleAnsiColors();

        static bool DetermineCanHandleAnsiColors()
        {
            var s = Environment.GetEnvironmentVariable(EnvironmentVariableNameIndicatingCanHandleColors);
            if (s != null)
            {
                if (s[0] == '1')
                    return true;
                if (s[0] == '0')
                    return false;
                throw new ArgumentException("Unknown value: " + s);
            }

            if (HostPlatform.IsOSX)
            {
                var sysIsatty = sys_isatty(1);
                return sysIsatty != 0;
            }

            return Environment.UserInteractive;
        }

        [DllImport("libc", EntryPoint = "isatty")]
        private static extern int sys_isatty(int fd);
    }

    public class ConsoleColorScope : IDisposable
    {
        private readonly ConsoleColor _prevColor;

        public ConsoleColorScope(ConsoleColor color)
        {
            _prevColor = Console.ForegroundColor;
            Console.ForegroundColor = color;
        }

        public void Dispose()
        {
            // When color has not been changed at all yet ForegroundColor returns "White", but setting it
            // will return in faint gray text color instead of regular default black.
            // We need to both set it, and call reset in that case.
            Console.ForegroundColor = _prevColor;
            if (_prevColor == ConsoleColor.White)
                Console.ResetColor();
        }
    }
}
