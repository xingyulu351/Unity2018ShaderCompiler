using System.Collections.Generic;
using System.Linq;
using System.Text;
using Bee.Core;
using NiceIO;
using Unity.BuildSystem.NativeProgramSupport;
using Unity.BuildTools;

namespace Bee.Toolchain.GNU
{
    public class ArBundleStaticLinker : StaticLinker
    {
        protected override bool SupportsResponseFile => false;

        public ArBundleStaticLinker(ToolChain toolChain) : base(toolChain)
        {
        }

        private static NPath MriScriptFor(NPath destination) => destination.ChangeExtension("mri");

        protected override IEnumerable<string> CommandLineFlagsFor(NPath destination, CodeGen codegen, IEnumerable<NPath> objectFiles,
            IEnumerable<PrecompiledLibrary> nonToolchainSpecificLibraries, IEnumerable<PrecompiledLibrary> allLibraries)
        {
            yield return $"-M < {MriScriptFor(destination).InQuotes()}";
        }

        protected override IEnumerable<NPath> InputFilesFor(NPath destination, CodeGen codegen, IEnumerable<NPath> objectFiles,
            IEnumerable<PrecompiledLibrary> nonToolchainSpecificLibraries, IEnumerable<PrecompiledLibrary> allLibraries)
        {
            // "ar" does not have an easy way of adding contents of a static library
            // into a destination library; the only way to get that is through "MRI scripts".
            // So setup the MRI script and use it in ar invocation.
            var objectFilesArray = objectFiles.ToArray();
            var nonToolchainSpecificLibrariesArray = nonToolchainSpecificLibraries.ToArray();

            var sb = new StringBuilder();
            sb.AppendLine($"create {destination}");
            foreach (var l in nonToolchainSpecificLibrariesArray)
                sb.AppendLine($"addlib {l}");
            foreach (var o in objectFilesArray)
                sb.AppendLine($"addmod {o}");
            sb.AppendLine("save");
            sb.AppendLine("end");
            sb.AppendLine();

            var mriScript = MriScriptFor(destination);
            Backend.Current.AddWriteTextAction(mriScript, sb.ToString());

            return base.InputFilesFor(destination, codegen, objectFilesArray, nonToolchainSpecificLibrariesArray, allLibraries)
                .Append(mriScript);
        }
    }
}
