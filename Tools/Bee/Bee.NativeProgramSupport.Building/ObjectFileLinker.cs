using System;
using System.Collections.Generic;
using System.Linq;
using Bee.Core;
using Bee.NativeProgramSupport.Building;
using NiceIO;
using Unity.BuildTools;

namespace Unity.BuildSystem.NativeProgramSupport
{
    public abstract class StaticLinker : ObjectFileLinker
    {
        protected override string LinkerCommand => Toolchain.Sdk.StaticLinkerCommand;
        protected override NPath LinkerPath => Toolchain.Sdk.StaticLinkerPath;
        protected override string ActionName => $"Lib_{Toolchain.ActionName}";

        protected StaticLinker(ToolChain toolChain) : base(toolChain) {}
    }

    public abstract class DynamicLinker : ObjectFileLinker
    {
        protected override string LinkerCommand => Toolchain.Sdk.DynamicLinkerCommand;
        protected override NPath LinkerPath => Toolchain.Sdk.DynamicLinkerPath;
        protected override string ActionName => $"Link_{Toolchain.ActionName}";

        public abstract bool DynamicLibrary { get; set; }

        protected DynamicLinker(ToolChain toolChain) : base(toolChain) {}
    }

    public static class DynamicLinkerExtensions
    {
        public static T AsDynamicLibrary<T>(this T _this) where T : DynamicLinker
            => _this.With<T>(l => l.DynamicLibrary = true);
    }

    public abstract class ObjectFileLinker
    {
        protected abstract string ActionName { get; }

        protected abstract NPath LinkerPath { get; }
        protected virtual string LinkerCommand => LinkerPath?.ToString(SlashMode.Native) ?? string.Empty;

        protected virtual bool SupportsResponseFile => true;
        public virtual bool AllowUnwrittenOutputFiles => false;

        public ToolChain Toolchain { get; }

        protected ObjectFileLinker(ToolChain toolchain)
        {
            Toolchain = toolchain;
        }

        public virtual NPath[] CombineObjectFiles(NPath destination,
            CodeGen codegen,
            IEnumerable<NPath> objectFiles,
            IEnumerable<PrecompiledLibrary> nonToolchainSpecificLibraries,
            IEnumerable<PrecompiledLibrary> allLibraries)
        {
            var flags = CommandLineFlagsFor(destination, codegen, objectFiles, nonToolchainSpecificLibraries, allLibraries).ToArray();
            var inputFiles = InputFilesFor(destination, codegen, objectFiles, nonToolchainSpecificLibraries, allLibraries).ToArray();
            var allTargets = OutputFilesFor(destination, codegen, objectFiles, nonToolchainSpecificLibraries, allLibraries).ToArray();

            Backend.Current.AddAction(ActionName,
                allTargets,
                inputFiles,
                LinkerCommand,
                flags.ToArray(),
                SupportsResponseFile,
                environmentVariables: EnvironmentVariables,
                allowUnwrittenOutputFiles: AllowUnwrittenOutputFiles);
            return allTargets;
        }

        protected virtual Dictionary<string, string> EnvironmentVariables { get; } = new Dictionary<string, string>();

        protected virtual ObjectFileLinker LinkerSpecificSettingsFor(
            NativeProgram nativeProgram,
            NativeProgramConfiguration config,
            CLikeCompilerSettings settings,
            NPath file,
            NPath artifacts)
        {
            return this;
        }

        public ObjectFileLinker LinkerFor(
            NativeProgram nativeProgram,
            NativeProgramConfiguration config,
            CLikeCompilerSettings settings,
            NPath file,
            NPath artifacts)
        {
            var linker = LinkerSpecificSettingsFor(nativeProgram, config, settings, file, artifacts);
            return nativeProgram.DynamicLinkerSettings().For(config, linker, file);
        }

        public T With<T>(Action<T> callback) where T : ObjectFileLinker
        {
            var copy = MemberwiseClone() as T;
            callback(copy);
            return copy;
        }

        protected abstract IEnumerable<string> CommandLineFlagsFor(NPath destination,
            CodeGen codegen,
            IEnumerable<NPath> objectFiles,
            IEnumerable<PrecompiledLibrary> nonToolchainSpecificLibraries,
            IEnumerable<PrecompiledLibrary> allLibraries);

        protected virtual IEnumerable<NPath> InputFilesFor(NPath destination,
            CodeGen codegen,
            IEnumerable<NPath> objectFiles,
            IEnumerable<PrecompiledLibrary> nonToolchainSpecificLibraries,
            IEnumerable<PrecompiledLibrary> allLibraries)
        {
            var inputFiles = objectFiles.Concat(allLibraries.Where(l => !l.System).Select(l => l.Path));
            // Add linker as an input (automatically adding Stevedore to
            // depgraph for downloadable SDKs), except for Jam (since it
            // doesn't distinguish between inputs and dependencies, causing
            // build errors when we do this).
            if (!(Backend.Current is IJamBackend) && LinkerPath != null)
                inputFiles.Append(LinkerPath);
            return inputFiles;
        }

        protected virtual IEnumerable<NPath> OutputFilesFor(NPath destination,
            CodeGen codegen,
            IEnumerable<NPath> objectFiles,
            IEnumerable<PrecompiledLibrary> nonToolchainSpecificLibraries,
            IEnumerable<PrecompiledLibrary> allLibraries)
        {
            yield return destination;
        }

        public static ICustomizationFor<TSettings> TypedLinkerCustomizationsFor<TSettings>(NativeProgram nativeProgram, Func<NativeProgramConfiguration, bool> condition = null) where TSettings : ObjectFileLinker
        {
            return nativeProgram._linkerCustomizations.TypedViewFor<TSettings>(condition);
        }
    }
}
