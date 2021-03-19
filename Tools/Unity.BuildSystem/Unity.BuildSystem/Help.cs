using System;
using System.Collections.Generic;
using System.Linq;
using System.Reflection;
using System.Text;
using Unity.BuildTools;

namespace Unity.BuildSystem
{
    [AttributeUsage(AttributeTargets.Class)]
    public class HelpAttribute : Attribute
    {
        public string Name { get; }
        public string Desc { get; }
        public string[] Notes { get; }
        public Help.Category Category { get; }
        public Help.Configs Configs { get;  }
        public Help.HostType Host { get; }
        public Help.Platform Platform { get; }
        public Help.ScriptingBackendOptions ScriptingBackend { get; }
        public int Order { get;  }

        public bool ConditionVariable { get; } = true;

        public HelpAttribute(string name, string desc = null,
                             string[] notes = null,
                             Help.Category category = Help.Category.Other,
                             Help.Configs configs = Help.Configs.DefaultDebugRelease,
                             Help.HostType host = Help.HostType.All,
                             Help.Platform platform = Help.Platform.Default,
                             Help.ScriptingBackendOptions scriptingBackendOptions = Help.ScriptingBackendOptions.DefaultOnly,
                             int order = 0, Type conditionVariableType = null, string conditionVariableName = null, bool conditionVariableTest = true)
        {
            Name = name;
            Desc = desc ?? "";
            Notes = notes ?? new string[0];
            Category = category;
            Configs = configs;
            Host = host;
            Platform = platform;
            ScriptingBackend = scriptingBackendOptions;
            Order = order;

            if (conditionVariableType != null && conditionVariableName != null)
            {
                var condProp = conditionVariableType.GetProperty(conditionVariableName, BindingFlags.Public | BindingFlags.Static);
                if (condProp != null && condProp.PropertyType == typeof(bool))
                {
                    ConditionVariable = (bool)condProp.GetValue(null, null) == conditionVariableTest;
                }
                else
                {
                    throw new ArgumentException();
                }
            }
        }

        public string toJSON()
        {
            StringBuilder sb = new StringBuilder();

            sb.Append("{ \n");
            sb.Append("\"name\":\"" + Name + "\",\n");
            sb.Append("\"desc\":\"" + Desc + "\",\n");
            sb.Append("\"notes\":[");
            for (int i = 0; i < Notes.Length; i++)
            {
                sb.Append("\"" + Notes[i] + "\"");
                if (i != Notes.Length - 1)
                    sb.Append(",");
            }
            sb.Append("],\n");
            sb.Append("\"category\":\"" + Enum.GetName(typeof(Help.Category) , Category) + "\",\n");
            sb.Append("\"configs\":\"" + Enum.GetName(typeof(Help.Configs), Configs) + "\",\n");
            sb.Append("\"host\":\"" + Enum.GetName(typeof(Help.HostType), Host) + "\",\n");
            sb.Append("\"platform\":\"" + Enum.GetName(typeof(Help.Platform), Platform) + "\",\n");
            sb.Append("\"scriptingbackends\":\"" +
                Enum.GetName(typeof(Help.ScriptingBackendOptions), ScriptingBackend) + "\",\n");
            sb.Append("\"order\":" + Order + "\n");

            sb.Append("}");
            return sb.ToString();
        }
    }

    public class Help
    {
        public enum Category
        {
            Editors,
            Standalone,
            OtherPlayers,
            Projects,
            Other
        }

        public enum Configs
        {
            DefaultDebugRelease,
            DebugReleaseMaster,
            DebugDevelopRelease,
            Release,
            None
        }
        public static string JamCmd => HostPlatform.IsWindows ? "jam" : "./jam";

        struct HelpEntry
        {
            public string[] CommandLineOfInterest;
            public Action PrintHelp;
        }

        public static void AddHelpForSpecificPlatform(string[] commandLinesOfInterest, Action printHelp)
        {
            PlatformSpecificHelp.Add(new HelpEntry() {CommandLineOfInterest = commandLinesOfInterest, PrintHelp = printHelp});
        }

        static List<HelpEntry> PlatformSpecificHelp { get; } = new List<HelpEntry>();

        static void Print(Configs v)
        {
            switch (v)
            {
                case Configs.DefaultDebugRelease:
                    Console.Write("debug ");
                    Light();
                    Console.Write("release         ");
                    Normal();
                    break;
                case Configs.DebugReleaseMaster:
                    Console.Write("debug ");
                    Light();
                    Console.Write("release master  ");
                    Normal();
                    break;
                case Configs.DebugDevelopRelease:
                    Console.Write("debug ");
                    Light();
                    Console.Write("develop release ");
                    Normal();
                    break;
                case Configs.Release:
                    Console.Write("release               ");
                    break;
                case Configs.None:
                    Console.Write("                      ");
                    break;
                default:
                    throw new ArgumentOutOfRangeException(nameof(v), v, null);
            }
        }

        public enum Platform
        {
            Default,
            Win6432,
            Linux6432,
            UAP
        }

        static void Print(Platform v)
        {
            switch (v)
            {
                case Platform.Default:
                    Console.Write("                        ");
                    break;
                case Platform.Win6432:
                    Console.Write("win64 ");
                    Light();
                    Console.Write("win32         ");
                    Normal();
                    break;
                case Platform.Linux6432:
                    Console.Write("linux64 ");
                    Light();
                    Console.Write("linux32         ");
                    Normal();
                    break;
                case Platform.UAP:
                    Light();
                    Console.Write("uap_x64 uap_x86 uap_arm uap_arm64 ");
                    Normal();
                    break;
                default:
                    throw new ArgumentOutOfRangeException(nameof(v), v, null);
            }
        }

        [Flags]
        public enum HostType
        {
            Windows = 1 << 0,
            Mac = 1 << 1,
            Linux = 1 << 2,
            All = Windows | Mac | Linux
        }

        [Flags]
        public enum ScriptingBackendOptions
        {
            DefaultOnly = 1 << 0,
            Mono = 1 << 1,
            Il2Cpp = 1 << 2,
            Dotnet = 1 << 3,
            MonoIl2Cpp = Mono | Il2Cpp,
            Il2CppDotNet = Il2Cpp | Dotnet
        }

        private static void Print(ScriptingBackendOptions tScriptingBackend)
        {
            switch (tScriptingBackend)
            {
                case ScriptingBackendOptions.DefaultOnly:
                    break;
                case ScriptingBackendOptions.Mono:
                    Console.Write("mono");
                    break;
                case ScriptingBackendOptions.Il2Cpp:
                    Console.Write("il2cpp");
                    break;
                case ScriptingBackendOptions.Dotnet:
                    Console.Write("dotnet");
                    break;
                case ScriptingBackendOptions.MonoIl2Cpp:
                    Console.Write("mono");
                    Light();
                    Console.Write(" il2cpp");
                    Normal();
                    break;
                case ScriptingBackendOptions.Il2CppDotNet:
                    Console.Write("il2cpp");
                    Light();
                    Console.Write(" dotnet");
                    Normal();
                    break;
                default:
                    throw new ArgumentOutOfRangeException(nameof(tScriptingBackend), tScriptingBackend, null);
            }
        }

        public static void Print()
        {
            var printSpecific = PlatformSpecificHelp
                .FirstOrDefault(c => Jamrules.CommandLineCouldBeInterestedIn(c.CommandLineOfInterest))
                .PrintHelp;
            if (printSpecific != null)
            {
                PrintOverview(true);
                printSpecific();
                return;
            }

            var targets = FindHelpAttributes();
            FilterTargetsByHost(ref targets);
            FilterTargetsByConditionVariable(ref targets);
            Sort(targets);
            PrintOverview(false);
            PrintTargets(targets);
        }

        public static void PrintHelpJSON()
        {
            var targets = FindHelpAttributes();
            FilterTargetsByHost(ref targets);
            Sort(targets);
            StringBuilder sb = new StringBuilder();
            sb.Append("{");
            sb.Append("\"targets\": [");
            foreach (var t in targets)
            {
                sb.Append(t.toJSON() + (!t.Equals(targets.Last()) ? "," : ""));
            }
            sb.Append("]\n}");
            Console.WriteLine(sb.ToString());
        }

        private static void PrintTargets(List<HelpAttribute> targets)
        {
            Heading();
            Console.WriteLine("Build Target              Description                -sCONFIG              -sPLATFORM              -sSCRIPTING_BACKEND");
            Normal();

            var categories = (Category[])Enum.GetValues(typeof(Category));
            foreach (var cat in categories)
            {
                var categoryTargets = targets.Where(h => h.Category == cat).ToArray();
                if (!categoryTargets.Any())
                    continue;
                Heading();
                Console.WriteLine(cat);
                Normal();
                foreach (var t in categoryTargets)
                {
                    Bold();
                    Console.Write($"{t.Name,-25}");
                    Normal();
                    Console.Write($" {t.Desc,-27}");
                    Print(t.Configs);
                    Print(t.Platform);
                    Print(t.ScriptingBackend);
                    Console.WriteLine();
                    foreach (var n in t.Notes)
                    {
                        Light();
                        Console.WriteLine($"  {n}");
                        Normal();
                    }
                }
            }
        }

        private static void Sort(List<HelpAttribute> targets)
        {
            targets.Sort((a, b) =>
            {
                if (a.Category != b.Category)
                    return (int)a.Category - (int)b.Category;
                if (a.Order != b.Order)
                    return a.Order.CompareTo(b.Order);
                return string.Compare(a.Name, b.Name, StringComparison.OrdinalIgnoreCase);
            });
        }

        private static List<HelpAttribute> FindHelpAttributes()
        {
            var allAttributes = new List<HelpAttribute>();
            foreach (var type in AppDomain.CurrentDomain
                     .GetAssemblies()
                     .SelectMany(t => t.GetTypes()))
            {
                var attributes = type.GetCustomAttributes(typeof(HelpAttribute));
                if (attributes == null)
                    continue;

                allAttributes.AddRange(attributes.Select(a => (HelpAttribute)a));
            }
            return allAttributes;
        }

        private static void FilterTargetsByHost(ref List<HelpAttribute> targets)
        {
            if (HostPlatform.IsWindows)
                targets = targets.Where(a => (a.Host & HostType.Windows) != 0).ToList();
            if (HostPlatform.IsOSX)
                targets = targets.Where(a => (a.Host & HostType.Mac) != 0).ToList();
            if (HostPlatform.IsLinux)
                targets = targets.Where(a => (a.Host & HostType.Linux) != 0).ToList();
        }

        private static void FilterTargetsByConditionVariable(ref List<HelpAttribute> targets)
        {
            targets = targets.Where(a => a.ConditionVariable).ToList();
        }

        private static void PrintOverview(bool specificTarget)
        {
            Console.WriteLine("Entry point into Unity's build system.");
            Heading();
            Console.WriteLine($"Usage: {JamCmd} [target...] [-sNAME=VALUE...] [-flags]");
            Normal();
            Console.WriteLine();
            Console.WriteLine("Case sensitive build target names are passed on the command line.");
            Console.WriteLine();
            if (specificTarget) // help for V2 target, which doesn't need -sPLATFORM
                Console.WriteLine("Additional options can be passed via \"-sNAME=VALUE\" arguments. Most common one is CONFIG (used to indicate debug or release build).");
            else
                Console.WriteLine("Additional options can be passed via \"-sNAME=VALUE\" arguments. Most common ones are CONFIG (used to indicate debug or release build) and PLATFORM (used to indicate 64 vs 32 bit build).");
            Console.WriteLine("Example: build Mac standalone player with Release config -- \"jam MacPlayer -sCONFIG=release\".");
            Console.WriteLine();
            if (!specificTarget)
            {
                Console.WriteLine("Standalone player builds using v1 buildcode generally understand these options:");
                Bold(); Console.Write("  -sDEVELOPMENT_PLAYER"); Normal();
                Console.WriteLine("=1 (default) or 0; zero turns off profiler etc.");
                Console.WriteLine("    This matches 'Development Build' option in Unity build window.");
                Console.WriteLine("    Some platforms use -sCONFIG=master to achieve the same effect.");
                Bold(); Console.Write("  -sSCRIPTING_BACKEND"); Normal();
                Console.WriteLine("=mono (default), il2cpp or dotnet; picks scripting backend.");
                Console.WriteLine("    Note that some platforms only have one (e.g. WebGL).");
                Console.WriteLine("  -sLUMP=1 (default) or 0; controls 'lumped/unity' C++ builds.");
                Console.WriteLine();
                Console.WriteLine("Standalone player builds using v2 buildcode specify the above options with");
                Console.WriteLine("target suffixes, and those can be listed with");
                Console.WriteLine();
                Heading(); Console.WriteLine($"{JamCmd} --help <target>"); Normal();
                Console.WriteLine();
                Console.WriteLine("Targets with the latter situation are listed with (v2) below.");
                Console.WriteLine();
            }

            Console.WriteLine("Other common options:");
            Console.WriteLine("  -sDEBUG=1 allows debugging build C# code with JetBrains Rider IDE.");
            Console.WriteLine("  -sOUTPUT_DEPGRAPH=1 dumps interactive HTML of the build dependency graph.");
            Console.WriteLine("  -a Will do 'full rebuild'.");
            Console.WriteLine("  -dax Will print full command lines for all executed actions.");
            Console.WriteLine();
            Console.WriteLine("See http://internaldocs.hq.unity3d.com/build_system/BuildingViaJam for more details");
            Console.WriteLine();
            if (!specificTarget)
                Console.WriteLine("A list of common build targets on your host OS:");
        }

        public static void Bold()
        {
            if (HostPlatform.IsWindows)
                Console.ForegroundColor = ConsoleColor.White;
            else
                Console.Write("\x1b[1m");
        }

        public static void Heading()
        {
            if (HostPlatform.IsWindows)
                Console.ForegroundColor = ConsoleColor.Yellow;
            else
            {
                Bold();
                Underline();
                Console.Write(("\x1b[33m"));
            }
        }

        public static void Light()
        {
            if (HostPlatform.IsWindows)
                Console.ForegroundColor = ConsoleColor.DarkGray;
            else
                Console.Write("\x1b[90m");
        }

        public static void Normal()
        {
            Console.ResetColor();
            if (!HostPlatform.IsWindows)
                Console.Write("\x1b[0m");
        }

        static void Underline()
        {
            if (!HostPlatform.IsWindows)
                Console.Write("\x1b[4m");
        }
    }
}
