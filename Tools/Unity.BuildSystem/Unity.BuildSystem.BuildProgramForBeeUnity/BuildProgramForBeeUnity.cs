using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Bee.BuildProgram;
using Bee.Core;
using Bee.CSharpSupport.Unity;
using Bee.TundraBackend;
using NiceIO;
using Unity.BuildSystem.CSharpSupport;
using Unity.BuildTools;

namespace Unity.BuildSystem.BuildProgram
{
    class Startup
    {
        static void Main()
        {
            new BuildProgramForBeeUnity().SetupBuildProgramBuildProgram();
        }
    }

    public class BuildProgramForBeeUnity : BuildProgramForBee
    {
        protected CSharpProgram JamStateStorage;
        protected CSharpProgram BuildProgram;
        protected CSharpProgram JamSharpRuntime;
        protected CSharpProgram BuildSystemCommon;
        public CSharpProgram BuildSystemCommonTests;
        protected SolutionFile Solution;
        protected override string NewtonSoftJsonDll { get; } = "External/Json.NET/Bin/Net40/Newtonsoft.Json.dll";
        protected override NPath Moq { get; } = "External/Moq/Moq.dll";
        protected override string NUnitFramework => "External/NUnit/framework/nunit.framework.dll";
        protected override NPath BeeRoot { get; } = "Tools/Bee";
        protected override CSharpProgramConfiguration CSharpConfig { get; } = new CSharpProgramConfiguration(CSharpCodeGen.Debug, new UnityVersionedMcs());

        protected override CSharpProgram[] ProgramsContainsBeeApiTestBackend()
        {
            var enablingFile = new NPath("Tools/Unity.BuildSystem/disable_jamcore");
            bool useJamCore = !(enablingFile.FileExists());
            Backend.Current.RegisterFileInfluencingGraph(enablingFile);

            JamSharpRuntime = UnityCSharpProgramFor("JamSharp.Runtime")
                .WithProjectReference(BeeNiceIO)
                .WithProjectReference(BeeTools)
                .WithProjectReference(BeeCore)
                .WithDefines(useJamCore ? Array.Empty<string>() : new[] {"JAMCORE_DISABLED"});
            SetupTestAssemblyFor(BeeTools);

            BuildSystemCommon = UnityCSharpProgramFor("Unity.BuildSystem.Common")
                .WithProjectReference(BeeNiceIO)
                .WithProjectReference(BeeTools)
                .WithProjectReference(JamSharpRuntime)
                .WithProjectReference(BeeCore);

            JamStateStorage = UnityCSharpProgramFor("JamSharp.JamState")
                .WithProjectReference(BeeNiceIO)
                .WithProjectReference(BeeTools)
                .WithProjectReference(JamSharpRuntime)
                .WithProjectReference(BuildSystemCommon)
                .WithPrebuiltReference(NewtonSoftJsonDll)
                .WithSystemReference("System", "System.Web");
            SetupTestAssemblyFor(JamSharpRuntime);

            BuildSystemCommonTests = SetupTestAssemblyFor(BuildSystemCommon, JamStateStorage);

            return base.ProgramsContainsBeeApiTestBackend().Append(BuildSystemCommonTests).ToArray();
        }

        protected override void SetupUnityAssemblies()
        {
            // Set up Unity-specific Bee/Stevedore integration tests. (These are not tests for a specific assembly.)
            var beeStevedoreUnityTests = new NPath("Tools/Bee.Unity/Bee.Stevedore.Unity.Tests");
            SetupTestAssemblyFor(beeStevedoreUnityTests,
                cp =>
                {
                    foreach (var beefile in beeStevedoreUnityTests.Files("*_beefile.cs"))
                        cp.WithResource(beefile);
                },
                new List<CSharpProgram>()
                {
                    // Assemblies needed by HelloWorld_beefile.cs (not strictly
                    // needed as the file is compiled by Bee with the appropriate
                    // dependencies, but this way the file can be verified by the
                    // IDE and build process, too.)
                    BeeNiceIO,
                    BeeTools,
                    BeeCore,
                    BeeStevedore,
                    NativeProgramSupportCore,
                    NativeProgramSupportBuilding,
                    CSharpPrograms.Where(IsBeeToolchain),
                    // Assemblies actually needed by the test code.
                    BeeStevedoreProgram,
                    BeeTestHelpers,
                    _testAssemblies[BeeCore],
                    BeeStandaloneDriverTests,
                }.ToArray());

            var jamstateToTundraConverter = UnityCSharpProgramFor("Unity.BuildSystem.JamStateToTundraConverter")
                .WithProjectReference(BeeNiceIO)
                .WithProjectReference(BeeTools)
                .WithProjectReference(JamSharpRuntime)
                .WithProjectReference(BeeCore)
                .WithProjectReference(BeeTundraBackend)
                .WithProjectReference(BuildSystemCommon)
                .WithProjectReference(BeeStevedore)
                .WithProjectReference(JamStateStorage);
            SetupTestAssemblyFor(jamstateToTundraConverter);

            UnityCSharpProgramFor("Unity.BuildSystem.VisualStudio")
                .WithProjectReference(BeeNiceIO)
                .WithProjectReference(BeeTools)
                .WithProjectReference(JamSharpRuntime)
                .WithProjectReference(NativeProgramSupportCore)
                .WithProjectReference(NativeProgramSupportBuilding)
                .WithProjectReference(BuildSystemCommon)
                .WithProjectReference(BeeCore)
                .WithProjectReference(BeeDotNet)
                .WithProjectReference(CSharpSupport)
                .WithProjectReference(VisualStudioToolchain);

            NPath allFilesInfluencingBuildProgram = "artifacts/UnityBuildSystem/AllFilesInfluencingBuildProgram.cs";

            var buildSystemDirectoriesInPlatformFolders = new NPath("Platforms")
                .Directories()
                .Select(platformDir => platformDir.Combine("Unity.BuildSystem"))
                .Where(d => d.DirectoryExists())
                .ToArray();

            var filesFromBuildSystemCsFiles = FilesFromBuildSystemCsFiles();

            var allPrograms = CSharpPrograms.Where(p => !p.Path.ToString().EndsWith("Tests.dll")).ToArray();
            BuildProgram = UnityCSharpProgramFor("Unity.BuildSystem", "exe")
                .WithSource(filesFromBuildSystemCsFiles)
                .WithSource(buildSystemDirectoriesInPlatformFolders)
                .WithSource(allFilesInfluencingBuildProgram)
                .WithDefines(PlatformDefines.ToArray())
                .WithProjectReference(allPrograms.Where(p => !IsTestProgram(p)).Exclude(ToolchainTestInfrastructure).Exclude(BeeTestHelpers).Exclude(BeeExamples).ToArray())
                .WithPrebuiltReference(NewtonSoftJsonDll)
                .WithSystemReference("System.Xml.Linq")
                .WithSystemReference("System.Xml")
                .WithSystemReference("System")
                .WithCopyReferencesNextToTarget(true);


            var allTransitiveReferencesFor = NativeProgramAndToolchainTests.TransitiveReferencesFor(CSharpConfig).ToArray();
            SetupTestAssemblyFor(
                "Tools/Unity.BuildSystem/Unity.BuildSystem.CompilerTests",
                null, allTransitiveReferencesFor);

            BuildProgram.SetupSpecificConfiguration(CSharpConfig);

            SetupAllFilesInfluencingBuildProgram(allFilesInfluencingBuildProgram);
            Backend.Current.AddDependency(BuildProgram.Path, allFilesInfluencingBuildProgram);

            SetupTestAssemblyFor(BuildProgram).WithDefines(PlatformDefines.ToArray());

            UnityCSharpProgramFor("Unity.BeeRepoSync", "exe")
                .WithProjectReference(BeeTools, BeeNiceIO);

            var solutionFileBuilder = new SolutionFileBuilder()
                .WithPath("Tools/Unity.BuildSystem/Unity.BuildSystem.gen.sln");

            foreach (var csharpProgram in CSharpPrograms)
                solutionFileBuilder.WithProgram(SolutionFolderFor(csharpProgram), csharpProgram);

            Solution = solutionFileBuilder.Complete();

            Backend.Current.AddAliasToAliasDependency("ProjectFiles", "BuildSystemProjectFiles");
            Backend.Current.AddAliasDependency("BuildSystemProjectFiles", Solution.Path);
            Backend.Current.AddAliasDependency("BuildSystemProjectFiles", BuildProgram.ProjectFilePath);

            Backend.Current.AddAliasDependency("BuildProgram", BuildProgram.Path);
        }

        private static NPath[] FilesFromBuildSystemCsFiles()
        {
            var buildSystemCSJamFile = new NPath("Tools/Unity.BuildSystem/Unity.BuildSystem/BuildSystemCsFiles.jam");
            Backend.Current.RegisterFileInfluencingGraph(buildSystemCSJamFile);
            var lines = buildSystemCSJamFile.MakeAbsolute().ReadAllLines().Select(l => l.Trim());
            var csFiles = lines.Where(l => l.EndsWith(".cs") && !l.StartsWith("#")).ToNPaths().Where(f => f.FileExists());
            var globbed = new NPath[] {"Modules", "Projects/Jam", "Platforms"}.SelectMany(d => d.Files("*.jam.cs", true));
            return csFiles.Concat(globbed).ToArray();
        }

        private bool IsTestProgram(CSharpProgram cSharpProgram)
        {
            return cSharpProgram.Path.FileNameWithoutExtension.EndsWith(".Tests");
        }

        protected override IEnumerable<CSharpProgram> AssembliesCSharpSupportIntegrationTestsShouldReference()
        {
            yield return CSharpSupportUnity;
        }

        private CSharpProgram CSharpSupportUnity
        {
            get
            {
                return CSharpPrograms.Single(p =>
                    p.Path.FileName.StartsWith("Bee.CSharpSupport.Unity") && !p.Path.FileName.Contains("Tests"));
            }
        }

        protected override NPath ILRepackExePath => "External/ILRepack/builds/tools/ILRepack.exe";

        protected static string[] PlatformDefines => new NPath("Platforms").Directories()
        .Select(d => $"PLATFORM_IN_SOURCESTREE_{d.FileName.ToUpper()}").ToArray();

        protected override NPath NiceIOPath => "External/NiceIO";
        protected override NPath CecilPath => Cecil.Current40Ref;

        protected override void SetupAdditionalToolchains(CSharpProgram[] referencesForToolChainAssemblies)
        {
            var beeToolchainsFromPlatformsFolder = new NPath("Platforms").Directories()
                .SelectMany(platformDir => platformDir.DirectoriesWithCsFiles())
                .Where(d => d.FileName.StartsWith("Bee.Toolchain") && !d.FileName.Contains("Tests"))
                .Select(beeToolchainFolder => {
                    var program = CSharpProgramFor(beeToolchainFolder, "dll")
                        .WithProjectReference(referencesForToolChainAssemblies)
                        .WithSystemReference("Microsoft.CSharp"); // For support of the 'dynamic' keyword
                    var optionalResourceFolder = new NPath($"{beeToolchainFolder}/Resources");
                    if (optionalResourceFolder.DirectoryExists())
                        program = program.WithResource(optionalResourceFolder.Files(true));
                    return program;
                }
                )
                .ToArray();
            foreach (var program in beeToolchainsFromPlatformsFolder)
                SetupTestAssemblyFor(program);
        }

        protected void SetupAllFilesInfluencingBuildProgram(NPath allFilesInfluencingBuildProgram)
        {
            var allProjects = BuildProgram.TransitiveReferencesFor(CSharpConfig)
                .Append(BuildProgram).Distinct()
                .ToList();
            var sb = new StringBuilder();
            sb.AppendLine("using NiceIO;");
            sb.AppendLine("namespace Unity.BuildSystem {");
            sb.AppendLine("\tpublic static class AllFilesInfluencingBuildProgram");
            sb.AppendLine("\t{");
            sb.AppendLine("\t\tpublic static NPath[] Provide() {");
            sb.AppendLine("\t\t\treturn new NPath[] {");
            var allFiles = allProjects
                .SelectMany(p => CSharpProgram.SourcesIn(p.Sources.For(CSharpConfig)).Concat(p.PrebuiltReferences.For(CSharpConfig)))
                .ToList();

            allFiles.Add("UserOverride.jam");

            bool first = true;
            foreach (var s in allFiles)
            {
                sb.Append("\t\t\t\t");
                if (!first)
                    sb.Append(",");
                sb.AppendLine(s.InQuotes());
                first = false;
            }

            sb.AppendLine("\t\t\t};");
            sb.AppendLine("\t\t}");
            sb.AppendLine("\t}");
            sb.AppendLine("}");

            TextFile.Setup(allFilesInfluencingBuildProgram, sb.ToString());
        }

        protected override void OnAllCoreProgramsHaveBeenSetup()
        {
            var programsInToolsBeeUnity = new NPath("Tools/Bee.Unity").DirectoriesWithCsFiles().Select(
                d =>
                {
                    if (d.FileName.Contains("Tests"))
                        return null;
                    return BeeUnityCSharpProgramFor(d.FileName);
                }
                ).Where(p => p != null).ToArray();
            foreach (var program in programsInToolsBeeUnity)
                SetupTestAssemblyFor(program);
        }

        private static bool IsBeeUnityToolchain(CSharpProgram arg)
        {
            return arg.Path.FileName.EndsWith("Unity.dll");
        }

        protected override bool ShouldIncludeToolchainInBeeDistribution(CSharpProgram toolchain)
        {
            return !IsBeeUnityToolchain(toolchain);
        }

        protected CSharpProgram UnityCSharpProgramFor(NPath sourceDirectory, string extension = "dll")
        {
            return CSharpProgramFor($"Tools/Unity.BuildSystem/{sourceDirectory}", extension);
        }

        protected CSharpProgram BeeUnityCSharpProgramFor(NPath sourceDirectory, string extension = "dll")
        {
            var augments = CSharpPrograms
                .Where(c => sourceDirectory.FileName.StartsWith(c.Path.FileNameWithoutExtension)).ToArray();
            var result = CSharpProgramFor($"Tools/Bee.Unity/{sourceDirectory}", extension);
            result.References.Add(CSharpProgram.AllTransitiveReferencesFor(CSharpConfig, augments).Concat(augments));
            return result;
        }

        protected override NPath SetupFileWithVersionControlnformation()
        {
            NPath hgDirstate = ".hg/dirstate";
            if (!hgDirstate.FileExists())
                return null;

            NPath branchInfoFile = "artifacts/BeeStandaloneDriver/branchinformation";
            Backend.Current.AddAction("QueryBranchInformation",
                new[] {branchInfoFile},
                new[] {hgDirstate},
                $"hg log -r . -T \"{{node|short}} ({{branch}})\" > {branchInfoFile.InQuotes(SlashMode.Native)}",
                Array.Empty<String>(),
                environmentVariables: new Dictionary<string, string> {{"HGPLAIN", "1"}}
            );
            return branchInfoFile;
        }

        protected override string TundraPathFor(string platform)
        {
            switch (platform)
            {
                case "MacOS":
                    return "External/tundra/builds/darwin/tundra2";
                case "Linux":
                    return "External/tundra/builds/linux/tundra2";
                case "Windows":
                    return "External/tundra/builds/MSWin32/tundra2.exe";
                default:
                    throw new ArgumentException();
            }
        }

        protected override IEnumerable<CSharpProgram> ProgramsToIncludeInBeeBootstrap()
        {
            foreach (var p in base.ProgramsToIncludeInBeeBootstrap())
                yield return p;
            yield return CSharpPrograms.Single(p => p.Path.FileName == "Bee.CSharpSupport.Unity.dll");
            yield return BeeWhy;
        }

        static string SolutionFolderFor(CSharpProgram program)
        {
            if (program.Path.FileName.Contains(".Unity."))
                return "BeeUnity";
            if (program.Path.FileName.StartsWith("Bee.Toolchain."))
                return "BeeToolchain";
            if (program.Path.FileName.StartsWith("Bee") || program.Path.FileName.StartsWith("NiceIO"))
                return "Bee";
            return "Unity";
        }
    }
}
