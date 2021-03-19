using System;
using System.Diagnostics;
using System.IO;
using System.Text.RegularExpressions;

namespace Unity.CommonTools
{
    public static class DebugViewHelper
    {
        private const string kDebugViewPath = @"External\Utilities\Windows\DebugView\DbgView.exe";
        private static Process s_DebugViewProcess;

        public static string RawLogPath { get; private set; }

        public static void KillDebugViewInstances()
        {
            // Mono Bug:
            //Unhandled Exception: System.InvalidOperationException: Process has exited, so the requested information is not available.
            //  at System.Diagnostics.Process.get_ProcessName () [0x00000] in <filename unknown>:0
            //  at (wrapper remoting-invoke-with-check) System.Diagnostics.Process:get_ProcessName ()
            // Same code work with .NET
            /*
            Process[] dbgViewProcs = Process.GetProcesses();
            foreach (var process in dbgViewProcs)
            {
                try
                {
                    if (process.HasExited == false && string.Compare(process.ProcessName, "DbgView", StringComparison.OrdinalIgnoreCase) == 0)
                        process.Kill();
                }
                catch (InvalidOperationException)
                {
                    // Process was probably killed
                    throw;
                }
            }
            */
            var proc = Process.Start("taskkill", "/IM DbgView*");
            if (proc != null)
                proc.WaitForExit();
        }

        public static void Launch()
        {
            Launch(Path.Combine(Path.GetTempPath(), string.Format("rawDebugView-{0:yyyy-MM-dd_HH-mm-ss}.log", DateTime.Now)));
        }

        public static void Launch(string rawLogPath)
        {
            RawLogPath = rawLogPath;
            KillDebugViewInstances();

            if (File.Exists(RawLogPath))
                File.Delete(RawLogPath);

            s_DebugViewProcess = new Process
            {
                StartInfo =
                {
                    FileName = Workspace.ResolvePath(kDebugViewPath),
                    Arguments = string.Format("/accepteula /f /t /l \"{0}\"", RawLogPath),
                    CreateNoWindow = true,
                    UseShellExecute = false,
                    RedirectStandardOutput = true
                }
            };
            s_DebugViewProcess.Start();
        }

        public static void Exit()
        {
            if (s_DebugViewProcess != null)
            {
                s_DebugViewProcess.CloseMainWindow();
                if (!s_DebugViewProcess.HasExited)
                {
                    s_DebugViewProcess.WaitForExit(3000);
                    if (!s_DebugViewProcess.HasExited)
                    {
                        s_DebugViewProcess.Kill();
                        s_DebugViewProcess.WaitForExit(3000);
                    }
                }
            }
            KillDebugViewInstances();
        }

        public static void ExtractMessagesFromRawLog(int processId, string filteredLogPath)
        {
            if (File.Exists(filteredLogPath))
                File.Delete(filteredLogPath);
            if (!File.Exists(RawLogPath))
                return;
            using (var inputStream = new FileStream(RawLogPath, FileMode.Open, FileAccess.Read))
            using (var input = new StreamReader(inputStream))
            using (var output = new StreamWriter(filteredLogPath))
            {
                FetchProcessMessages(processId, input, output);
            }
        }

        public static void FetchProcessMessages(int processId, TextReader input, TextWriter output)
        {
            var pattern = string.Format(@"\[({0})\]", processId);
            string line;
            while ((line = input.ReadLine()) != null)
                if (Regex.IsMatch(line, pattern))
                    output.WriteLine(line);
        }
    }
}
