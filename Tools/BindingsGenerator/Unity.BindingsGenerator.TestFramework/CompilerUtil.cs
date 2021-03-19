using System;
using System.CodeDom.Compiler;
using System.IO;
using System.Linq;

namespace Unity.BindingsGenerator.TestFramework
{
    public class CompilerUtil
    {
        public static string CompileAssembly(string outputPath, string source, params string[] assemblyReferences)
        {
            var directory = Path.GetDirectoryName(outputPath);
            if (!Directory.Exists(directory))
                Directory.CreateDirectory(directory);

            var codeDomProvider = new Microsoft.CSharp.CSharpCodeProvider();
            var p = new CompilerParameters();

            p.OutputAssembly = outputPath;
            p.IncludeDebugInformation = true;
            p.CompilerOptions = "/o- /debug+ /noconfig /warn:0";

            if (assemblyReferences != null)
            {
                p.ReferencedAssemblies.AddRange(assemblyReferences);
            }

            var compilerResult = codeDomProvider.CompileAssemblyFromSource(p, source);
            if (compilerResult.Errors.Count > 0)
            {
                throw new Exception(compilerResult.Errors.OfType<CompilerError>().Aggregate("", (acc, curr) => acc + "\r\n" + curr.ErrorText));
            }

            return compilerResult.PathToAssembly;
        }
    }
}
