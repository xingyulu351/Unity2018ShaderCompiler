using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using Mono.Cecil;
using Mono.Cecil.Cil;
using Mono.Cecil.Mdb;
using Mono.Cecil.Pdb;
using Unity;
using Unity.BindingsGenerator.Core;
using Unity.BindingsGenerator.Core.AssemblyPatcher;
using Unity.Options;

namespace BindingsGenerator
{
    [ProgramOptions]
    internal class BindingsGeneratorOptions
    {
        public static bool Patch = false;
        public static bool Verify = false;
        public static string Assembly = null;
        public static string OutputFile = null;
        public static string[] Defines = new string[0];
        public static string ScriptingBackend = "MONO";
    }

    [ProgramOptions(Group = "codegen")]
    internal class CodeGenOptions
    {
        public static string InputFile = null;
        public static string[] References = new string[0];
        public static string[] ReferenceSearchDirectories = new string[0];
    }

    [ProgramOptions(Group = "patcher")]
    internal class PatcherOptions
    {
        public static string[] References = new string[0];
    }

    internal sealed class BindingsGenerator
    {
        public BindingsGenerator(string[] args)
        {
            OptionsParser.Prepare(args, typeof(BindingsGenerator).Assembly);
        }

        public int Run()
        {
            if (!ValidateProgramOptions())
            {
                PrintUsage();
                return -1;
            }

            return GenerateBindings();
        }

        private int GenerateBindings()
        {
            foreach (var define in BindingsGeneratorOptions.Defines)
                GlobalContext.SetDefine(define);

            try
            {
                PrepareOutputPath();
            }
            catch (Exception e)
            {
                ReportError("Could not pepare path for output file {0}", BindingsGeneratorOptions.OutputFile);
                ReportException(e);

                return 1;
            }

            AssemblyDefinition assemblyDefinition;

            var assemblyPath = BindingsGeneratorOptions.Assembly;
            var fullPath = Path.GetFullPath(assemblyPath);

            try
            {
                assemblyDefinition = LoadAssembly(fullPath);
            }
            catch (Exception e)
            {
                ReportError("Could not read assembly located at {0}", assemblyPath);
                ReportException(e);

                return 1;
            }

            if (BindingsGeneratorOptions.Verify)
            {
                try
                {
                    var errors = VerifyAssembly(assemblyDefinition);

                    if (errors.Count > 0)
                    {
                        ReportError("Assembly verification found problems in assembly located at {0}:", assemblyPath);
                        ReportError("{0}", errors.ToString());
                        return 1;
                    }
                }
                catch (Exception e)
                {
                    ReportError("Could not verify assembly located at {0}", assemblyPath);
                    ReportException(e);

                    return 1;
                }
            }

            if (BindingsGeneratorOptions.Patch)
            {
                try
                {
                    PatchAssembly(assemblyDefinition);
                }
                catch (Exception e)
                {
                    ReportError("Could not patch assembly {0}", assemblyPath);
                    ReportException(e);

                    return 1;
                }

                try
                {
                    assemblyDefinition.Write(BindingsGeneratorOptions.OutputFile, new WriterParameters
                    {
                        WriteSymbols = true,
                        SymbolWriterProvider = new MdbWriterProvider()
                    });
                }
                catch (Exception e)
                {
                    ReportError("Could not write assembly at {0}", assemblyPath);
                    ReportException(e);

                    return 1;
                }
            }
            else
            {
                try
                {
                    GenerateBindingsFor(assemblyDefinition);
                }
                catch (Exception e)
                {
                    ReportError("Could not generate bindings for {0}", assemblyPath);
                    ReportException(e);

                    return 1;
                }
            }

            return 0;
        }

        private static AssemblyDefinition LoadAssembly(string fullPath)
        {
            var searchDirs = new HashSet<string>();
            searchDirs.UnionWith(PatcherOptions.References.Select(Path.GetDirectoryName));
            searchDirs.UnionWith(CodeGenOptions.References.Select(Path.GetDirectoryName));
            searchDirs.UnionWith(CodeGenOptions.ReferenceSearchDirectories);

            ISymbolReaderProvider symbolReaderProvider = null;
            if (File.Exists(Path.ChangeExtension(fullPath, "mdb")) || File.Exists(fullPath + ".mdb"))
                symbolReaderProvider = new MdbReaderProvider();
            else if (File.Exists(Path.ChangeExtension(fullPath, "pdb")))
                symbolReaderProvider = new PdbReaderProvider();
            else
                throw new ApplicationException(string.Format("Cannot find .mdb or .pdb symbol file for : {0}", fullPath));

            var resolver = new Unity.BindingsGenerator.Core.AssemblyResolver.DefaultAssemblyResolver();
            foreach (var searchDir in searchDirs)
                resolver.AddSearchDirectory(searchDir);

            return AssemblyDefinition.ReadAssembly(fullPath, new ReaderParameters
            {
                ReadSymbols = true,
                SymbolReaderProvider = symbolReaderProvider,
                AssemblyResolver = resolver
            });
        }

        private static void PrepareOutputPath()
        {
            var outputFile = BindingsGeneratorOptions.OutputFile;
            if (File.Exists(outputFile))
                return;

            var parentDir = Path.GetDirectoryName(outputFile);
            if (parentDir == null)
                return;

            if (Directory.Exists(parentDir))
                return;

            Directory.CreateDirectory(parentDir);
        }

        private static void PatchAssembly(AssemblyDefinition assembly)
        {
            new AssemblyPatcher(new FilteringVisitor(assembly), ScriptingBackendFromString(BindingsGeneratorOptions.ScriptingBackend)).Patch();
        }

        private static AssemblyVerifier.ErrorList VerifyAssembly(AssemblyDefinition assembly)
        {
            return AssemblyVerifier.Verify(assembly, ScriptingBackendFromString(BindingsGeneratorOptions.ScriptingBackend));
        }

        private static ScriptingBackend ScriptingBackendFromString(string scriptingBackend)
        {
            if (scriptingBackend.Equals("mono", StringComparison.OrdinalIgnoreCase))
                return ScriptingBackend.Mono;
            if (scriptingBackend.Equals("dotnet", StringComparison.OrdinalIgnoreCase))
                return ScriptingBackend.DotNet;
            if (scriptingBackend.Equals("il2cpp", StringComparison.OrdinalIgnoreCase))
                return ScriptingBackend.IL2CPP;

            throw new ArgumentException($"Unsupported backend \"{scriptingBackend}\"");
        }

        private void GenerateBindingsFor(AssemblyDefinition assembly)
        {
            var scriptingBackend = ScriptingBackendFromString(BindingsGeneratorOptions.ScriptingBackend);
            var generator = new FilteredTreeGenerator(CodeGenOptions.InputFile, BindingsGeneratorOptions.Defines, scriptingBackend)
                .Generate(new FilteringVisitor(assembly));

            var headerCode = new CppGenerator().Generate(generator.HeaderFile);
            headerCode = @"#pragma once

#include ""Runtime/Scripting/Marshalling/Marshalling.h""
#include ""Runtime/Scripting/ReadOnlyScriptingObjectOfType.h""
#include ""Runtime/Scripting/BindingsDefs.h""

using namespace Scripting::ManagedNames;

" + headerCode;
            File.WriteAllText(Path.ChangeExtension(BindingsGeneratorOptions.OutputFile, ".h"), headerCode);

            var sourceCode = new CppGenerator().Generate(generator.SourceFile);
            var registrationCode = new RegistrationGenerator().Generate(generator.SourceFile, CodeGenOptions.InputFile);

            File.WriteAllText(BindingsGeneratorOptions.OutputFile, @"
#include ""UnityPrefix.h""
#include ""Configuration/UnityConfigure.h""
#include """ + Path.GetFileNameWithoutExtension(BindingsGeneratorOptions.OutputFile) + @".h""

using namespace Unity;

" + sourceCode + @"

#if !defined(INTERNAL_CALL_STRIPPING)
#   error must include unityconfigure.h
#endif" + registrationCode);
        }

        private static void PrintUsage()
        {
            Console.WriteLine("USAGE:\nBindingParser.exe --assembly=FILE --output-file=FILE");
        }

        private static bool ValidateProgramOptions()
        {
            var assemblyPath = BindingsGeneratorOptions.Assembly;

            if (string.IsNullOrEmpty(assemblyPath))
                return ReportError("The input assembly path cannot be empty");

            if (!File.Exists(assemblyPath))
                return ReportError("The given assembly path does not exist: {0}", assemblyPath);

            if (string.IsNullOrEmpty(BindingsGeneratorOptions.OutputFile))
                return ReportError("The output file cannot be empty");

            if (BindingsGeneratorOptions.Patch)
            {
                if (!BindingsGeneratorOptions.OutputFile.EndsWith(".dll"))
                    return ReportError("When patching, the output file must be a .dll file.");
            }
            else
            {
                if (!BindingsGeneratorOptions.OutputFile.EndsWith(".cpp"))
                    return ReportError("When processing for code generation, the output file must be a .cpp file.");

                if (string.IsNullOrEmpty(CodeGenOptions.InputFile))
                    return ReportError("The input file cannot be empty when processing for code generation");
            }

            return true;
        }

        private static void ReportException(Exception e)
        {
            ReportError("Unexpected exception: {0}", e.ToString());
            ReportError(e.StackTrace);
        }

        private static bool ReportError(string format, params object[] args)
        {
            Console.Error.WriteLine(format, args);

            return false;
        }
    }
}
