using System.Linq;
using Bee.Core;
using Bee.Toolchain.VisualStudio;
using NiceIO;
using Unity.BuildSystem.NativeProgramSupport;
using Unity.BuildSystem.VisualStudio;
using Unity.BuildTools;

namespace Bee.Toolchain.UWP
{
    public sealed class UWPToolchain : VisualStudioToolchain
    {
        public override string LegacyPlatformIdentifier
        {
            get
            {
                if (Architecture is x64Architecture)
                    return "uap_x64";
                else
                    return $"uap_{Architecture.Name}";
            }
        }

        public override NativeProgramFormat ExecutableFormat { get; }
        public override NativeProgramFormat DynamicLibraryFormat { get; }

        public override CLikeCompiler CppCompiler => new UWPCompiler(this);

        public UWPToolchain(UWPSdk sdk) : base(sdk)
        {
            ExecutableFormat = new UWPExecutableFormat(this);
            DynamicLibraryFormat = new UWPDynamicLibraryFormat(this);
        }
    }

    public sealed class UWPExecutableFormat : NativeProgramFormat<UWPDynamicLinker>
    {
        public override string Extension { get; } = "exe";

        public UWPExecutableFormat(UWPToolchain toolchain) : base(
            new UWPDynamicLinker(toolchain).WithSubSystemType(SubSystemType.Windows))
        {
        }

        public override NPath[] SetupAdditionalSourceFilesForTarget(NPath destination, CodeGen codegen)
        {
            var destinationDirectory = destination.Parent.Combine("generated");
            var uwpMainEmulationSourceFile = destinationDirectory.Combine(destination.ChangeExtension("main.cpp").FileName);
            Backend.Current.AddWriteTextAction(uwpMainEmulationSourceFile, ResourceHelper.ReadTextFromResource(typeof(VisualStudioToolchain).Assembly, "WinRtMainStub.cpp"));
            return base.SetupAdditionalSourceFilesForTarget(destination, codegen).Append(uwpMainEmulationSourceFile).ToArray();
        }
    }

    public sealed class UWPDynamicLibraryFormat : NativeProgramFormat<MsvcDynamicLinker>
    {
        public override string Extension { get; } = "dll";

        public UWPDynamicLibraryFormat(UWPToolchain toolchain) : base(
            new UWPDynamicLinker(toolchain).AsDynamicLibrary())
        {
        }
    }
}
