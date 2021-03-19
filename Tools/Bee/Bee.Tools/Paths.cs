using System;
using System.IO;
using NiceIO;

namespace Unity.BuildTools
{
    public class Paths
    {
        private static NPath _projectRoot;
        public static NPath ProjectRoot => _projectRoot ?? (_projectRoot = GuessProjectRoot());

        //todo: inline all UnityRoot invocations to ProjectRoot.  There's too many branch in flight right now
        //to cause so much havok though.
        public static NPath UnityRoot => ProjectRoot;

        public static NPath MonoBleedingEdgeCLI { get; } = FindMonoBleedingEdgeCli();

        public static NPath CCache { get; } = FindCCache();

        private static NPath FindMonoBleedingEdgeCli()
        {
            var mono_exe = Environment.GetEnvironmentVariable("MONO_EXECUTABLE");
            if (mono_exe != null)
            {
                var monoPath = new NPath(mono_exe);
                if (!monoPath.FileExists())
                    throw new ArgumentException($"MONO_EXECUTABLE set to {mono_exe} but that file doesn't exist");
                return monoPath;
            }
            var candidate = UnityRoot.Combine("External/MonoBleedingEdge/builds/monodistribution/bin/cli");
            if (candidate.FileExists())
                return candidate;
            return new NPath("mono");
        }

        static NPath FindCCache()
        {
            string folder;
            string ext = "";
            if (HostPlatform.IsWindows)
            {
                folder = "windows";
                ext = ".exe";
            }
            else if (HostPlatform.IsOSX)
            {
                folder = "darwin-x86";
            }
            else if (HostPlatform.IsLinux)
            {
                folder = "linux-x86";
            }
            else
            {
                return null;
            }

            NPath path = $"External/ccache/builds/{folder}/ccache{ext}";
            if (!path.FileExists())
                return null;

            if (!HostPlatform.IsWindows)
            {
                System.Diagnostics.Process.Start("chmod", $"+x {path.InQuotes()}");
            }

            return path;
        }

        static NPath GuessProjectRoot()
        {
            var candidate = NPath.CurrentDirectory;
            if (candidate.FileExists(".repoconfig"))
                return candidate;

            candidate = new NPath(typeof(Paths).Assembly.Location).ParentContaining(".repoconfig");
            if (candidate != null)
                return candidate;

            return new NPath(typeof(Paths).Assembly.Location).Parent;
        }
    }
}
