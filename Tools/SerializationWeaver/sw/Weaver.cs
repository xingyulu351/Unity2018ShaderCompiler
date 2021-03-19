using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Reflection;
using System.Text;
using Mono.Cecil;
using Mono.Cecil.Cil;
using Mono.Cecil.Mdb;
using Mono.Cecil.Pdb;
using Unity.Serialization.Weaver;
using Unity.SerializationLogic;
using usw.Configuration;

namespace usw
{
    public class Weaver
    {
        public enum OutSymbolsFormat
        {
            None,
            Pdb,
            Mdb
        }
        public static void WeaveAssemblies(IEnumerable<string> assemblies, string outputDir, string unityEngineDLLPath, ConversionOptions options)
        {
            new Weaver(assemblies, outputDir, unityEngineDLLPath, options).Weave();
        }

        private readonly string _outputDir;
        private readonly string _unityEngineDLLPath;
        private readonly string _unityEngineNetworkingDllPath;
        private readonly ConversionOptions _options;
        private readonly IEnumerable<string> _assemblies;

        protected Weaver(IEnumerable<string> assemblies, string outputDir, string unityEngineDLLPath, ConversionOptions options)
        {
            _unityEngineDLLPath = unityEngineDLLPath;
            _options = options;
            _assemblies = assemblies;
            _outputDir = outputDir;
            _unityEngineNetworkingDllPath = options.UnityEngineNetworkingDllPath;
        }

        protected void Weave()
        {
            var assemblyResolver = GetAssemblyResolver(_options);
            var unityEngineAssemblyDefinition = AssemblyDefinition.ReadAssembly(_unityEngineDLLPath, ReaderParameters(_unityEngineDLLPath, _options, assemblyResolver));

            foreach (var assemblyPath in _assemblies)
            {
                string stage2assemblyPath;
                if (!string.IsNullOrEmpty(_unityEngineNetworkingDllPath) && assemblyPath != _unityEngineNetworkingDllPath)
                {
                    var dependencyPaths = new List<string>(_assemblies.Where(s => s != assemblyPath));
                    dependencyPaths.Add(_unityEngineDLLPath);
                    var unetLog = new StringBuilder();
                    if (!Unity.UNetWeaver.Program.Process(_unityEngineDLLPath, _unityEngineNetworkingDllPath, _outputDir,
                        new[] {assemblyPath},
                        dependencyPaths.ToArray(), assemblyResolver, (s) => unetLog.AppendLine(s),
                        (s) => unetLog.AppendLine(s)))
                        throw new Exception(string.Format("Failed to generate networking code for {0}:\r\n{1}", assemblyPath, unetLog));

                    stage2assemblyPath = Path.Combine(_outputDir, Path.GetFileName(assemblyPath));
                    // In case UNetWeaver did nothing and didn't create output file
                    if (!File.Exists(stage2assemblyPath))
                        stage2assemblyPath = assemblyPath;
                }
                else
                    stage2assemblyPath = assemblyPath;

                WeaveAssembly(stage2assemblyPath, unityEngineAssemblyDefinition, ReaderParameters(stage2assemblyPath, _options, assemblyResolver));
            }
        }

        private WriterParameters GetWriterParameters()
        {
            var writeParams = new WriterParameters();
            switch (_options.OutSymbolsFormat)
            {
                case OutSymbolsFormat.Mdb:
                    Log("Will export symbols of mdb format");
                    writeParams.SymbolWriterProvider = new MdbWriterProvider();
                    break;
                case OutSymbolsFormat.Pdb:
                    Log("Will export symbols of pdb format");
                    writeParams.SymbolWriterProvider = new PdbWriterProvider();
                    break;
            }
            return writeParams;
        }

        private void WeaveAssembly(string assemblyPath, AssemblyDefinition unityEngineAssemblyDefinition, ReaderParameters readerParameters)
        {
            Log("Weaving assembly {0}", Path.GetFullPath(assemblyPath));

            var assemblyDefinition = AssemblyDefinition.ReadAssembly(assemblyPath, readerParameters);
            TypeDefinition[] targetDefs = TypeDefinitionsIn(assemblyDefinition).Where(UnitySerializationLogic.ShouldImplementIDeserializable).ToArray();
            foreach (var typeDefinition in targetDefs)
            {
                SerializationWeaver.Weave(typeDefinition, unityEngineAssemblyDefinition);
                Log(" + {0}", typeDefinition.FullName);
            }

            SerializationWeaver.FinalizeModuleWeaving(assemblyDefinition.MainModule);
            assemblyDefinition.Write(DestinationFileFor(assemblyPath), GetWriterParameters());
        }

        private ISymbolReaderProvider GetSymbolReaderProvider(string inputFile)
        {
            string nakedFileName = inputFile.Substring(0, inputFile.Length - 4);
            if (File.Exists(nakedFileName + ".pdb"))
            {
                Log("Symbols will be read from " + nakedFileName + ".pdb");
                return new PdbReaderProvider();
            }
            if (File.Exists(nakedFileName + ".dll.mdb"))
            {
                Log("Symbols will be read from " + nakedFileName + ".dll.mdb");
                return new MdbReaderProvider();
            }
            Console.WriteLine("No symbols for " + inputFile);
            return null;
        }

        private ReaderParameters ReaderParameters(string assemblyPath, ConversionOptions options, IAssemblyResolver assemblyResolver)
        {
            var parameters = new ReaderParameters();

            parameters.AssemblyResolver = assemblyResolver;
            if (options.OutSymbolsFormat != OutSymbolsFormat.None)
                parameters.SymbolReaderProvider = GetSymbolReaderProvider(assemblyPath);
            return parameters;
        }

        private IAssemblyResolver GetAssemblyResolver(ConversionOptions options)
        {
            if (!string.IsNullOrEmpty(options.ProjectLockFile))
            {
                var nugetResolver = new Unity.NuGetAssemblyResolver(options.ProjectLockFile);
                nugetResolver.AddSearchDirectory(UnityEngineDLLDirectoryName());
                nugetResolver.AddSearchDirectory(Path.GetDirectoryName(_unityEngineDLLPath));
                if (_unityEngineNetworkingDllPath != null)
                    nugetResolver.AddSearchDirectory(Path.GetDirectoryName(_unityEngineNetworkingDllPath));

                foreach (var path in options.additionalAssemblyPaths)
                    nugetResolver.AddSearchDirectory(path);

                return nugetResolver;
            }

            var defaultAssemblyResolver = new DefaultAssemblyResolver();
            defaultAssemblyResolver.AddSearchDirectory(UnityEngineDLLDirectoryName());
            defaultAssemblyResolver.AddSearchDirectory(Path.GetDirectoryName(_unityEngineDLLPath));
            if (_unityEngineNetworkingDllPath != null)
                defaultAssemblyResolver.AddSearchDirectory(Path.GetDirectoryName(_unityEngineNetworkingDllPath));

            foreach (var path in options.additionalAssemblyPaths)
                defaultAssemblyResolver.AddSearchDirectory(path);

            return defaultAssemblyResolver;
        }

        private static string UnityEngineDLLDirectoryName()
        {
            var directoryName = Path.GetDirectoryName(Assembly.GetExecutingAssembly().CodeBase);
            return directoryName != null ? directoryName.Replace(@"file:\", "") : null;
        }

        private string DestinationFileFor(string assemblyPath)
        {
            var fileName = Path.GetFileName(assemblyPath);
            Debug.Assert(fileName != null, "fileName != null");

            return Path.Combine(_outputDir, fileName);
        }

        private static IEnumerable<TypeDefinition> TypeDefinitionsIn(AssemblyDefinition assemblyDefinition)
        {
            var typeDefinitions = assemblyDefinition.Modules.SelectMany(moduleDefinition => moduleDefinition.Types);
            return typeDefinitions.SelectMany(NestedTypeDefinitionsAndSelfIn);
        }

        private static IEnumerable<TypeDefinition> NestedTypeDefinitionsAndSelfIn(TypeDefinition typeDefinition)
        {
            foreach (var definition in typeDefinition.NestedTypes.SelectMany(NestedTypeDefinitionsAndSelfIn))
                yield return definition;

            yield return typeDefinition;
        }

        private void Log(string message, params object[] args)
        {
            if (_options.Verbose)
                Console.WriteLine(message, args);
        }
    }
}
