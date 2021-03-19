using System;
using System.Diagnostics;
using System.Text;
using UnityEditor;
using UnityEngine;

namespace DocBrowserHelper
{
    // Utility class to deal with jam building for the DocBrowser project.
    // It needs to be external, so that the files get copied in for use.
    public class DocBrowserUtility
    {
        public static void InitialUnderlyingModel()
        {
            ProcessStartInfo startInfo = new ProcessStartInfo
            {
                WorkingDirectory = Unsupported.GetBaseUnityDeveloperFolder(),
                RedirectStandardOutput = true,
                RedirectStandardError = false,
                // Jam execution has some failures since it's trying to overwrite managed DLLs that the editor has open right now;
                // have to continue in presence of errors.
                Arguments = "jam.pl -e DocProject",
                FileName = "perl",
            };

            int exitCode;
            string messages = GetOutputStream(startInfo, out exitCode);
            if (exitCode != 0)
                UnityEngine.Debug.LogError(messages);
        }

        private static string GetOutputStream(ProcessStartInfo startInfo, out int exitCode)
        {
            startInfo.UseShellExecute = false;
            startInfo.CreateNoWindow = true;
            var p = new Process { StartInfo = startInfo };

            // Read data asynchronously
            var sbStandardOut = new StringBuilder();
            var sbStandardError = new StringBuilder();
            p.OutputDataReceived += (sender, data) => sbStandardOut.AppendLine(data.Data);
            p.ErrorDataReceived += (sender, data) => sbStandardError.AppendLine(data.Data);
            p.Start();
            if (startInfo.RedirectStandardError)
                p.BeginErrorReadLine();
            else
                p.BeginOutputReadLine();

            // Wain until process is done
            p.WaitForExit();

            var output = startInfo.RedirectStandardError ? sbStandardError.ToString() : sbStandardOut.ToString();
            exitCode = p.ExitCode;
            return output;
        }
    }
}
