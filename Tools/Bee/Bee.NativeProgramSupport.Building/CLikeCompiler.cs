using System;
using System.Collections.Generic;
using System.Linq;
using Bee.Core;
using Bee.NativeProgramSupport.Building;
using NiceIO;

namespace Unity.BuildSystem.NativeProgramSupport
{
    public abstract class CLikeCompiler : ObjectFileProducer
    {
        protected virtual string ActionName => $"C_{Toolchain.ActionName}";

        public override string[] SupportedExtensions { get; } = {"cpp", "c", "cc"};

        public Architecture Architecture => Toolchain.Architecture;
        public ToolChain Toolchain { get; }
        public CLikeCompilerSettings DefaultSettings { get; protected set; }
        protected virtual bool SupportsResponseFile => true;
        public abstract string WarningStringForUnusedVariable { get; }
        public virtual string CppCompilerCommand => Toolchain.Sdk.CppCompilerCommand;
        public virtual bool SupportsPCH => true;
        public virtual string PCHExtension { get; } = "pch";

        public static bool VerbosePerfOutput { get; set; } = false;

        protected CLikeCompiler(ToolChain toolchain)
        {
            Toolchain = toolchain;
        }

        public override ObjectFileProductionInstructions ObjectProduceInstructionsFor(NativeProgram nativeProgram,
            NativeProgramConfiguration config, NPath inputFile, NPath[] allIncludes, string[] allDefines,
            NPath artifactPath)
        {
            var language = FileLanguage.FromPath(inputFile);

            var programDefines = allDefines;
            var fileDefines = nativeProgram.PerFileDefines.For(inputFile, config);

            // NDEBUG is fairly standard in C (e.g. controls built-in asserts);
            // _DEBUG is mostly a MSVC-ism, however quite a lot of our codebase uses it
            // on non-Windows platforms too. So just setup these macros always.
            var globalDefines = config.CodeGen == CodeGen.Debug ? new[] {"_DEBUG"} : new[] {"NDEBUG"};
            var defines = programDefines.Concat(fileDefines).Concat(globalDefines).Distinct().ToArray();

            var includeDirs = allIncludes
                .Concat(nativeProgram.PerFileIncludeDirectories.For(inputFile, config))
                .Distinct()
                .ToArray();

            var settings = DefaultSettings
                .WithLanguage(language)
                .WithDefines(defines)
                .WithIncludeDirectories(includeDirs)
                .CompilerSettingsFor(nativeProgram, config, inputFile, artifactPath);

            var usePch = SupportsPCH && language != Language.AssemblerWithCpp;
            if (usePch)
            {
                var inputPch = nativeProgram.PerFilePchs.Has(inputFile, config)
                    ? nativeProgram.PerFilePchs.For(inputFile, config)
                    : nativeProgram.DefaultPCH;
                settings = settings.WithInputPCH(inputPch);
            }

            return new CLikeCompilationUnitInstruction(
                settings,
                inputFile,
                canLump: config.Lump);
        }

        public override ObjectFileProductionInstructions[] PostProcessObjectProductionInstructions(
            ObjectFileProductionInstructions[] inputInstructions,
            NPath artifactsPath,
            Action<NPath> reportAdditionalObjectFile)
        {
            return inputInstructions
                .OfType<CLikeCompilationUnitInstruction>()
                .GroupBy(i => i.CompilerSettings.HashSettingsRelevantForCompilation())
                .SelectMany(
                    @group => @group.First().CompilerSettings.InputPCH == null
                    ? @group
                    : SetupPCHForGroupOfInstructions(
                        @group,
                        artifactsPath,
                        reportAdditionalObjectFile,
                        @group.First().CompilerSettings))
                .Concat(inputInstructions.Where(i => !(i is CLikeCompilationUnitInstruction)))
                .ToArray();
        }

        private IEnumerable<CLikeCompilationUnitInstruction> SetupPCHForGroupOfInstructions(
            IEnumerable<CLikeCompilationUnitInstruction> inputInstructions,
            NPath artifactsPath,
            Action<NPath> reportAdditionalObjectFile,
            CLikeCompilerSettings compilerSettings)
        {
            NPath outputPch = $"{artifactsPath}/{compilerSettings.InputPCH.FileNameWithoutExtension}-{compilerSettings.HashSettingsRelevantForCompilation()}.{compilerSettings.Compiler.PCHExtension}";

            var settingsWithoutPch = compilerSettings.WithInputPCH(null).WithOutputPCH(null);
            var optionalAdditionalObjectFile =
                SetupOutputPCH(outputPch, compilerSettings.InputPCH, settingsWithoutPch, artifactsPath);

            if (optionalAdditionalObjectFile != null)
                reportAdditionalObjectFile(optionalAdditionalObjectFile);

            return inputInstructions.Select(i => i.WithCompilerSettings(i.CompilerSettings.WithOutputPCH(outputPch)));
        }

        protected virtual NPath SetupOutputPCH(
            NPath outputPch,
            NPath inputPch,
            CLikeCompilerSettings settings,
            NPath artifactsPath)
        {
            Backend.Current.AddAction(
                $"{ActionName}Pch",
                new[] {outputPch},
                new[] {inputPch},
                CppCompilerCommand,
                settings.CommandLineInfluencingCompilationFor(
                    null,
                    null,
                    createOutputPchAt: outputPch,
                    headerToCreateOutputPchFrom: inputPch)
                    .ToArray());

            Backend.Current.ScanCppFileForHeaders(outputPch, inputPch, settings.IncludeDirectories);
            return null;
        }

        public void SetupInvocation(NPath objectFile, CLikeCompilationUnitInstruction cLikeCompilationUnitInstruction)
        {
            var commandLine = cLikeCompilationUnitInstruction.CompilerSettings.CommandLineInfluencingCompilationFor(
                objectFile,
                cLikeCompilationUnitInstruction.InputFile);

            var outputPch = cLikeCompilationUnitInstruction.CompilerSettings.OutputPCH;

            var outputToSwallow = cLikeCompilationUnitInstruction.CompilerSettings.OutputToSwallowFor(cLikeCompilationUnitInstruction);

            var inputFiles = new List<NPath>
            {
                cLikeCompilationUnitInstruction.InputFile
            };
            if (outputPch != null)
                inputFiles.Add(outputPch);
            if (cLikeCompilationUnitInstruction.CompilerSettings.InputPCH != null)
                inputFiles.Add(cLikeCompilationUnitInstruction.CompilerSettings.InputPCH);
            inputFiles.AddRange(cLikeCompilationUnitInstruction.CompilerSettings.AdditionalInputs);

            // Add compiler as an input (automatically adding Stevedore to
            // depgraph for downloadable SDKs), except for Jam (since it
            // doesn't distinguish between inputs and dependencies, causing
            // build errors when we do this).
            if (!(Backend.Current is IJamBackend))
                inputFiles.AddRange(Toolchain.Sdk.TundraInputs.Where(p => p != null));

            Backend.Current.AddAction(
                ActionName,
                new[] {objectFile},
                inputFiles.ToArray(),
                CppCompilerCommand,
                commandLine.ToArray(),
                SupportsResponseFile,
                allowedOutputSubstrings: outputToSwallow,
                environmentVariables: EnvironmentVariables);

            if (!cLikeCompilationUnitInstruction.HasIncludeDependenciesSetupManually)
            {
                Backend.Current.ScanCppFileForHeaders(
                    objectFile,
                    cLikeCompilationUnitInstruction.InputFile,
                    cLikeCompilationUnitInstruction.CompilerSettings.IncludeDirectories);
            }
        }

        protected virtual Dictionary<string, string> EnvironmentVariables => new Dictionary<string, string>();

        public static ICustomizationFor<TSettings> TypedCompilerCustomizationsFor<TSettings>(NativeProgram nativeProgram, Func<NativeProgramConfiguration, bool> condition = null) where TSettings : CLikeCompilerSettings
        {
            return nativeProgram._compilerCustomizations.TypedViewFor<TSettings>(condition);
        }
    }
}
