using System;
using System.IO;
using System.Collections.Generic;
using System.Reflection;
using Mono.Cecil;

namespace ProfileDocumenter
{
    class MainClass
    {
        static string unityRoot;
        static string monoLibMono;

        static bool CheckAssemblyName(string name)
        {
            if (name.Equals("mscorlib.dll", StringComparison.OrdinalIgnoreCase))
                return true;
            if (name.Equals("system.dll", StringComparison.OrdinalIgnoreCase))
                return true;
            if (name.Equals("system.core.dll", StringComparison.OrdinalIgnoreCase))
                return true;
            return false;
        }

        static IEnumerable<AssemblyDefinition> GetAssembliesForProfile(UnityProfiles profile)
        {
            foreach (string file in Directory.GetFiles(monoLibMono + UnityProfilesUtils.DirectoryNameFromProfile(profile), "*.dll"))
            {
                string assemblyname = Path.GetFileName(file);

                if (CheckAssemblyName(assemblyname))
                {
                    yield return AssemblyFactory.GetAssembly(file);
                    //yield return AssemblyDefinition.ReadAssembly (file);
                }
            }
            yield break;
        }

        public static void Main(string[] args)
        {
            string myAssembly = Assembly.GetExecutingAssembly().Location;
            unityRoot = new FileInfo(myAssembly).Directory.Parent.Parent.Parent.Parent.FullName;
            monoLibMono =  unityRoot + "/External/Mono/builds/monodistribution/lib/mono/";
            string destinationFolder = unityRoot + "/Runtime/ExportOnlyDocs/";

            UnityProfilesDocumentation doc = new UnityProfilesDocumentation();
            BaseProfileBuilder docBuilder = new BaseProfileBuilder(doc);

            Console.WriteLine("Scanning base assemblies...");

            foreach (AssemblyDefinition assembly in GetAssembliesForProfile(UnityProfiles.Base))
            {
                //AssemblyApiScanner.Scan (assembly, new PrintAssemblyApi ());
                AssemblyApiScanner.Scan(assembly, docBuilder);
            }

            foreach (UnityProfiles profile in UnityProfilesUtils.ListProfiles())
            {
                Console.WriteLine("Scanning assemblies in profile {0}...", profile);

                foreach (AssemblyDefinition assembly in GetAssembliesForProfile(profile))
                {
                    ProfileFlagsUpdater flagsUpdater = new ProfileFlagsUpdater(doc, profile);
                    AssemblyApiScanner.Scan(assembly, flagsUpdater);
                }
            }

            Console.WriteLine("Writing profiles documentation...");
            //doc.Print (DebugDumpDocumentation.Printer);
            doc.Print(new HttpDocumentationPrinter(destinationFolder + "UnityProfiles.txt"));
            Console.WriteLine("Profiles documentation done.");
        }
    }
}
