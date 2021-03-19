using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Reflection;
using System.Runtime.CompilerServices;
using Bee.Core;
using Bee.CSharpSupport;
using Bee.DotNet;
using Bee.StandaloneDriver;
using Bee.Tools;
using Bee.TundraBackend;
using JamSharp.Runtime;
using Mono.Options;
using Newtonsoft.Json.Linq;
using NiceIO;
using Unity.BuildSystem.CSharpSupport;
using Unity.BuildTools;

//This is the driver program for when Bee is used out of the context of building Unity.
namespace Bee
{
    public class StandaloneBeeDriver
    {
        private const string ExampleBuildCsFileName = "Build.bee.cs";

        private static NPath BeeExe => new NPath(typeof(StandaloneBeeDriver).Assembly.Location).RelativeTo(NPath.CurrentDirectory);
        private static string BeeBranchInfo => ResourceHelper.ReadTextFromResource(typeof(StandaloneBeeDriver).Assembly, "branchinformation").Trim();

        internal static TopLevelBeeCommand[] Subcommands = DynamicLoader.FindAndCreateInstanceOfEachInAllAssemblies<TopLevelBeeCommand>("*.dll", true).ToArray();

        internal static TopLevelBeeCommand DefaultSubcommand = Subcommands.First(cmd => cmd.Name == "build");

        public static int Main(string[] args)
        {
            //DotnetCLR and Mono have different behaviour in what they output to stdout/stderr when an unhandled
            //exception happens. Mono writes all of it to stdout, while CoreCLR (Desktop) outputs to stderr.
            //Unifying this here, by catching all exceptions, and writing them to stdout, so we have consistent
            //behaviour across vm's and across platforms
            try
            {
                RealMain(args);
                return 0;
            }
            catch (Exception e)
            {
                Console.WriteLine(e.ToString());
                return 1;
            }
        }

        // Global options, which are parsed and applied prior to selecting a TopLevelBeeCommand.
        // Individual TopLevelBeeCommand subclasses may then have their own options on top of these.
        private static OptionSet GlobalOptions { get; } = new OptionSet()
        {
            { "root-artifacts-path=", $"path to the directory for buildsystem intermediate files, default '{Configuration.DefaultRootArtifactsPath}'", p => Configuration.RootArtifactsPath = p }
        };

        static void RealMain(string[] args)
        {
            List<string> unparsedArgs = null;
            try
            {
                unparsedArgs = GlobalOptions.Parse(args);
            }
            catch (OptionException e)
            {
                Console.Write("Error parsing: ");
                Console.WriteLine(e.Message);
                Console.WriteLine("Try `bee --help` for more information.");
                Environment.Exit(1);
            }

            var arg0 = unparsedArgs.FirstOrDefault();

            if (arg0 == "--help" || arg0 == "-h")
            {
                ShowBeeHelp();
                return;
            }
            if (arg0 == "--version")
            {
                ShowBeeVersion();
                return;
            }

            var subcommand = arg0 == null ? null : Subcommands.FirstOrDefault(cmd => arg0 == cmd.Name || arg0 == cmd.Abbreviation);
            if (subcommand != null)
                subcommand.Execute(unparsedArgs.Skip(1).ToArray());
            else
                DefaultSubcommand.Execute(unparsedArgs.ToArray());
        }

        public static void ShowBeeVersion()
        {
            Console.WriteLine($@"Bee buildsystem. Powered by Tundra by Andreas Fredriksson.  Built from: {BeeBranchInfo}");
        }

        public static void ShowBeeHelp()
        {
            ShowBeeVersion();
            Console.WriteLine();
            Console.WriteLine("list of commands:");
            Console.WriteLine();

            foreach (var subcommand in Subcommands)
            {
                var arg0 = subcommand.Name;
                if (subcommand.Abbreviation != null)
                    arg0 = subcommand.Abbreviation + " | " + arg0;

                Console.WriteLine($" {arg0,-9}  {subcommand.Description}");
            }

            Console.WriteLine();
            Console.WriteLine($"'{DefaultSubcommand.Name}' is the default command. Use 'bee COMMAND --help' for help on specific commands.");
        }

        public static void BuildMain(string[] args)
        {
            //A typical flow between bee and tundra goes like this:
            //
            //- user invokes bee.exe
            //- bee.exe invokes tundra
            //- tundra will realize it does not have a buildgraph yet, and will call bee.exe requesting one to be made.
            //- now it's bee.exe's job to create a .json file. In order to do that, it needs to have the user's bee.cs files compiled
            //   and then run them.
            //- how do we compile the bee.cs files? We use tundra for that as well!
            //- bee.exe, in the role of being asked to create the buildfgraph json, invokes tundra with -R artifacts/buildprogram/buildprogram.dag
            //- tundra realizes that in order to create the buildprogram, it also doesn't have a buildgraph for that yet. so it again asks bee.exe to make that
            //- bee.exe realizes its asked to spit out a json for the buildgraph, and luckily it knows how to do that, and will write out the json file.
            //- tundra can now use the buildgraph json file to build buildprogram.exe
            //- bee.exe that was supposed to generate a .json file for the real build, can now do that by running buildprogram.exe
            //- tundra that was asked to do a real build, can now do that using the generated json file, and will make a nice shiny build
            //
            //That sounded complicated right? subsequent builds are much simpler:
            //- user invokes bee.exe
            //- bee.exe invokes tundra
            //- tundra realizes the graph it has is up to date, and immediately builds

            BeeOptions.ParseCommandLine(args);

            var configfile = BeeOptions.BuildProgramRoot.Combine("bee.config").MakeAbsolute();
            Config = configfile.FileExists() ? JObject.Parse(configfile.ReadAllText()) : new JObject();
            var frontEndOptions = Environment.GetEnvironmentVariable("TUNDRA_FRONTEND_OPTIONS");

            Directory.SetCurrentDirectory(BeeOptions.BuildProgramRoot.MakeAbsolute().ToString(SlashMode.Native));

            MultiProcessDebugHelper.WriteLine($"BeeDriver invoked with args: {args.Length} and frontendOptions-envvar {frontEndOptions}");

            if (ConsoleColors.ShouldWeOutputColors && !BeeOptions.disableColors)
                Environment.SetEnvironmentVariable(ConsoleColors.EnvironmentVariableNameIndicatingCanHandleColors, "1");
            if (BeeOptions.disableColors)
                Environment.SetEnvironmentVariable(ConsoleColors.EnvironmentVariableNameIndicatingCanHandleColors, "0");

            if (BeeOptions.beeMode == BeeMode.InvokedDirectlyByUser)
            {
                //okay, we are being invoked directly by the user. this case is simple, we just invoke tundra, and let tundra know
                //how it can have a .json generated for its graph
                UnpackTundraIfNecissery();

                InvokeTundra(BuildGraphDagFile, $"{BeeInvocationString} build --beemode={BeeMode.ToCreateBuildGraph} {BeeBehaviourFlags().SeparateWithSpace()}", BeeOptionsTranslatedToTundraBehaviourFlags.Concat(BeeOptions.extra).ToArray());
                return;
            }

            NPath targetJson = GetTargetJsonPathFromFrontendOptions(frontEndOptions);

            switch (BeeOptions.beeMode)
            {
                case BeeMode.ToCreateBuildGraph:
                    CreateBuildGraph(targetJson);
                    return;
                case BeeMode.ToCreateBuildProgramGraph:
                    CreateGraphForBuildProgram(targetJson);
                    return;
            }

            throw new Exception($"Bee.exe being invoked as frontend, with a targetJson file it doesn't know how to make: {targetJson}");
        }

        static JObject Config { get; set; }

        private static IEnumerable<string> BeeBehaviourFlags()
        {
            if (BeeOptions.debug)
                yield return "-d";
            if (BeeOptions.forceRecreateDepgraph)
                yield return "-f";
            if (BeeOptions.dontRecyclePreviousResults)
                yield return "-l";
            if (BeeOptions.verbose)
                yield return "-v";
            yield return $"--root-artifacts-path={Configuration.RootArtifactsPath}";
            yield return $"--bee-driver-buildprogramroot={BeeOptions.BuildProgramRoot}";
        }

        private static NPath BuildProgramPath => Configuration.RootArtifactsPath.Combine("buildprogram/buildprogram.exe");
        private static NPath BuildProgramDagFile => Configuration.RootArtifactsPath.Combine("buildprogram/tundra_buildprogram.dag");
        private static NPath BuildGraphDagFile => Configuration.RootArtifactsPath.Combine("tundra.dag");

        enum BeeMode
        {
            InvokedDirectlyByUser,
            ToCreateBuildGraph,
            ToCreateBuildProgramGraph
        }

        static class BeeOptions
        {
            public static NPath BuildProgramRoot = ".";
            public static bool verbose = false;
            public static bool debug = false;
            public static bool forceRecreateDepgraph = false;
            public static List<string> extra;
            public static bool dontRecyclePreviousResults = false;
            public static bool printHelp = false;
            public static int jobCount = 0;
            public static BeeMode beeMode = BeeMode.InvokedDirectlyByUser;
            public static bool disableColors = false;
            public static bool silenceIfPossible = false;

            public static void ParseCommandLine(string[] args)
            {
                var options = new OptionSet
                {
                    { "bee-driver-buildprogramroot=", "internal only.", p => BuildProgramRoot = p, true },
                    { "d", "print many decision making steps",  b => { debug = true; } },
                    { "v", "print more detailed information of each node built", (v) => { verbose = true; } },
                    { "f", "force recreating of the dependency graph", (b) => { forceRecreateDepgraph = true; }},
                    { "l", "dont recycle any results from previous builds", (b) => { dontRecyclePreviousResults = true; forceRecreateDepgraph = true; }},
                    { "j:", "maximum number of simultaneous nodes to build in parallel", (int amount) => { jobCount = amount; }},
                    { "silence-if-possible", "don't emit a conclusion message if the build succeeded with no items updated", (b) => { silenceIfPossible = true; }},
                    { "no-colors", "disable colors in output", b => disableColors = true},
                    { "beemode=", "which mode bee is invoked in. internal only", (BeeMode mode) => { beeMode = mode; }},
                    { "?|help", "print help", (b) => { printHelp = true; }}
                };

                try
                {
                    // parse the command line
                    extra = options.Parse(args);
                }
                catch (OptionException e)
                {
                    // output some error message
                    Console.Write("Error parsing: ");
                    Console.WriteLine(e.Message);
                    Console.WriteLine("Try `bee --help' for more information.");
                    Environment.Exit(1);
                }

                if (printHelp)
                {
                    Console.WriteLine($"Bee buildsystem. Powered by Tundra by Andreas Fredriksson.  Built from: {BeeBranchInfo}");
                    Console.WriteLine();
                    options.WriteOptionDescriptions(Console.Out);
                    Environment.Exit(0);
                }
            }
        }

        private static IEnumerable<string> BeeOptionsTranslatedToTundraBehaviourFlags
        {
            get
            {
                if (BeeOptions.debug)
                    yield return "-D";
                if (BeeOptions.verbose)
                    yield return "-v";
                if (BeeOptions.forceRecreateDepgraph)
                    yield return "-f";
                if (BeeOptions.dontRecyclePreviousResults)
                    yield return "-l";
                if (BeeOptions.jobCount > 0)
                    yield return $"-j {BeeOptions.jobCount}";
                if (BeeOptions.silenceIfPossible)
                    yield return "--silence-if-possible";

                yield return $"-p {Configuration.RootArtifactsPath}/profile.json";
            }
        }

        private static void InvokeTundra(NPath tundraDag, string frontendCommandLine, IEnumerable<string> tundraArgs)
        {
            var result = TundraInvoker.Invoke(TundraBinaryPath, tundraDag, NPath.CurrentDirectory, frontendCommandLine, Shell.StdMode.Stream, false, tundraArgs);
            if (result.ExitCode != 0)
                Environment.Exit(result.ExitCode);
        }

        private static string BeeInvocationString
        {
            get
            {
                var monoExe = Environment.GetEnvironmentVariable("MONO_EXECUTABLE") ?? "mono";

                var beeInvocationString = HostPlatform.IsWindows
                    ? $"{BeeExe.ToString(SlashMode.Native)}"
                    : $"{monoExe} --debug {BeeExe}";
                return beeInvocationString;
            }
        }

        private static NPath GetTargetJsonPathFromFrontendOptions(string frontEndOptions)
        {
            var split = frontEndOptions.Split(' ');
            if (split.Length < 3)
                throw new ArgumentException("Frontendoptions had fewer than three elements: " + frontEndOptions);
            return split[2];
        }

        private static void CreateBuildGraph(NPath targetJson)
        {
            InvokeTundra(BuildProgramDagFile, $"{BeeInvocationString} build --beemode={BeeMode.ToCreateBuildProgramGraph}", BeeOptionsTranslatedToTundraBehaviourFlags);

            var tundraBackend = new TundraBackend.TundraBackend(Configuration.RootArtifactsPath, "Bee");
            Backend.Current = tundraBackend;

            using (new GraphBuildingNPathHooks())
            {
                var assembly = Assembly.LoadFrom(BuildProgramPath.ToString(SlashMode.Native));
                assembly.EntryPoint.Invoke(null, Array.Empty<object>());
            }

            RegisterFrontendInfluencingFilesWithTundraBackend(tundraBackend);

            tundraBackend.AddExtensionToBeScannedByHashInsteadOfTimeStamp("c", "cpp", "h", "cs", "hpp");
            tundraBackend.Write(targetJson);
        }

        private static void CreateGraphForBuildProgram(NPath targetJson)
        {
            var tundraBackend = new TundraBackend.TundraBackend(Configuration.RootArtifactsPath.Combine("buildprogram"), "");
            Backend.Current = tundraBackend;

            using (new GraphBuildingNPathHooks())
            {
                var projectRoot = NPath.CurrentDirectory;
                var buildCsFiles = BuildCsFiles;

                var exampleBody = ResourceHelper
                    .ReadTextFromResource(typeof(StandaloneBeeDriver).Assembly, ExampleBuildCsFileName)
                    .Replace("Main_RemoveMe", "Main");

                if (!buildCsFiles.Any())
                {
                    //we need to rename Main_RemoveMe, as we keep the example inside Bee assembly to make sure it keeps on compiling, and that cannot have two main methods

                    projectRoot.Combine(ExampleBuildCsFileName).WriteAllText(exampleBody);

                    buildCsFiles = new NPath[] {ExampleBuildCsFileName};
                }

                NPath buildProgramProjectFilesDirectory =
                    (string)Config["BuildProgramProjectFilesDirectory"] ?? BeeOptions.BuildProgramRoot;
                var buildProgram = new CSharpProgram()
                {
                    ProjectFilePath = buildProgramProjectFilesDirectory.Combine("build.gen.csproj"),
                    Path = BuildProgramPath,
                    PrebuiltReferences = {BeeExe.Parent.Files("*.dll").Append(BeeExe).Select(p => p.MakeAbsolute())},
                    Sources = {buildCsFiles},
                    LanguageVersion = "7.2",
                    Framework = Framework.Framework46,
                    CopyReferencesNextToTarget = false
                };

                RegisterFrontendInfluencingFilesWithTundraBackend(tundraBackend);

                buildProgram.SetupSpecificConfiguration(new CSharpProgramConfiguration(CSharpCodeGen.Debug, CSharpProgram.DetermineCompiler()));

                var buildGenSln = buildProgramProjectFilesDirectory.Combine("build.gen.sln");
                var solutionFile = new SolutionFileBuilder().WithProgram(buildProgram).WithPath(buildGenSln).Complete();

                var s = solutionFile.Path;

                tundraBackend.AddAliasDependency("build", buildProgram.Path);
                tundraBackend.AddAliasDependency("build", s);
                foreach (var csproj in solutionFile.ProjectFiles.Select(a => a.Path))
                    tundraBackend.AddAliasDependency("build", csproj);

                bool onlyHasUnmodifiedExamplebeeFile =
                    BuildCsFiles.Length == 1 && BuildCsFiles.Single().FileName == ExampleBuildCsFileName &&
                    BuildCsFiles.Single().MakeAbsolute().ReadAllText() == exampleBody;

                if (onlyHasUnmodifiedExamplebeeFile)
                {
                    Console.WriteLine("Looks like this is the first time you're running bee in this project!");
                    Console.WriteLine("To get you started easily, we've already created a Build.bee.cs file");
                    Console.WriteLine(
                        "As well as a project and solution file, which you can open with your favorite C# ide");
                    Console.WriteLine(
                        $"The solution is here: {buildGenSln.MakeAbsolute().ToString(SlashMode.Native)}, open it in Rider or Visual Studio");
                    Console.WriteLine();
                    Console.WriteLine("Have fun using Bee!");
                    Console.WriteLine();

                    //if the user never modified the example script, we continue to rerun this buildprogram, so that the help message above keeps on printing
                    //until a modification to the template buildscript has been made.
                    tundraBackend.SetForceDagRebuild(true);
                }
            }

            tundraBackend.Write(targetJson);
        }

        private static void RegisterFrontendInfluencingFilesWithTundraBackend(TundraBackend.TundraBackend tundraBackend)
        {
            tundraBackend.AddFileSignature(new NPath("bee.config").MakeAbsolute());
            tundraBackend.AddFileSignature(BeeExe.Parent.Files("Bee.*.dll"));
            tundraBackend.AddFileSignature(BuildCsFiles.Select(p => p.MakeAbsolute()).ToArray());
            BeeDirs.ForEach(tundraBackend.AddGlobSignature);
            tundraBackend.AddFileSignature(BeeExe);
        }

        private static readonly Lazy<NPath[]> _buildCsFiles = new Lazy<NPath[]>(() =>
        {
            var result = new List<NPath>();
            var beeFiles = Config["BeeFiles"];
            if (beeFiles != null)
            {
                foreach (var token in beeFiles)
                {
                    NPath beeFile = (string)token;
                    if (beeFile.FileExists())
                    {
                        result.Add(beeFile);
                        Backend.Current.RegisterFileInfluencingGraph(beeFile);
                    }

                    if (beeFile.DirectoryExists())
                    {
                        foreach (var file in beeFile.Files("*.bee.cs", true))
                        {
                            result.Add(file);
                            Backend.Current.RegisterFileInfluencingGraph(file);
                        }
                    }
                }

                return result.ToArray();
            }


            Backend.Current.RegisterGlobInfluencingGraph(BeeOptions.BuildProgramRoot);
            foreach (var dir in BeeDirs)
                Backend.Current.RegisterGlobInfluencingGraph(dir);

            var filesInRoot = BeeOptions.BuildProgramRoot.Files("*bee.cs", false);
            return filesInRoot.Concat(BeeDirs.SelectMany(d => d.Files("*bee.cs", true))).ToArray();
        });

        private static IEnumerable<NPath> BeeDirs
        {
            get
            {
                return BeeOptions.BuildProgramRoot.Directories().Where(d => d.FileName.Contains("bee") || d.FileName.Contains("Bee"));
            }
        }

        private static NPath[] BuildCsFiles => _buildCsFiles.Value;

        private static void UnpackTundraIfNecissery()
        {
            if (TundraBinaryPath != null && TundraBinaryPath != UnpackedTundraPath)
                return;

            if (UnpackedTundraPath.FileExists() && UnpackedTundraPath.GetLastWriteTimeUtc() == BeeExe.GetLastWriteTimeUtc())
                return;

            try
            {
                ResourceHelper.WriteResourceToDisk(typeof(TundraBackend.TundraBackend).Assembly, $"tundra_{TundraPlatformName()}", UnpackedTundraPath);
                UnpackedTundraPath.SetLastWriteTimeUtc(BeeExe.GetLastWriteTimeUtc());
            }
            catch (IOException)
            {
                if (UnpackedTundraPath.FileExists())
                    return;
                throw;
            }
            if (!HostPlatform.IsWindows)
                Shell.Execute("/bin/chmod", $"+x {UnpackedTundraPath.InQuotes()}");
        }

        private static NPath TundraBinaryPath
        {
            get
            {
                var plat = TundraPlatformName();
                var candidates = new[]
                {
                    UnpackedTundraPath,
                    Paths.ProjectRoot.Combine($"External/tundra/builds/{plat}/tundra2{HostPlatform.Exe}"),
                    Paths.ProjectRoot.Combine($"External/tundra/{plat}/tundra2{HostPlatform.Exe}")
                };

                return candidates.FirstOrDefault(c => c.FileExists());
            }
        }

        private static NPath UnpackedTundraPath => Configuration.RootArtifactsPath.Combine("tundra2" + HostPlatform.Exe);

        private static string TundraPlatformName()
        {
            if (HostPlatform.IsLinux)
                return "linux";
            if (HostPlatform.IsOSX)
                return "darwin";
            if (HostPlatform.IsWindows)
                return "MSWin32";
            throw new ArgumentException();
        }
    }
}
