using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;
using Unity.PackageManager;
using Unity.Packager;

namespace Tests
{
    static class Utils
    {
        public static void CreatePackage(DirectoryInfo outputDir, string packagePath, string org, string name, string version,
            string unityVersion, string packageType, string packageSubdir)
        {
            List<string> args = new List<string>();
            args.Add("-s");
            args.Add(packagePath);
            args.Add("-o");
            args.Add(outputDir.FullName);

            args.Add("-g");
            args.Add(org);
            args.Add("-n");
            args.Add(name);
            args.Add("-v");
            args.Add(version);
            args.Add("-u");
            args.Add(unityVersion);
            args.Add("-t");
            args.Add(packageType);
            args.Add("--packagesubdir");
            args.Add(packageSubdir);

            RunPackager(args.ToArray());
        }

        public static void CreateIndex(DirectoryInfo outputDir, string packagePath, string packageSubdir)
        {
            List<string> args = new List<string>();

            args.Add("-x");
            args.Add("-s");
            args.Add(packagePath);
            args.Add("-o");
            args.Add(outputDir.FullName);
            args.Add("-r");
            args.Add(Settings.RepoUrl.ToString());
            args.Add("--packagesubdir");
            args.Add(packageSubdir);

            RunPackager(args.ToArray());
        }

        static void RunPackager(string[] arguments)
        {
            Program.Main(arguments);
        }
    }
}
