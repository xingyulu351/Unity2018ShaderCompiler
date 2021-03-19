using NiceIO;
using Unity.BuildTools;

namespace Bee.Tools
{
    public static class MultiProcessDebugHelper
    {
        public static bool Enabled = false;

        public static void Clear()
        {
            if (!Enabled)
                return;
            OutputFile.WriteAllText("");
        }

        private static NPath OutputFile => Paths.ProjectRoot.Combine("multiprocess_debughelper_output");

        public static void WriteLine(string msg)
        {
            if (!Enabled)
                return;
            var currentProcess = System.Diagnostics.Process.GetCurrentProcess();
            var fileName = currentProcess.ProcessName;
            var id = currentProcess.Id;
            var prefix = $"{fileName} {id}: {msg}\n";
            OutputFile.WriteAllText(OutputFile.ReadAllText() + prefix);
        }
    }
}
