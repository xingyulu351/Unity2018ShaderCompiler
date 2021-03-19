using System;
using System.Collections.Generic;
using System.IO;
using System.Text;

namespace Unity.CommonTools
{
    public static class CSharpCompiler
    {
        public static string CompileAssemblyFromSource(string source)
        {
            var dll = Paths.CreateTempDirectory() + "/assembly.dll";
            return CompileAssemblyFromSourceWithReferences(dll, source);
        }

        public static string CompileAssemblyFromSource(string outputAssembly, string source)
        {
            return CompileAssemblyFromSourceWithReferences(outputAssembly, source, Workspace.UnityEngineAssemblyPath);
        }

        public static string CompileAssemblyFromSourceWithReferences(string outputAssembly, string source, params string[] references)
        {
            return CompileAssemblyFromSourceWithArgsAndReferences(outputAssembly, source, "", references);
        }

        public static string CompileAssemblyFromSourceWithArgsAndReferences(string outputAssembly, string source, string additionalargs, params string[] references)
        {
            var tempSourceFile = Files.GetTempFile();
            File.WriteAllText(tempSourceFile, source);

            //hack
            var isunsafe = source.Contains("unsafe");
            var commandLine = GmcsCommandLineFor(outputAssembly, references, isunsafe, tempSourceFile) + " " + additionalargs;
            var gmcs = Paths.Combine(MonoRuntime.Prefix, "lib", "mono", "2.0", "gmcs.exe");

            // Launch - and make sure one stream is read asynchronously
            var output = string.Empty;
            var error = string.Empty;
            var errorCode = 0;
            MonoRuntime.Launch(gmcs, commandLine, out output, out error, out errorCode);
            if (errorCode != 0)
                throw new Exception("Failed compiling assembly. error: " + error);
            return outputAssembly;
        }

        static string GmcsCommandLineFor(string outputAssembly, IEnumerable<string> references, bool isunsafe, string sourceFile)
        {
            var commandLine = new StringBuilder();
            commandLine.AppendFormat("/out:\"{0}\" ", outputAssembly);
            commandLine.Append("/target:library ");
            if (isunsafe)
                commandLine.Append("/unsafe ");
            foreach (var reference in references)
                commandLine.AppendFormat("/r:\"{0}\" ", reference);
            commandLine.AppendFormat("\"{0}\"", sourceFile);

            return commandLine.ToString();
        }
    }
}
