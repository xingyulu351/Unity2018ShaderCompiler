using System;
using System.Reflection;
using System.Collections.Generic;
using System.Linq;
using NUnit.Framework;
using NDesk.Options;
using System.IO;
using Unity.CommonTools;
using BindingsToCsAndCpp.Tests;

namespace BindingsToCsAndCpp.TestGenerator
{
    class MainClass
    {
        static bool GenerateZip = false;
        static bool HasZipFile;
        static string ZipFile;
        static bool GenerateDiffFile = false;
        static bool Run = false;
        static string DiffFile;
        static string Method = null;
        static bool Help;

        static bool HasTemplatePath;
        static string TemplatePath;
        static bool GenerateTemplates = false;
        static string Dll;
        static List<ITemplateTest> Types = new List<ITemplateTest>();

        static void SetTemplatePath(string v)
        {
            TemplatePath = v;
            HasTemplatePath = true;
        }

        static void SetZipFile(string v)
        {
            ZipFile = v;
            HasZipFile = true;
        }

        public static int Main(string[] args)
        {
            string basePath = Path.Combine(Workspace.BasePath, "Tools", "UnityBindingsParser", "BindingsToCsAndCpp", "Tests");
            ZipFile = Path.Combine(basePath, "Generated.zip");
            DiffFile = Path.Combine(basePath, "generated.diff");

            var p = new OptionSet() {
                { "g", "Generate templates from tests", v => Run = GenerateTemplates = true },
                { "p=",
                  string.Format("Path where to put generated templates. Default is {0}/[whatever the ITemplateTest implementation sets].",
                      Path.Combine(Workspace.BasePath, "Tools", "UnityBindingsParser", "BindingsToCsAndCpp", "Tests")),
                  SetTemplatePath },
                { "z", "Zip up the generated templates", v => GenerateZip = true },
                { "zip=", string.Format("Zip file to use. Default is whatever the ITemplateTest implementation sets"), SetZipFile },
                { "d", "Generate a diff file between the zipped templates and the unzipped templates", v => GenerateDiffFile = true },
                { "diff=", string.Format("Diff file to use."), v => DiffFile = v },
                { "m|method=", "Specify which test method to generate a template for (default: none, all tests will get generated)", v => Method = v },
                { "f=", "Dll to generate templates from", v => Dll = v },
                { "h|help", "Show usage", v => Help = true },
            };

            try
            {
                p.Parse(args);
            }
            catch (OptionException e)
            {
                Console.WriteLine("Error parsing option '{0}' : {1}", e.OptionName, e.Message);
                Console.WriteLine();
                return -1;
            }

            if (Help)
            {
                p.WriteOptionDescriptions(Console.Out);
                return 0;
            }

            var asm = Assembly.LoadFile(Dll);
            Types.AddRange(asm.GetTypes()
                .Where(x => typeof(ITemplateTest).IsAssignableFrom(x))
                .Select(x => {
                    var t = Activator.CreateInstance(x) as ITemplateTest;
                    if (HasTemplatePath)
                        t.TemplatePath = Path.Combine(TemplatePath, t.DefaultTemplatePath);
                    if (HasZipFile)
                        t.ZipFile = ZipFile;
                    ZipFile = t.ZipFile;
                    TemplatePath = t.TemplatePath;
                    DiffFile = Path.Combine(Path.GetDirectoryName(ZipFile), Path.GetFileNameWithoutExtension(ZipFile) + ".diff");
                    t.GenerateTemplates = GenerateTemplates;
                    return t;
                }
                )
            );

            foreach (var t in Types)
            {
                if (Run)
                {
                    if (Method != null)
                    {
                        var m = t.GetType().GetMethod(Method);
                        if (m == null)
                        {
                            Console.WriteLine("Could not find method {0}. Aborting.", Method);
                            return -2;
                        }

                        RunMethod(t, m);
                    }
                    else
                    {
                        if (Directory.Exists(t.TemplatePath))
                            Directory.GetFiles(t.TemplatePath).All(x => { File.Delete(x); return true; });

                        var methods = t.GetType().GetMethods().Where(x => x.GetCustomAttributes(typeof(TestAttribute), false).Any());

                        foreach (var m in methods)
                            RunMethod(t, m);
                    }
                }

                if (GenerateDiffFile)
                {
                    if (File.Exists(ZipFile))
                    {
                        if (File.Exists(DiffFile))
                            File.Delete(DiffFile);

                        var diffFile = new FileStream(DiffFile, FileMode.Append);

                        string outputPath = ZipUtils.GetTempDirectory().FullName;

                        Console.WriteLine("Extracting template zip to {0}", outputPath);
                        ZipUtils.ExtractZipFile(ZipFile, outputPath);
                        var leftSide = Directory.GetFiles(outputPath);
                        foreach (var f in leftSide)
                        {
                            GenerateDiff(Path.GetFileName(f), outputPath, t.TemplatePath, diffFile);
                        }
                        var rightSide = Directory.GetFiles(t.TemplatePath).Select(Path.GetFileName).Except(leftSide.Select(Path.GetFileName));

                        foreach (var f in rightSide)
                        {
                            GenerateDiff(Path.GetFileName(f), outputPath, t.TemplatePath, diffFile);
                        }

                        diffFile.Close();
                    }
                }


                if (GenerateZip)
                {
                    if (File.Exists(ZipFile))
                        File.Delete(ZipFile);
                    ZipUtils.CompressFolder(t.TemplatePath, t.TemplatePath, ZipFile);
                }
            }
            return 0;
        }

        static void GenerateDiff(string file, string leftPath, string rightPath, Stream diffFile)
        {
            Console.Write("Generating diff for {0}... ", file);
            var leftFile = Path.Combine(leftPath, file);
            var rightFile = Path.Combine(rightPath, file);

            Stream ls = null;
            Diff.UnifiedDiffInfo old = null;
            if (File.Exists(leftFile))
            {
                ls = File.OpenRead(leftFile);
                old = new Diff.UnifiedDiffInfo(ls, "old/" + file);
            }

            Diff.UnifiedDiffInfo now = null;
            Stream rs = null;
            if (File.Exists(rightFile))
            {
                rs = File.OpenRead(rightFile);
                now = new Diff.UnifiedDiffInfo(rs, "new/" + file);
            }
            if (!Diff.UnifiedDiff.Create(old, now, diffFile, 5))
            {
                Console.WriteLine("nothing to do");
            }
            else
            {
                Console.WriteLine("done");
            }
            if (ls != null)
                ls.Close();
            if (rs != null)
                rs.Close();
        }

        static void RunMethod(object t, MethodInfo m)
        {
            try
            {
                Console.WriteLine("Generating {0}", m.Name);
                m.Invoke(t, null);
            }
            catch (Exception ex)
            {
                if (ex.GetType().Name == "UnbalancedBracesException" || ex.InnerException.GetType().Name == "UnbalancedBracesException")
                    Console.WriteLine("Skipping {0}, it's not meant to have a template", m.Name);
                else
                    Console.WriteLine("Skipping {0}, an error ocurred, should investigate. {1}", m.Name, ex);
            }
        }
    }
}
