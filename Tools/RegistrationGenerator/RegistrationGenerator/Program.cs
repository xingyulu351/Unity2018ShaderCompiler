using Mono.Cecil;
using System;
using System.Collections.Generic;
using System.IO;
using Unity.RegistrationGenerator;

namespace RegistrationGenerator
{
    class Program
    {
        static int Main(string[] rawArgs)
        {
            var args = Unity.Options.OptionsParser.ExpandCommandLineResponseFiles(rawArgs);

            if (args.Count < 3)
            {
                Console.Error.WriteLine("Usage: RegistrationGenerator <module name> <managed assembly dll> <referenceDirs> [cpp codegen output file]");
                return 1;
            }

            var resolver = new DefaultAssemblyResolver();
            resolver.RemoveSearchDirectory(".");
            resolver.RemoveSearchDirectory("bin");
            resolver.AddSearchDirectory(Path.GetDirectoryName(args[1]));

            foreach (var dir in args[2].Split(';'))
                resolver.AddSearchDirectory(dir);

            var readerParameters = new ReaderParameters()
            {
                AssemblyResolver = resolver
            };

            var managedAssembly = AssemblyDefinition.ReadAssembly(args[1], readerParameters);
            var sharedInternalsAssembly = FindModuleAssembly(resolver, managedAssembly, "SharedInternals");
            var coreAssembly = FindModuleAssembly(resolver, managedAssembly, "Core");

            var codeGen = new CppCodeGeneration(args[0], managedAssembly, coreAssembly, sharedInternalsAssembly);

            codeGen.AddProcessor(new RequireComponentProcessor(coreAssembly));
            codeGen.AddProcessor(new ClassRegistrationProcessor(coreAssembly));

            var result = codeGen.Generate();
            if (args.Count < 3)
            {
                Console.Out.Write(result);
            }
            else
            {
                var path = args[3];
                var dirPath = Path.GetDirectoryName(path);
                Directory.CreateDirectory(dirPath);
                File.WriteAllText(path, result);
            }
            return 0;
        }

        private static AssemblyDefinition FindModuleAssembly(IAssemblyResolver resolver, AssemblyDefinition managedAssembly, string moduleName)
        {
            var moduleAssemblyName = $"UnityEngine.{moduleName}Module";
            if (managedAssembly.Name.Name == moduleAssemblyName)
                return managedAssembly;

            try
            {
                var assemblyName = new AssemblyNameReference(moduleAssemblyName, new Version());
                return resolver.Resolve(assemblyName);
            }
            catch (AssemblyResolutionException)
            {
            }

            if (managedAssembly.Name.Name == "UnityEngine")
                return managedAssembly;

            try
            {
                var assemblyName = new AssemblyNameReference("UnityEngine", new Version());
                return resolver.Resolve(assemblyName);
            }
            catch (AssemblyResolutionException)
            {
            }

            return null;
        }
    }
}
