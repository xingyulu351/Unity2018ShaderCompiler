using System;
using System.Collections.Generic;
using System.IO;
using usw.Configuration;

namespace usw
{
    public class Program
    {
        public static int Main(string[] args)
        {
            try
            {
                return RunProgram(ConversionOptions.Parse(args));
            }
            catch (Exception e)
            {
                Console.WriteLine(e);
                return 1;
            }
        }

        private static int RunProgram(ConversionOptions options)
        {
            CheckUnityEngineDLLPath(options.UnityEngineDLLPath);
            CheckOutputDirectory(options.OutputDir);
            CheckAssemblies(options.Assemblies);
            CheckLockJsonFile(options.ProjectLockFile);
            CheckUnityNetworkingDLLPath(options.UnityEngineNetworkingDllPath);

            Weaver.WeaveAssemblies(options.Assemblies, options.OutputDir, options.UnityEngineDLLPath, options);

            return 0;
        }

        private static void CheckUnityEngineDLLPath(string path)
        {
            if (!File.Exists(path))
                throw new Exception("UnityEngine.dll could not be located at " + path + "!");
        }

        private static void CheckAssemblies(IEnumerable<string> assemblyPaths)
        {
            foreach (var assemblyPath in assemblyPaths)
                CheckAssemblyPath(assemblyPath);
        }

        private static void CheckAssemblyPath(string assemblyPath)
        {
            if (!File.Exists(assemblyPath))
                throw new Exception("Assembly " + assemblyPath + " does not exist!");
        }

        private static void CheckOutputDirectory(string outputDir)
        {
            try
            {
                if (!Directory.Exists(outputDir))
                    Directory.CreateDirectory(outputDir);
            }
            catch (Exception e)
            {
                throw new Exception("Failed to create " + outputDir + Environment.NewLine + e);
            }
        }

        private static void CheckLockJsonFile(string lockJsonFile)
        {
            if (!string.IsNullOrEmpty(lockJsonFile) && !File.Exists(lockJsonFile))
                throw new Exception("project.lock.json file at " + lockJsonFile + " does not exist!");
        }

        private static void CheckUnityNetworkingDLLPath(string unityEngineNetworkingDllPath)
        {
            if (!string.IsNullOrEmpty(unityEngineNetworkingDllPath) && !File.Exists(unityEngineNetworkingDllPath))
                throw new Exception("Networking DLL at " + unityEngineNetworkingDllPath + " does not exist!");
        }
    }
}
