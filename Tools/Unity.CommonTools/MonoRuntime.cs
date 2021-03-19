using System.Diagnostics;
using System.IO;

namespace Unity.CommonTools
{
    public static class MonoRuntime
    {
        public static string Prefix
        {
            get
            {
                string subpath = string.Empty;

                if (Workspace.IsWindows() || Workspace.IsLinux())
                    subpath = Paths.Combine("Data", "Mono");
                else if (Workspace.IsOSX())
                    subpath = Paths.Combine("Unity.app", "Contents", "Mono");
                else
                    throw new System.ArgumentException("Unable to detect platform");

                return Path.Combine(Workspace.EditorDirectory, subpath);
            }
        }

        static string Executable
        {
            get
            {
                var executable = Path.Combine("bin", "cli") + (Workspace.IsWindows() ? ".bat" : "");

                return Path.GetFullPath(Path.Combine(Prefix, executable));
            }
        }

        public static Process Launch(string managedExecutable, string arguments)
        {
            var path = Path.GetFullPath(managedExecutable);
            if (!File.Exists(path))
                throw new FileNotFoundException(path);
            return Shell.LaunchRedirectingOutput(Executable, string.Format("\"{0}\" {1}", path, arguments));
        }

        public static void Launch(string managedExecutable, string arguments, out string output, out string error, out int exitCode)
        {
            var path = Path.GetFullPath(managedExecutable);
            if (!File.Exists(path))
                throw new FileNotFoundException(path);
            output = Shell.ExecuteProgramAndGetStdout(Executable, string.Format("\"{0}\" {1}", path, arguments), out error,
                out exitCode);
        }
    }
}
