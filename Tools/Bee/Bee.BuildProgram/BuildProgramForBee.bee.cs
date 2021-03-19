using System;
using System.Collections.Generic;
using System.Linq;
using Bee.Core;
using Bee.DotNet;
using Bee.Stevedore;
using NiceIO;
using Unity.BuildSystem.CSharpSupport;
using Unity.BuildTools;

namespace Bee.BuildProgram
{
    public abstract class BuildProgramForBee
    {
        protected abstract string NewtonSoftJsonDll { get; }
        protected abstract NPath Moq { get; }

        protected readonly Dictionary<CSharpProgram, CSharpProgram> _testAssemblies =
            new Dictionary<CSharpProgram, CSharpProgram>();

        public static NPath BuildSystemDir = new NPath($"artifacts/UnityBuildSystem");
        protected CSharpProgram NativeProgramSupportCore;
        protected CSharpProgram NativeProgramSupportBuilding;
        protected CSharpProgram NativeProgramSupportRunning;
        protected CSharpProgram ARMToolchain;
        protected CSharpProgram GNUToolchain;
        CSharpProgram LuminToolchain;
        protected CSharpProgram LLVMToolchain;
        CSharpProgram XcodeToolchain;
        protected CSharpProgram CSharpSupport;
        CSharpProgram UWPToolchain;
        protected CSharpProgram BeeCore;
        CSharpProgram BeeProjectGeneration;
        protected CSharpProgram BeeDotNet;
        protected CSharpProgram BeeStevedore;
        protected CSharpProgram BeeStevedoreProgram;
        protected CSharpProgram BeeNiceIO;
        protected CSharpProgram BeeTestHelpers;
        protected CSharpProgram BeeTools;
        protected CSharpProgram BeeTundraBackend;
        protected CSharpProgram BeeTundraBinaries;
        CSharpProgram BeeStandaloneDriver;
        protected CSharpProgram BeeStandaloneDriverTests;
        CSharpProgram BeeProgramRunnerCLI;
        protected CSharpProgram VisualStudioToolchain;
        protected CSharpProgram ToolchainTestInfrastructure;
        public readonly List<CSharpProgram> CSharpPrograms = new List<CSharpProgram>();
        CSharpProgram BeeTundraBackendTests;
        protected CSharpProgram NativeProgramAndToolchainTests;
        protected CSharpProgram BeeExamples;
        protected CSharpProgram BeeWhy;

        protected virtual CSharpProgramConfiguration CSharpConfig { get; } = new CSharpProgramConfiguration(CSharpCodeGen.Debug, CSharpProgram.DetermineCompiler());

        protected abstract string NUnitFramework { get; }

        public virtual void SetupBuildProgramBuildProgram()
        {
            BeeNiceIO = new CSharpProgram()
            {
                Path = BuildSystemDir.Combine("NiceIO/NiceIO.dll"),
                Sources = {NiceIOPath.Combine("NiceIO.cs")},
                CopyReferencesNextToTarget = false,
                Framework = Framework.Framework46,
                LanguageVersion = "7"
            };

            CSharpPrograms.Add(BeeNiceIO);

            _testAssemblies.Add(BeeNiceIO,
                SetupTestAssemblyFor($"{NiceIOPath}/NiceIO.Tests", null, new[] {BeeNiceIO}, new NPath[0]));

            BeeTestHelpers = BeeCSharpProgramFor("Bee.TestHelpers")
                .WithPrebuiltReference(NUnitFramework)
                .WithProjectReference(BeeNiceIO);

            BeeTools = BeeCSharpProgramFor("Bee.Tools")
                .WithProjectReference(BeeNiceIO)
                .WithSource("External/TinyProfiler");

            BeeCore = BeeCSharpProgramFor("Bee.Core")
                .WithProjectReference(BeeNiceIO)
                .WithProjectReference(BeeTools);
            var beeCoreTests = SetupTestAssemblyFor(BeeCore);

            BeeDotNet = BeeCSharpProgramFor("Bee.DotNet")
                .WithProjectReference(BeeNiceIO)
                .WithProjectReference(BeeTools)
                .WithProjectReference(BeeCore);

            BeeTundraBackend = BeeCSharpProgramFor("Bee.TundraBackend")
                .WithProjectReference(BeeNiceIO)
                .WithProjectReference(BeeTools)
                .WithProjectReference(BeeCore)
                .WithPrebuiltReference(NewtonSoftJsonDll);

            BeeTundraBinaries = BeeCSharpProgramFor("Bee.TundraBinaries")
                .WithResource(new EmbeddedResource()
            {
                PathToEmbed = TundraPathFor("MacOS"),
                Identifier = "tundra_darwin"
            })
                .WithResource(new EmbeddedResource()
                {
                    PathToEmbed = TundraPathFor("Linux"),
                    Identifier = "tundra_linux"
                })
                .WithResource(new EmbeddedResource()
                {
                    PathToEmbed = TundraPathFor("Windows"),
                    Identifier = "tundra_MSWin32"
                });

            BeeStevedoreProgram = BeeCSharpProgramFor("Bee.Stevedore.Program", ".exe")
                .WithProjectReference(BeeNiceIO)
                .WithProjectReference(BeeTools)
                .WithSystemReference("System.IO.Compression.FileSystem")
                .WithSystemReference("System.Net.Http")
                .WithSystemReference("System.Web");
            SetupTestAssemblyFor(BeeStevedoreProgram);

            BeeStevedore = BeeCSharpProgramFor("Bee.Stevedore")
                .WithProjectReference(BeeNiceIO)
                .WithProjectReference(BeeTools)
                .WithProjectReference(BeeCore)
                .WithProjectReference(BeeDotNet)
                .WithProjectReference(BeeStevedoreProgram);

            BeeTundraBackendTests =
                SetupTestAssemblyFor(BeeTundraBackend, cp => cp.WithProjectReference(BeeStevedore));

            BeeWhy = BeeCSharpProgramFor("Bee.Why")
                .WithProjectReference(BeeNiceIO)
                .WithProjectReference(BeeCore)
                .WithProjectReference(BeeTundraBackend)
                .WithPrebuiltReference(NewtonSoftJsonDll);

            SetupTestAssemblyFor(BeeWhy, cp => cp
                .WithProjectReference(BeeTundraBackendTests)
                .WithProjectReference(BeeTools)
            );

            SetupTestAssemblyFor(BeeStevedore, cp => cp
                .WithSystemReference("System.IO.Compression.FileSystem")
                .WithProjectReference(BeeTundraBackendTests)
            );

            NativeProgramSupportCore = BeeCSharpProgramFor("Bee.NativeProgramSupport.Core")
                .WithProjectReference(BeeNiceIO)
                .WithProjectReference(BeeTools)
                .WithProjectReference(BeeCore);

            NativeProgramSupportRunning = BeeCSharpProgramFor("Bee.NativeProgramSupport.Running")
                .WithProjectReference(BeeNiceIO)
                .WithProjectReference(BeeTools)
                .WithProjectReference(BeeCore)
                .WithProjectReference(NativeProgramSupportCore)
                .WithPrebuiltReference(NewtonSoftJsonDll);

            BeeProgramRunnerCLI = BeeCSharpProgramFor("Bee.ProgramRunner.CLI", "exe")
                .WithProjectReference(BeeNiceIO)
                .WithProjectReference(BeeTools)
                .WithProjectReference(BeeCore)
                .WithProjectReference(BeeDotNet)
                .WithProjectReference(NativeProgramSupportCore)
                .WithProjectReference(NativeProgramSupportRunning)
                .WithPrebuiltReference(NewtonSoftJsonDll)
                .WithSource("External/Mono.Options/Options.cs");

            NativeProgramSupportBuilding = BeeCSharpProgramFor("Bee.NativeProgramSupport.Building")
                .WithProjectReference(BeeNiceIO)
                .WithProjectReference(BeeTools)
                .WithProjectReference(BeeCore)
                .WithProjectReference(BeeStevedore)
                .WithProjectReference(NativeProgramSupportCore)
                // Needed because of NativeProgramFormat.CanRun & NativeProgramFormat.SetupInvocation.
                .WithProjectReference(NativeProgramSupportRunning)
                .WithProjectReference(BeeProgramRunnerCLI);
            SetupTestAssemblyFor(NativeProgramSupportBuilding);

            ToolchainTestInfrastructure = BeeCSharpProgramFor("Bee.ToolchainTestInfrastructure")
                .WithProjectReference(BeeNiceIO)
                .WithProjectReference(BeeTools)
                .WithProjectReference(BeeCore)
                .WithProjectReference(beeCoreTests)
                .WithProjectReference(NativeProgramSupportCore)
                .WithProjectReference(NativeProgramSupportBuilding)
                .WithProjectReference(BeeTundraBackendTests)
                .WithPrebuiltReference(Moq)
                .WithPrebuiltReference(NUnitFramework);

            var referencesForToolChainAssemblies = new[]
            {
                BeeNiceIO, BeeTools, BeeCore, NativeProgramSupportCore, NativeProgramSupportRunning,
                NativeProgramSupportBuilding, BeeStevedore
            };

            ARMToolchain = BeeCSharpProgramFor("Bee.Toolchain.ARM")
                .WithProjectReference(referencesForToolChainAssemblies);

            GNUToolchain = BeeCSharpProgramFor("Bee.Toolchain.GNU")
                .WithProjectReference(referencesForToolChainAssemblies);

            LLVMToolchain = BeeCSharpProgramFor("Bee.Toolchain.LLVM")
                .WithProjectReference(GNUToolchain)
                .WithProjectReference(referencesForToolChainAssemblies);

            XcodeToolchain = BeeCSharpProgramFor("Bee.Toolchain.Xcode")
                .WithProjectReference(referencesForToolChainAssemblies)
                .WithPrebuiltReference(NewtonSoftJsonDll)
                .WithProjectReference(LLVMToolchain)
                .WithProjectReference(GNUToolchain);

            VisualStudioToolchain = BeeCSharpProgramFor("Bee.Toolchain.VisualStudio")
                .WithProjectReference(referencesForToolChainAssemblies)
                .WithResource($"{BeeRoot}/Bee.Toolchain.VisualStudio/Resources/WinRtMainStub.cpp")
                .WithResource($"{BeeRoot}/Bee.Toolchain.VisualStudio/Resources/WindowsStoreLogo.png");
            SetupTestAssemblyFor(VisualStudioToolchain);

            var windowsToolchain = BeeCSharpProgramFor("Bee.Toolchain.Windows")
                .WithProjectReference(VisualStudioToolchain)
                .WithProjectReference(BeeStevedoreProgram)
                .WithProjectReference(referencesForToolChainAssemblies);
            SetupTestAssemblyFor(windowsToolchain);

            var referencesForToolChainAssembliesAndBaseToolchains = referencesForToolChainAssemblies.Concat(BaseToolchains).ToArray();

            UWPToolchain = BeeCSharpProgramFor("Bee.Toolchain.UWP")
                .WithProjectReference(BeeStevedoreProgram)
                .WithProjectReference(referencesForToolChainAssembliesAndBaseToolchains)
                .WithResource($"{BeeRoot}/Bee.Toolchain.UWP/Resources/AppXManifest_uwp.xml");
            SetupTestAssemblyFor(UWPToolchain);

            LuminToolchain = BeeCSharpProgramFor("Bee.Toolchain.Lumin")
                .WithProjectReference(referencesForToolChainAssembliesAndBaseToolchains)
                .WithPrebuiltReference(NewtonSoftJsonDll);
            SetupTestAssemblyFor(LuminToolchain);

            foreach (var toolchainDir in BeeRoot.DirectoriesWithCsFiles().Where(d =>
                d.FileName.StartsWith("Bee.Toolchain") &&
                CSharpPrograms.All(c => c.Path.FileNameWithoutExtension != d.FileName)))
            {
                var toolchain = BeeCSharpProgramFor(toolchainDir.FileName)
                    .WithProjectReference(referencesForToolChainAssembliesAndBaseToolchains);
                SetupTestAssemblyFor(toolchain);
            }

            CSharpSupport = BeeCSharpProgramFor("Bee.CSharpSupport")
                .WithProjectReference(BeeNiceIO)
                .WithProjectReference(BeeTools)
                .WithProjectReference(BeeCore)
                .WithProjectReference(BeeDotNet)
                .WithProjectReference(BeeStevedore)
                .WithResource($"{BeeRoot}/Bee.CSharpSupport/template.csproj");
            SetupTestAssemblyFor(CSharpSupport);

            BeeProjectGeneration = BeeCSharpProgramFor("Bee.ProjectGeneration")
                .WithProjectReference(NativeProgramSupportCore)
                .WithProjectReference(NativeProgramSupportBuilding)
                .WithProjectReference(CSharpSupport)
                .WithProjectReference(BeeCore)
                .WithProjectReference(BeeNiceIO)
                .WithProjectReference(BeeTools);

            OnAllCoreProgramsHaveBeenSetup();

            SetupAdditionalToolchains(referencesForToolChainAssembliesAndBaseToolchains);

            var beeDotNetTests = SetupTestAssemblyFor(BeeDotNet);

            var branchInfoFile = SetupFileWithVersionControlnformation();

            BeeStandaloneDriver = BeeCSharpProgramFor("Bee.StandaloneDriver", "exe")
                .WithProjectReference(CSharpSupport)
                .WithProjectReference(BeeDotNet)
                .WithProjectReference(BeeCore)
                .WithProjectReference(BeeTundraBackend)
                .WithProjectReference(BeeNiceIO)
                .WithProjectReference(BeeTools)
                .WithProjectReference(BeeWhy)
                .WithPrebuiltReference(NewtonSoftJsonDll)
                .WithCopyReferencesNextToTarget(true)
                .WithResource($"{BeeRoot}/Bee.Examples/Build.bee.cs")
                .WithSource("External/Mono.Options/Options.cs");

            if (branchInfoFile != null)
            {
                BeeStandaloneDriver = BeeStandaloneDriver.WithResource(branchInfoFile);
                Backend.Current.AddDependency(BeeStandaloneDriver.ProjectFile.Path, branchInfoFile);
            }

            BeeStandaloneDriverTests = SetupTestAssemblyFor(
                BeeStandaloneDriver,
                cp => cp.WithResource(new EmbeddedResource
                {
                    Identifier = "BeeBackend_build",
                    PathToEmbed = $"{BeeRoot}/Bee.StandaloneDriver.Tests/BeeBackend_beefile.cs"
                })
                .WithSource("External/Mono.Options/Tests")
                .WithPrebuiltReference(Moq),
                beeDotNetTests);

            var beeToolChains = CSharpPrograms.Where(IsBeeToolchain).ToList();
            NativeProgramAndToolchainTests = SetupTestAssemblyFor(
                $"{BeeRoot}/Bee.NativeProgramAndToolchainTests",
                null, new List<CSharpProgram>()
                {
                    beeToolChains,
                    BeeProgramRunnerCLI,
                    BeeStandaloneDriver,
                    BeeStandaloneDriverTests,
                    BeeNiceIO,
                    BeeTools,
                    NativeProgramSupportCore,
                    NativeProgramSupportRunning,
                    NativeProgramSupportBuilding,
                    BeeCore,
                    BeeTundraBackendTests,
                    ToolchainTestInfrastructure
                }.ToArray());
            NativeProgramAndToolchainTests.PrebuiltReferences.Add(Moq);

            BeeExamples = BeeCSharpProgramFor("Bee.Examples")
                .WithProjectReference(beeToolChains.ToArray())
                .WithProjectReference(NativeProgramSupportBuilding)
                .WithProjectReference(CSharpSupport)
                .WithProjectReference(CSharpProgram
                    .AllTransitiveReferencesFor(CSharpConfig, NativeProgramSupportBuilding, CSharpSupport)
                    .ToArray());


            SetupTestAssemblyFor(BeeProgramRunnerCLI,
                beeToolChains.Concat(CSharpProgram.AllTransitiveReferencesFor(CSharpConfig, beeToolChains.ToArray()))
                    .Append(ToolchainTestInfrastructure)
                    .Append(BeeTundraBackend)
                    .ToArray());

            SetupTestAssemblyFor(NativeProgramSupportRunning, new List<CSharpProgram>()
            {
                beeToolChains.Concat(beeToolChains.SelectMany(x => x.References.For(CSharpConfig))).ToArray(),
                BeeTundraBackend, // NativeProgramSupportRunning tests needs to build stuff to run it.
                NativeProgramSupportBuilding,
                ToolchainTestInfrastructure,
                BeeProgramRunnerCLI
            }.ToArray());

            var csharpSupportIntegrationTests = SetupTestAssemblyFor($"{BeeRoot}/Bee.CSharpSupport.IntegrationTests",
                cp => cp.PrebuiltReferences.Add(CecilPath), new List<CSharpProgram>
                {
                    ProgramsContainsBeeApiTestBackend(),
                    CSharpSupport,
                    BeeNiceIO,
                    BeeDotNet,
                    BeeTools,
                    BeeCore,
                    AssembliesCSharpSupportIntegrationTestsShouldReference()
                }.ToArray());

            csharpSupportIntegrationTests.PrebuiltReferences.Add(CecilPath);

            SetupUnityAssemblies();

            var beeExe = new NPath("build/BeeDistribution/bee.exe");
            SetupBeeDistribution(beeExe, new[]
            {
                BeeCore,
                CSharpSupport,
                NativeProgramSupportCore,
                NativeProgramSupportRunning,
                NativeProgramSupportBuilding,
                BeeTundraBackend,
                BeeTools,
                BeeProjectGeneration,
                BeeTundraBinaries,
                BeeWhy
            }
                    .Concat(CSharpPrograms.Where(p => IsBeeToolchain(p) && ShouldIncludeToolchainInBeeDistribution(p)))
                    .ToArray());
            Backend.Current.AddAliasDependency("bee", beeExe);

            var beeBootstrapExe = new NPath("build/BeeDistribution/bee_bootstrap.exe");
            SetupBeeDistribution(beeBootstrapExe,
                ProgramsToIncludeInBeeBootstrap().ToArray()
            );
            Backend.Current.AddAliasDependency("bee", beeBootstrapExe);

            foreach (var csharpProgram in CSharpPrograms)
                csharpProgram.SetupSpecificConfiguration(CSharpConfig);
        }

        protected virtual IEnumerable<CSharpProgram> ProgramsToIncludeInBeeBootstrap()
        {
            yield return CSharpSupport;
        }

        protected abstract NPath CecilPath { get; }

        protected abstract NPath NiceIOPath { get; }

        protected abstract string TundraPathFor(string platform);

        protected virtual NPath SetupFileWithVersionControlnformation() => null;

        protected abstract NPath BeeRoot { get; }

        protected CSharpProgram[] BaseToolchains => new[] {GNUToolchain, XcodeToolchain, LLVMToolchain, VisualStudioToolchain, ARMToolchain};

        protected virtual void OnAllCoreProgramsHaveBeenSetup()
        {
        }

        protected virtual IEnumerable<CSharpProgram> AssembliesCSharpSupportIntegrationTestsShouldReference()
        {
            yield break;
        }

        protected virtual void SetupAdditionalToolchains(CSharpProgram[] referencesForToolChainAssemblies)
        {
        }

        protected virtual void SetupUnityAssemblies()
        {
        }

        protected virtual CSharpProgram[] ProgramsContainsBeeApiTestBackend()
        {
            return new[] {BeeTundraBackend, BeeStandaloneDriverTests};
        }

        protected static bool IsBeeToolchain(CSharpProgram arg)
        {
            return arg.Path.FileName.StartsWith("Bee.Toolchain.") && !arg.Path.FileName.Contains("Tests");
        }

        void SetupBeeDistribution(NPath targetFile, CSharpProgram[] programsToILRepack)
        {
            var allCSharpPrograms = CSharpProgram.AllTransitiveReferencesFor(CSharpConfig, programsToILRepack).Concat(programsToILRepack);
            var withAllDependencies = allCSharpPrograms
                .Select(p => p.Path)
                .Concat(allCSharpPrograms.SelectMany(p => p.PrebuiltReferences.For(CSharpConfig)))
                .Distinct()
                .ToArray();

            var dependencyPaths = withAllDependencies.Prepend(BeeStandaloneDriver.Path);

            var ilrepack2 = new DotNetRunnableProgram(new DotNetAssembly(ILRepackExePath, Framework.Framework46));

            Backend.Current.AddAction(
                actionName: "ILMerge",
                targetFiles: new[] {targetFile},
                inputs: dependencyPaths.Append(ILRepackExePath).ToArray(),
                executableStringFor: ilrepack2.InvocationString,
                commandLineArguments: new string[] {}
                    .Concat(dependencyPaths.InQuotes())
                    .Append("/union")
                    .Append($"/out:{targetFile.InQuotes()}")
                    .ToArray()
            );
        }

        protected abstract NPath ILRepackExePath { get; }

        protected virtual bool ShouldIncludeToolchainInBeeDistribution(CSharpProgram toolchain) => true;

        protected CSharpProgram SetupTestAssemblyFor(CSharpProgram assemblyUnderTest,
            params CSharpProgram[] additionalReferences)
        {
            return SetupTestAssemblyFor(assemblyUnderTest, cp => {}, additionalReferences);
        }

        private CSharpProgram SetupTestAssemblyFor(
            CSharpProgram assemblyUnderTest,
            Action<CSharpProgram> configurationCallback,
            params CSharpProgram[] additionalReferences)
        {
            var originalAssemblyDir = assemblyUnderTest.Sources.ForAny().First(d => d.DirectoryExists());
            var references = assemblyUnderTest.References.For(CSharpConfig)
                .Append(assemblyUnderTest)
                .Append(BeeTestHelpers)
                .Concat(additionalReferences)
                .ToArray();
            var testAssemblySourcesDir = originalAssemblyDir.Parent.Combine(originalAssemblyDir.FileName + ".Tests");

            if (IsBeeToolchain(assemblyUnderTest))
            {
                references = references
                    .Append(ToolchainTestInfrastructure)
                    .Append(BeeTundraBackendTests)
                    .ToArray();
            }

            var csharpProgram = SetupTestAssemblyFor(testAssemblySourcesDir, configurationCallback, references,
                assemblyUnderTest.PrebuiltReferences.For(CSharpConfig));

            if (_testAssemblies.ContainsKey(assemblyUnderTest))
                throw new ArgumentException($"Trying to setup test for {assemblyUnderTest.Path} twice");
            _testAssemblies.Add(assemblyUnderTest, csharpProgram);

            return csharpProgram;
        }

        protected virtual CSharpProgram SetupTestAssemblyFor(NPath testAssemblySourcesDir,
            Action<CSharpProgram> configurationCallback, CSharpProgram[] references,
            IEnumerable<NPath> prebuiltReferences = null)
        {
            var testAssemblies = references.Select(r =>
            {
                _testAssemblies.TryGetValue(r, out var correspondingTestAssembly);
                return correspondingTestAssembly;
            }).Where(t => t != null).ToArray();

            var csharpProgram = new CSharpProgram()
            {
                Path = BuildSystemDir.Combine(testAssemblySourcesDir.FileName)
                    .Combine(testAssemblySourcesDir.FileName + ".dll"),
                Framework = Framework.Framework46,
                LanguageVersion = "7",
                PrebuiltReferences = {NUnitFramework, Moq, prebuiltReferences ?? Enumerable.Empty<NPath>()},
                Sources = {testAssemblySourcesDir},
                CopyReferencesNextToTarget = true,
                References =
                {
                    references,
                    testAssemblies,
                },
            };
            if (configurationCallback != null) configurationCallback?.Invoke(csharpProgram);

            csharpProgram.SetupSpecificConfiguration(CSharpConfig);
            Backend.Current.AddAliasDependency("BuildSystemTests", csharpProgram.Path);
            CSharpPrograms.Add(csharpProgram);
            return csharpProgram;
        }

        protected CSharpProgram BeeCSharpProgramFor(NPath sourceDirectory, string extension = "dll")
        {
            NPath dir = BeeRoot.Combine(sourceDirectory);
            return CSharpProgramFor(dir, extension);
        }

        protected CSharpProgram CSharpProgramFor(NPath sourceDirectory, string extension)
        {
            var csharpProgram = new CSharpProgram()
            {
                Path = BuildSystemDir.Combine($"{sourceDirectory.FileName}/{sourceDirectory.FileName}.dll").ChangeExtension(extension),
                Sources = {sourceDirectory},
                CopyReferencesNextToTarget = false,
                Framework = Framework.Framework46,
                LanguageVersion = "7"
            };
            CSharpPrograms.Add(csharpProgram);
            return csharpProgram;
        }
    }

    //implement a Builder like api through extension methods so we don't have to change this entire file right now.
    public static class TempCSharpProgramExtensions
    {
        public static CSharpProgram WithSource(this CSharpProgram _this, params NPath[] source)
        {
            _this.Sources.Add(source);
            return _this;
        }

        public static CSharpProgram WithProjectReference(this CSharpProgram _this, params CSharpProgram[] reference)
        {
            for (int i = 0; i != reference.Length; i++)
                if (reference[i] == null)
                    throw new Exception($"index {i} is null");
            _this.References.Add(reference);
            return _this;
        }

        public static CSharpProgram WithResource(this CSharpProgram _this, EmbeddedResource resource)
        {
            _this.Resources.Add(resource);
            return _this;
        }

        public static CSharpProgram WithResource(this CSharpProgram _this, NPath resource)
        {
            _this.Resources.Add(new EmbeddedResource() {PathToEmbed = resource, Identifier = resource.FileName});
            return _this;
        }

        public static CSharpProgram WithResource(this CSharpProgram _this, IEnumerable<NPath> resources)
        {
            foreach (var resource in resources)
                _this.WithResource(resource);
            return _this;
        }

        public static CSharpProgram WithPrebuiltReference(this CSharpProgram _this, NPath reference)
        {
            _this.PrebuiltReferences.Add(reference);
            return _this;
        }

        public static CSharpProgram WithSystemReference(this CSharpProgram _this, params string[] systemReference)
        {
            _this.SystemReferences.Add(systemReference);
            return _this;
        }

        public static CSharpProgram WithCopyReferencesNextToTarget(this CSharpProgram _this, bool value)
        {
            _this.CopyReferencesNextToTarget = value;
            return _this;
        }

        public static CSharpProgram WithDefines(this CSharpProgram _this, string[] value)
        {
            _this.Defines.Add(value);
            return _this;
        }
    }

    public static class NiceIOExtensions
    {
        public static NPath[] DirectoriesWithCsFiles(this NPath path)
        {
            return path.Directories().Where(d => d.Files("*.cs").Any()).ToArray();
        }
    }
}
