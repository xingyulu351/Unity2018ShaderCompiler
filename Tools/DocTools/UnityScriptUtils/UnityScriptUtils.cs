using System;
using System.Diagnostics;
using System.IO;
using System.Text;
using UnderlyingModel;

namespace UnityScriptUtils
{
    public static class Compilation
    {
        public static string CompileUnityScript(string scriptPath)
        {
            const string pathToUs = @"External/Mono/builds/monodistribution/lib/mono/2.0/us.exe";

            string outputAssemblyPath = Path.GetTempFileName();
            File.Delete(outputAssemblyPath);

            Console.WriteLine("Output Assembly Path: " + outputAssemblyPath);

            var usCompiler = CreateUnityScriptCompilerProcess(pathToUs, scriptPath, outputAssemblyPath);
            var sb = new StringBuilder();

            usCompiler.ErrorDataReceived += (sender, args) => sb.Append(args.Data);
            usCompiler.Start();
            usCompiler.WaitForExit();
            if (usCompiler.ExitCode != 0)
            {
                var compilerOutput = sb + "  " + usCompiler.StandardOutput.ReadToEnd();
                return "Failed to compile script:\n" + scriptPath + "\r\nCompiler output: " + compilerOutput;
            }
            return null; //no error
        }

        private static Process CreateUnityScriptCompilerProcess(string pathToUs, string scriptPath, string outputAssemblyPath)
        {
            var psi = ProcessUtil.GetStartInfo(pathToUs,
                scriptPath +
                @" -pragmas:strict -method:Main -import:System.Collections -import:System -import:UnityEngine -base:System.Object -r:""..\..\..\..\..\build\WindowsEditor\Data\Managed\UnityEngine.dll"" -out:" +
                outputAssemblyPath);

            psi.RedirectStandardOutput = true;
            psi.RedirectStandardError = true;

            return new Process { StartInfo = psi };
        }
    }
}
