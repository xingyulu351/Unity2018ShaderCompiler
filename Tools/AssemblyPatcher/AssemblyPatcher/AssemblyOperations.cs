using System.IO;
using System.Reflection;
using AssemblyPatcher.Configuration;
using AssemblyPatcher.Extensions;
using Mono.Cecil;
using Mono.Cecil.Mdb;
using Mono.Cecil.Pdb;

namespace AssemblyPatcher
{
    internal class AssemblyOperations
    {
        internal static AssemblyDefinition ReadAssemblyToBePatched(string assemblyPath, string searchPath)
        {
            var assemblyResolver = DefaultAssemblyResolverFor(assemblyPath, searchPath);

            var readerParameters = new ReaderParameters
            {
                AssemblyResolver = assemblyResolver
            };

            var assemblyToBePatched = AssemblyDefinition.ReadAssembly(assemblyPath, readerParameters);
            TryReadSymbols(assemblyPath, assemblyToBePatched);

            return assemblyToBePatched;
        }

        internal static string SavePatchedAssembly(string assemblyPath, string output, AssemblyDefinition assembly, string keyFilePath, TargetPlatform platform)
        {
            var sourceDirectory = Path.GetDirectoryName(assemblyPath);

            if (string.IsNullOrWhiteSpace(output))
            {
                output = Path.Combine(sourceDirectory, Path.GetFileNameWithoutExtension(assemblyPath) + ".patched.dll");
            }
            else
            {
                output = output.Trim();
                if (!Path.IsPathRooted(output) && output.IndexOf(Path.DirectorySeparatorChar) == -1)
                {
                    output = Path.Combine(sourceDirectory, output);
                }
            }
            output.EnsureDirectoryExists();
            assembly.Write(output, WriteParametersFor(keyFilePath, platform));

            return output;
        }

        private static WriterParameters WriteParametersFor(string keyFilePath, TargetPlatform platform)
        {
            var writeParameters = new WriterParameters();
            if (keyFilePath != null)
            {
                writeParameters.StrongNameKeyPair = new StrongNameKeyPair(File.Open(keyFilePath, FileMode.Open, FileAccess.Read, FileShare.Read));
            }

            switch (platform)
            {
                case TargetPlatform.WP8:
                case TargetPlatform.WSA:
                    writeParameters.SymbolWriterProvider = new PdbWriterProvider();
                    break;
                default:
                    writeParameters.SymbolWriterProvider = new MdbWriterProvider();
                    break;
            }


            return writeParameters;
        }

        private static DefaultAssemblyResolver DefaultAssemblyResolverFor(string assemblyPath, string searchPath)
        {
            var assemblyResolver = new DefaultAssemblyResolver();
            assemblyResolver.AddSearchDirectory(Path.GetDirectoryName(assemblyPath));
            if (searchPath != null)
            {
                assemblyResolver.AddSearchDirectory(searchPath);
            }
            return assemblyResolver;
        }

        private static void TryReadSymbols(string assemblyPath, AssemblyDefinition assemblyToBePatched)
        {
            var pdbSymbolFile = Path.ChangeExtension(assemblyPath, ".pdb");
            if (File.Exists(pdbSymbolFile))
            {
                var symbolReader = new PdbReaderProvider().GetSymbolReader(assemblyToBePatched.MainModule, assemblyPath);
                assemblyToBePatched.MainModule.ReadSymbols(symbolReader);
            }

            var mdbSymbolFile = Path.ChangeExtension(assemblyPath, ".dll.mdb");
            if (File.Exists(mdbSymbolFile))
            {
                var symbolReader = new MdbReaderProvider().GetSymbolReader(assemblyToBePatched.MainModule, assemblyPath);
                assemblyToBePatched.MainModule.ReadSymbols(symbolReader);
            }
        }
    }
}
