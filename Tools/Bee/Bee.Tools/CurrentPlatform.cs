using System;
using System.IO;

namespace Unity.BuildTools
{
    public static class HostPlatform
    {
        public static bool IsWindows { get; } =
            Environment.OSVersion.Platform == PlatformID.Win32Windows ||
            Environment.OSVersion.Platform == PlatformID.Win32NT;
        public static bool IsLinux { get; } =
            !IsWindows && Directory.Exists("/proc");
        public static bool IsOSX { get; } =
            !IsWindows && !IsLinux;

        public static string Exe { get; } = IsWindows ? ".exe" : "";
    }
}
