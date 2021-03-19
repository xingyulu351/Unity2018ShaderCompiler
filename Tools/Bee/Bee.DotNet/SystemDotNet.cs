using System.Collections.Generic;
using NiceIO;

namespace Bee.DotNet
{
    public class SystemDotNet : DotNetRuntime
    {
        public override IEnumerable<Framework> SupportedFrameworks { get; } = new Framework[] {Framework.Framework46, Framework.Framework47, Framework.NetStandard20};
        public override IEnumerable<DebugFormat> DebugFormats { get; }  = new DebugFormat[] {DebugFormat.Pdb, DebugFormat.PortablePdb};
        public override string InvocationStringFor(NPath programPath, bool debug) => programPath.ToString(SlashMode.Native);
    }
}
