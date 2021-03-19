using System;
using System.Collections.Generic;
using System.Linq;
using Bee.Core;
using Bee.NativeProgramSupport.Core;
using Bee.NativeProgramSupport.Running;
using Unity.BuildSystem.NativeProgramSupport;
using Bee.ProgramRunner.CLI;
using JamSharp.Runtime;
using NiceIO;

namespace Unity.BuildSystem.NativeProgramSupport
{
    public abstract class NativeProgramFormat<TLinker> : NativeProgramFormat,
        INativeProgramFormat<TLinker> where TLinker : ObjectFileLinker
    {
        protected NativeProgramFormat(TLinker linker) : base(linker) {}

        public T WithLinkerSetting<T>(Action<TLinker> callback) where T : class
            => WithLinkerSetting<NativeProgramFormat, TLinker>(callback) as T;
    }

    public abstract class NativeProgramFormat
    {
        public bool CanRun => AvailableRunnerEnvironments.Any();
        protected virtual string RunnerArgs => null;

        public abstract string Extension { get; }
        public virtual string LinkTargetExtension => Extension;
        public ToolChain Toolchain => Linker.Toolchain;

        internal ObjectFileLinker Linker { get; set; }

        private IEnumerable<RunnerEnvironment> AvailableRunnerEnvironments => ProgramRunnerDaemon
        .FindRunnerEnvironments(Toolchain.Platform, new[] {Toolchain.Architecture})
        .Where(x => x.Status == RunnerEnvironment.EnvironmentStatus.FullAccess);

        protected NativeProgramFormat(ObjectFileLinker linker) => Linker = linker;

        public virtual NPath[] SetupAdditionalSourceFilesForTarget(NPath destination, CodeGen codegen) =>
            Array.Empty<NPath>();

        public virtual NPath[] CombineObjectFilesAndSetupProgram(
            NativeProgram nativeProgram,
            NativeProgramConfiguration config,
            IEnumerable<NPath> objectFiles,
            IEnumerable<PrecompiledLibrary> allLibraries,
            NPath destination,
            NPath artifactsPath)
        {
            var nonToolchainSpecificLibraries = allLibraries.Where(l => !l.ToolchainSpecific);
            var linkTarget = destination.ChangeExtension(LinkTargetExtension);
            var linkResult = CombineObjectFiles(nativeProgram,
                config,
                objectFiles,
                nonToolchainSpecificLibraries,
                allLibraries,
                linkTarget,
                artifactsPath);
            return SetupProgram(destination, linkResult, config.CodeGen, nonToolchainSpecificLibraries, allLibraries).ToArray();
        }

        public static bool VerbosePerfOutput { get; set; } = false;

        protected virtual IEnumerable<NPath> SetupProgram(NPath destination,
            IEnumerable<NPath> linkResult,
            CodeGen codegen,
            IEnumerable<PrecompiledLibrary> nonToolchainSpecificLibraries,
            IEnumerable<PrecompiledLibrary> allLibraries)
        {
            return linkResult;
        }

        protected virtual IEnumerable<NPath> CombineObjectFiles(
            NativeProgram nativeProgram,
            NativeProgramConfiguration config,
            IEnumerable<NPath> objectFiles,
            IEnumerable<PrecompiledLibrary> nonToolchainSpecificLibraries,
            IEnumerable<PrecompiledLibrary> allLibraries,
            NPath linkTarget,
            NPath artifactsPath)
        {
            return SetupLinker(nativeProgram, config, linkTarget, artifactsPath)
                .CombineObjectFiles(linkTarget,
                config.CodeGen,
                objectFiles,
                nonToolchainSpecificLibraries,
                allLibraries);
        }

        protected internal ObjectFileLinker SetupLinker(
            NativeProgram nativeProgram,
            NativeProgramConfiguration config,
            NPath linkTarget,
            NPath artifactsPath)
        {
            var compilerSettings = Toolchain.CppCompiler.DefaultSettings.CompilerSettingsFor(nativeProgram, config, linkTarget, artifactsPath);
            compilerSettings = nativeProgram.CompilerSettings().For(config, compilerSettings, linkTarget);
            return Linker.LinkerFor(nativeProgram, config, compilerSettings, linkTarget, artifactsPath);
        }

        public InvocationResult SetupInvocationOfProducedExecutable(
            NPath program,
            string[] args = null,
            bool enableConsoleOutput = true,
            bool ignoreProgramExitCode = false,
            NPath targetDirectory = null)
        {
            var runnerEnvironment = AvailableRunnerEnvironments.FirstOrDefault();
            if (runnerEnvironment == null)
                throw new Exception($"No active RunnerEnvironment available for toolchain {Toolchain.ActionName}.");

            var result = InvocationResult.For(program, targetDirectory);
            var targetManagerArgs = new List<string>(16)
            {
                "--launch",
                $"--environment={runnerEnvironment.Identifier}",
                $"--app-path={program.InQuotes()}",
                $"--app-exit={result.Result.InQuotes()}",
                $"--app-err={result.Stderr.InQuotes()}",
                $"--app-std={result.Stdout.InQuotes()}"
            };
            if (ignoreProgramExitCode)
                targetManagerArgs.Add("--app-ignore-failure");
            if (enableConsoleOutput)
                targetManagerArgs.Add("--app-forward-console-output");
            if (RunnerArgs != null)
                targetManagerArgs.Add($"--runner-args=\"{RunnerArgs}\"");
            if (args?.Length > 0)
            {
                targetManagerArgs.Add("--");
                targetManagerArgs.AddRange(args);
            }

            Backend.Current.AddAction(
                actionName: $"Run_{Toolchain.ActionName}",
                targetFiles: new[] {result.Result, result.Stdout, result.Stderr},
                inputs: new[] {program, runnerEnvironment.RunnerEnvironmentFilePath},
                executableStringFor: ProgramRunnerCLI.InvocationString,
                commandLineArguments: targetManagerArgs.ToArray()
            );
            return result;
        }

        public T WithLinkerSetting<T, TLinker>(Action<TLinker> callback)
            where T : NativeProgramFormat
            where TLinker : ObjectFileLinker
        {
            return With<T>(l => l.Linker = l.Linker.With(callback));
        }

        private T With<T>(Action<T> callback) where T : NativeProgramFormat
        {
            var copy = MemberwiseClone() as T;
            callback(copy);
            return copy;
        }
    }

    // A bit messy this but variant types only works for interfaces and this makes extension functions
    // easier to implement
    public interface INativeProgramFormat<out TLinker> where TLinker : ObjectFileLinker
    {
        T WithLinkerSetting<T>(Action<TLinker> callback) where T : class;
    }
}
