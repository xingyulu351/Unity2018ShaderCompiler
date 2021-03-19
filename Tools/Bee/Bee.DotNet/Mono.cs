using System.Collections.Generic;
using System.Diagnostics;
using Bee.Core;
using NiceIO;

namespace Bee.DotNet
{
    public class Mono : DotNetRuntime
    {
        public NPath PathToMonoBinary { get; }

        public Mono(NPath pathToMonoBinary) => PathToMonoBinary = pathToMonoBinary;

        public override string InvocationStringFor(NPath programPath, bool debug) => $"{PathToMonoBinary} {(debug ? "--debug " : "")}{programPath}";

        public override IEnumerable<Framework> SupportedFrameworks { get; } = new Framework[] {Framework.Framework46, Framework.Framework47, Framework.NetStandard20};

        public override IEnumerable<DebugFormat> DebugFormats { get; } = new DebugFormat[] {DebugFormat.Mdb, DebugFormat.PortablePdb};
    }
}
