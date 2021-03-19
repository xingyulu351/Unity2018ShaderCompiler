using System.Collections.Generic;
using System.Linq;
using Bee.Toolchain.GNU;
using NiceIO;
using Unity.BuildSystem.NativeProgramSupport;

namespace Bee.Toolchain.Xcode
{
    public static class XcodeDynamicLinkerExtensions
    {
        // NativeProgramFormat settings
        public static T WithInstallName<T>(this T _this, string installName) where T : XcodeDynamicLinker
            => _this.With<T>(l => l.InstallName = installName);

        public static T WithExportedSymbolList<T>(this T _this, string installName) where T : XcodeDynamicLinker
            => _this.With<T>(l => l.ExportedSymbolsList = installName);

        public static T WithAllLoad<T>(this T _this, bool allLoad) where T : XcodeDynamicLinker
            => _this.With<T>(l => l.AllLoad = allLoad);
    }

    public class XcodeDynamicLinker : LdLikeDynamicLinker
    {
        public override bool DynamicLibrary { get; set; }

        public string InstallName { private get; set; }
        public bool AllLoad { private get; set; }
        public NPath ExportedSymbolsList { get; set; }

        public XcodeDynamicLinker(XcodeToolchain toolchain) : base(toolchain, true) {}

        protected override IEnumerable<NPath> InputFilesFor(NPath destination, CodeGen codegen, IEnumerable<NPath> objectFiles,
            IEnumerable<PrecompiledLibrary> nonToolchainSpecificLibraries, IEnumerable<PrecompiledLibrary> allLibraries)
        {
            foreach (var file in base.InputFilesFor(destination, codegen, objectFiles, nonToolchainSpecificLibraries, allLibraries))
                yield return file;

            if (ExportedSymbolsList != null)
                yield return ExportedSymbolsList;
        }

        protected override IEnumerable<string> CommandLineFlagsFor(NPath target, CodeGen codegen, IEnumerable<NPath> inputFiles,
            IEnumerable<PrecompiledLibrary> nonToolchainSpecificLibraries, IEnumerable<PrecompiledLibrary> allLibraries)
        {
            foreach (var flag in base.CommandLineFlagsFor(target, codegen, inputFiles, nonToolchainSpecificLibraries, allLibraries))
                yield return flag;

            foreach (var framework in allLibraries.OfType<SystemFramework>())
                yield return $"-framework {framework.Name}";

            foreach (var framework in allLibraries.OfType<NonSystemFramework>())
            {
                yield return $"-F {framework.Path.Parent}";
                yield return $"-framework {framework.Path.FileNameWithoutExtension}";
                yield return "-Wl,-rpath,@executable_path/../Frameworks";
            }

            yield return "-dead_strip";

            if (DynamicLibrary)
                yield return "-dynamiclib";

            if (AllLoad)
                yield return "-all_load";

            // Install name is only for dylibs
            if (DynamicLibrary && (InstallName != null))
                yield return $"-install_name {InstallName}";

            if (ExportedSymbolsList != null)
                yield return $"-exported_symbols_list {ExportedSymbolsList.InQuotes()}";
        }
    }
}
