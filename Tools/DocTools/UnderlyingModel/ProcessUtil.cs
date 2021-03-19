using System;
using System.Diagnostics;
using System.IO;

namespace UnderlyingModel
{
    static public class ProcessUtil
    {
        //WorkingDirectory not specified, all paths are relative to repo root
        static public ProcessStartInfo GetStartInfo(string exePath, string args)
        {
            var unitySourceRoot = DirectoryCalculator.RootDirName;

            if (IsWindows())
                return new ProcessStartInfo
                {
                    FileName = Path.Combine(unitySourceRoot, exePath),
                    Arguments = args,
                    CreateNoWindow = true,
                    UseShellExecute = false,
                    WindowStyle = ProcessWindowStyle.Hidden,
                };

            var pathToMono = Path.Combine(unitySourceRoot, "External/MonoBleedingEdge/builds/monodistribution/bin");

            var retStartInfo = new ProcessStartInfo
            {
                FileName = Path.Combine(pathToMono, "mono"),
                Arguments = unitySourceRoot + "/" + exePath + " " + args,
                CreateNoWindow = true,
                UseShellExecute = false,
                WindowStyle = ProcessWindowStyle.Hidden,
            };
            return retStartInfo;
        }

        public static bool IsWindows()
        {
            return Environment.OSVersion.Platform == PlatformID.Win32Windows
                || Environment.OSVersion.Platform == PlatformID.Win32NT
                || Environment.OSVersion.Platform == PlatformID.Win32S;
        }
    }
}
