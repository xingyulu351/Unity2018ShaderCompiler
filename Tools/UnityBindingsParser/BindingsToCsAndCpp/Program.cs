using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
using NDesk.Options;
#if TEST
using NUnit.Framework;
#endif
using Unity.CommonTools;
using UnityBindingsParser;

namespace BindingsToCsAndCpp
{
    class Constants
    {
        public const string SourceFileExtension = ".bindings";
        public const string SourceFileWildcard = "*" + SourceFileExtension;
    }

    internal class Program
    {
        protected static string TargetPlatform { get; set; }
        protected static string OutputFolder { get; set; }
        protected static List<string> Sources { get; set; }
        protected static bool CS { get; set; }
        protected static bool CPP { get; set; }

        public static void Main(string[] args)
        {
            Sources = new List<string>();
            CS = true;
            CPP = true;

            var p = new OptionSet
            {
                { "p|platform=",    v => TargetPlatform = ValidatePlatform(v) },
                { "o|output=",  v => OutputFolder = ValidateOutput(v) },
                { "nocs",   v => CS = v == null },
                { "nocpp",  v => CPP = v == null }
            };

            var files = new List<string>();
            try
            {
                files = p.Parse(args);
            }
            catch (OptionException e)
            {
                var msg = string.Format("Error parsing option '{0}' : {1}{2}", e.OptionName, e.Message, Environment.NewLine);
                throw new Exception(msg, e);
            }

            if (!CS && !CPP)
            {
                var msg = string.Format("C# and C++ output disabled, nothing to do.{0}", Environment.NewLine);
                throw new Exception(msg);
            }

            if (files.Count == 0)
            {
                Sources.AddRange(Directory.GetFiles(Path.Combine(Workspace.BasePath, "Editor", "Mono"), Constants.SourceFileWildcard, SearchOption.AllDirectories)
                    .Union(Directory.GetFiles(Path.Combine(Workspace.BasePath, "Runtime", "Export"), Constants.SourceFileWildcard)));
            }
            else
            {
                foreach (var file in files)
                    Sources.Add(ValidateSource(file));
            }

            if (Sources.Count == 0)
            {
                var msg = string.Format("No files to process, exiting{0}", Environment.NewLine);
                throw new Exception(msg);
            }

            if (TargetPlatform != null)
                Run(Sources, TargetPlatform);
            else
            {
                var msg = string.Format("Please specify a platform{0}", Environment.NewLine);
                throw new Exception(msg);
            }
        }

        static string ValidateOutput(string s)
        {
            if (!Directory.Exists(s))
                Directory.CreateDirectory(s);
            return s;
        }

        static string ValidateSource(string s)
        {
            if (!File.Exists(s))
                throw new ArgumentException("Invalid source file for " + s);

            if (Path.GetExtension(s) != Constants.SourceFileExtension)
                throw new ArgumentException("Invalid source file extension for " + s);

            return s;
        }

        static string CalculateOutputFolder(string source, string platform)
        {
            var root = Path.GetFullPath(Workspace.BasePath);
            return root + "/artifacts/" + platform + "Generated/";
        }

        static string ValidatePlatform(string s)
        {
            return s;
        }

        static void Run(List<string> files, string platform)
        {
            IBindingsConsumer consumer = null;
            GeneratorBase csconsumer = null;
            GeneratorBase cppconsumer = null;
            var typeMap = MapBuilder.BuildFrom<Dictionary<string, string>>(File.ReadAllLines(Path.Combine(Workspace.BasePath, "Runtime", "Export", "common_include")), MapBuilder.MapType.Type);

            foreach (var file in files)
            {
                var outputFolder = OutputFolder;
                if (OutputFolder == null)
                    outputFolder = CalculateOutputFolder(file, platform);

                if (!Directory.Exists(outputFolder))
                    Directory.CreateDirectory(outputFolder);

                if (CS)
                    consumer = csconsumer = new CSharpGenerator();
                if (CPP)
                    consumer = cppconsumer = new CppGenerator(Path.GetFileNameWithoutExtension(file), typeMap, ParsingHelpers.RetTypeMap, platform);
                if (CS && CPP)
                    consumer = new ConsumerDispatcher(new IBindingsConsumer[] {csconsumer, cppconsumer});

                try
                {
                    Parser.Parse(new StreamReader(file), consumer);
                }
                catch (InvalidMethodSignatureException ex)
                {
                    var msg = string.Format("Fatal: An invalid C# method signature was found while parsing {0}:{1}{2}", file, Environment.NewLine, ex.Signature);
                    throw new Exception(msg, ex);
                }
                catch (UnbalancedBracesException ub)
                {
                    var msg = string.Format("Fatal: A mismatched closing brace was found on {0}, for block {1}{2}", file, Environment.NewLine, ub.Message);
                    throw new Exception(msg, ub);
                }
                catch (Exception ex)
                {
                    var msg = string.Format("An error ocurred while parsing {0}:{1}{2}", file, Environment.NewLine, ex);
                    throw new Exception(msg, ex);
                }

                if (CS)
                {
                    var cs = csconsumer.GetOutput();
                    File.WriteAllText(Path.Combine(outputFolder, Path.GetFileNameWithoutExtension(file) + "Bindings.gen.cs"), cs);
                }

                if (CPP)
                {
                    var cpp = cppconsumer.GetOutput();
                    File.WriteAllText(Path.Combine(outputFolder, Path.GetFileNameWithoutExtension(file) + "Bindings.gen.cpp"), cpp);
                }
            }
        }

        public static string TransformToCSharp(string txt)
        {
            var consumer = new CSharpGenerator();
            Parser.Parse(new StringReader(txt), consumer);
            return consumer.GetOutput();
        }

        public static string TransformToCpp(string filename, string txt, string platform)
        {
            var typeMap = MapBuilder.BuildFrom<Dictionary<string, string>>(File.ReadAllLines(Path.Combine(Workspace.BasePath, "Runtime", "Export", "common_include")), MapBuilder.MapType.Type);
            var consumer = new CppGenerator(filename, typeMap, ParsingHelpers.RetTypeMap, platform);
            Parser.Parse(new StringReader(txt), consumer);
            return consumer.GetOutput();
        }

#if TEST
        //[Test]
        public static void RunProcessorForAllFiles()
        {
            const string arguments = " -platform Editor";
            var app = Path.Combine(Workspace.BasePath, "Tools", "UnityBindingsParser", "BindingsToCsAndCpp", "bin", "Debug",
                "BindingsToCsAndCpp.exe");

            var p = new Process
            {
                StartInfo = new ProcessStartInfo
                {
                    FileName = app,
                    Arguments =
                        arguments,
                    RedirectStandardOutput = true,
                    RedirectStandardError = true,
                    UseShellExecute = false,
                    CreateNoWindow = true
                }
            };
            p.Start();
            p.WaitForExit();
        }

        //[Test]
        public static void RunProcessorForAllFilesOneAtATime()
        {
            var sources = Directory.GetFiles(Path.Combine(Workspace.BasePath, "Editor", "Mono"), Constants.SourceFileWildcard, SearchOption.AllDirectories)
                .Union(Directory.GetFiles(Path.Combine(Workspace.BasePath, "Runtime", "Export"), Constants.SourceFileWildcard)).ToList();


            foreach (var source in sources)
            {
                var arguments = " -platform Editor " + source;
                var app = Path.Combine(Workspace.BasePath, "Tools", "UnityBindingsParser", "BindingsToCsAndCpp", "bin", "Debug",
                    "BindingsToCsAndCpp.exe");

                var p = new Process
                {
                    StartInfo = new ProcessStartInfo
                    {
                        FileName = app,
                        Arguments =
                            arguments,
                        RedirectStandardOutput = true,
                        RedirectStandardError = true,
                        UseShellExecute = false,
                        CreateNoWindow = true
                    }
                };
                p.Start();
                p.WaitForExit();
            }
        }

#endif
    }
}
