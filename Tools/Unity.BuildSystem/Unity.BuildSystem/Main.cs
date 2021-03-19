using System;
using System.Collections.Generic;
using System.Linq;
using Bee.Core;
using External.Jamplus.builds.bin;
using JamSharp.JamState;
using JamSharp.Runtime;
using NiceIO;
using Unity.BuildSystem;
using Unity.BuildSystem.Common;
using Unity.BuildSystem.JamStateToTundraConverter;
using Unity.BuildTools;
using Unity.TinyProfiling;

static class BuildProgram
{
    static IDisposable _TinyProfilerSectionForBindingPhase;

    public static void Main()
    {
        NPath.WithFrozenCurrentDirectory(NPath.CurrentDirectory);

        VerifyCurrentDirectoryIsUnityRoot();
        VerifyUnityRootPathLength();

        JamStateBuilder jamStateBuilder = null;
        TinyProfiler.Section("TopLevel");

        using (TinyProfiler.Section("SetupGlobalVariables"))
        {
            GlobalVariables.Singleton["BACKEND"].Assign("JAM");
            GlobalVariables.Singleton["OSPLAT"].Assign(Environment.Is64BitProcess ? "X64" : "X86");

            GlobalVariables.Singleton["CWD"].Assign(Environment.CurrentDirectory);
            GlobalVariables.Singleton["OS"].Assign(HostPlatform.IsWindows ? "NT" : HostPlatform.IsOSX ? "MACOSX" : "LINUX");
            if (HostPlatform.IsWindows) GlobalVariables.Singleton["NT"].Assign("true");
            if (HostPlatform.IsOSX || HostPlatform.IsLinux) GlobalVariables.Singleton["UNIX"].Assign("true");

            var envVars = Environment.GetEnvironmentVariables();
            foreach (string key in envVars.Keys)
            {
                if (key == "OS")
                    continue;

                if (!ShouldLeakEnvironmentVariableIntoBuildProgramVariablesFor(key))
                    continue;

                if (GlobalVariables.BlacklistedGlobalVariables.Contains(key.ToUpperInvariant()))
                    continue;

                var splitChar = ' ';
                // Any env var ending in 'path' gets split differently
                if (key.EndsWith("path", StringComparison.InvariantCultureIgnoreCase))
                    splitChar = HostPlatform.IsWindows ? ';' : ':';
                GlobalVariables.Singleton[key.ToUpperInvariant()].Assign(((string)envVars[key]).Split(splitChar));
            }

            // Parse the commandline, skipping the first arg which is the buildprogram path
            var cmdLine = new JamStyleCommandLineParser(Environment.GetCommandLineArgs().Skip(1));

            GlobalVariables.Singleton["JAM_COMMAND_LINE_TARGETS"].Assign(cmdLine.TargetsToBuild);

            foreach (var jamVar in cmdLine.Variables)
                if (!GlobalVariables.BlacklistedGlobalVariables.Contains(jamVar.Key))
                    GlobalVariables.Singleton[jamVar.Key].Assign(jamVar.Value);

            var tundraDagJson = RetrieveTundraDagJsonFile();
            if (tundraDagJson != null)
                GlobalVariables.Singleton["TUNDRA_DAG_FILE"].AssignIfEmpty(tundraDagJson);
        }

        using (TinyProfiler.Section("Main"))
        {
            TinyProfiler.ConfigureOutput(GetProfilerOutputPath());

            var jamBackend = new JamBackend();
            Backend.Current = jamBackend;

            using (TinyProfiler.Section("Initialize"))
                RuntimeManager.Initialize();

            jamStateBuilder = new JamStateBuilder();
            RuntimeManager.ParallelJamStateBuilder = jamStateBuilder;

            var emittingEmptyDepGraph = false;
            try
            {
                RealMain();
            }
            catch (AbortRestOfDependencyGraphSetup)
            {
                emittingEmptyDepGraph = true;
            }

            Rules.Cleanup();

            RuntimeManager.OnFinishedBindingPhase += () =>
            {
                var jamState = new Lazy<JamState>(() =>
                {
                    var builder = jamStateBuilder;
                    if (emittingEmptyDepGraph)
                    {
                        // A process that invokes us (e.g. tundra) wants _some_ build graph to be produced,
                        // but we want to effectively just "quit the build" (e.g. after printing help).
                        // So create a tiny build graph that basically has nothing.
                        builder = new JamStateBuilder();
                        builder.NotFile("all");
                        builder.Depends("all", "artifacts");
                    }

                    return builder.Build();
                });
                if (ShouldOutputDepgraph())
                {
                    using (TinyProfiler.Section("Writing Depgraph"))
                        new JamStateHtmlWriter().WriteFor(jamState.Value);
                }

                _TinyProfilerSectionForBindingPhase.Dispose();

                var targets = emittingEmptyDepGraph ? "all" : GlobalVariables.Singleton["JAM_COMMAND_LINE_TARGETS"];

                var tundraBuildPath = new NPath(GlobalVariables.Singleton["TUNDRA_DAG_FILE"].Single());

                var tundraFilesToReadSignaturesFrom = new NPath[]
                {
                    //the dag that builds the buildprogram for the buildprogram
                    "artifacts/buildprogram/tundra_buildprogram.dag.json",

                    //the dag that builds buildsystem.exe
                    "artifacts/tundra.dag.json"
                };

                new Converter().Convert(
                    jamState.Value,
                    targets.ToArray(),
                    Paths.UnityRoot.Combine(tundraBuildPath),
                    jamBackend, AllFilesInfluencingBuildProgram.Provide(), tundraFilesToReadSignaturesFrom);
            };
        }

        _TinyProfilerSectionForBindingPhase = TinyProfiler.Section("BindingPhase");
        RuntimeManager.InvokeOnFinishedBindingPhase();
    }

    private static bool ShouldLeakEnvironmentVariableIntoBuildProgramVariablesFor(string key)
    {
        if (key == "SCE_ORBIS_SDK_DIR")
            return true;
        if (key == "SCE_ROOT_DIR")
            return true;
        if (key == "NINTENDO_SDK_ROOT")
            return true;

        if (key == "PLATFORM")
            return false;
        if (key == "CONFIG")
            return false;
        if (key == "OS")
            return false;
        if (key == "SDKROOT")
            return false;

        //todo: flip this default to false once we have found the remaining places where we depend on environment variables.
        return true;
    }

    public static NPath RetrieveTundraDagJsonFile()
    {
        var environmentVariable = Environment.GetEnvironmentVariable("TUNDRA_FRONTEND_OPTIONS");
        if (environmentVariable == null)
            return "artifacts/tundra/dummy.dag.json";

        var tundraOptions = environmentVariable.Split(new[] {' '});

        if (tundraOptions.Length == 0)
            throw new ArgumentException("Expected TUNDRA_FRONTEND_OPTIONS env var to be set");

        if (tundraOptions.Length < 3)
            throw new Exception("Expected TUNDRA_FRONTEND_OPTIONS to have 3 space seperated strings");
        return tundraOptions[2];
    }

    static void VerifyCurrentDirectoryIsUnityRoot()
    {
        if (NPath.CurrentDirectory != Paths.UnityRoot)
            Errors.Exit(
                $"You're trying to run the build from workingdirectory: {NPath.CurrentDirectory}. We only support building from the repo root. This is caused most likely by outdated project files.\n Rebuild your projectfiles with: jam NativeProjectFiles");
    }

    static void VerifyUnityRootPathLength()
    {
        if (HostPlatform.IsWindows)
        {
            var length = Paths.UnityRoot.ToString().Length;
            const int errorLength = 64;
            const int warnLength = 48;
            if (length > errorLength)
                Errors.Exit($"On Windows the Unity checkout should be at short enough path (recommend under {warnLength} chars). Yours is {length}, which is way too much.");
            if (length > warnLength)
                Errors.PrintWarning($"On Windows the Unity checkout should be at short enough path (recommend under {warnLength} chars). Yours is a bit longer at {length}; some builds might fail due to too long paths.");
        }
    }

    private static NPath GetProfilerOutputPath()
    {
        var targets = GlobalVariables.Singleton["JAM_COMMAND_LINE_TARGETS"];
        return new NPath($"artifacts/BuildProfile/{targets}_frontend.json").MakeAbsolute().EnsureParentDirectoryExists();
    }

    static bool ShouldOutputDepgraph()
    {
        return GlobalVariables.Singleton["OUTPUT_DEPGRAPH"].Elements.FirstOrDefault() == "1";
    }

    public static void RealMain()
    {
        using (TinyProfiler.Section("InitConvertedJamFiles"))
            ConvertedJamFileBase.InitializeAllConvertedJamFiles();

        var ndaRoot = GlobalVariables.Singleton["NDA_PLATFORM_ROOT"].FirstOrDefault();
        if (ndaRoot != null)
        {
            var ndaRootPath = new NPath(ndaRoot);
            if (ndaRootPath.IsChildOf(NPath.CurrentDirectory))
                ndaRootPath = ndaRootPath.RelativeTo(NPath.CurrentDirectory);
            Jambase.PlatformRootPath = ndaRootPath.ToString();
        }

        Jambase.TopLevel();

        if (ShouldOutputHelpJSON())
        {
            Help.PrintHelpJSON();
            throw new AbortRestOfDependencyGraphSetup();
        }

        if (ShouldOutputHelp())
        {
            Help.Print();
            throw new AbortRestOfDependencyGraphSetup();
        }
    }

    static bool ShouldOutputHelp()
    {
        return
            GlobalVariables.Singleton["OUTPUT_HELP"].Elements.FirstOrDefault() == "1" ||
            // note, output help when just "./jam" is invoked, in that case the build
            // targets will be set to "all", which don't do anything useful in our case
            GlobalVariables.Singleton["JAM_COMMAND_LINE_TARGETS"] == "all";
    }

    static bool ShouldOutputHelpJSON()
    {
        return GlobalVariables.Singleton["OUTPUT_HELPJSON"].Elements.FirstOrDefault() == "1";
    }
}
