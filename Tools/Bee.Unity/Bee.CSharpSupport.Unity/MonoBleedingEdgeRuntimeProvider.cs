using System.Collections.Generic;
using System.Web;
using Bee.DotNet;
using NiceIO;
using Unity.BuildTools;

namespace Bee.CSharpSupport.Unity
{
    public class MonoBleedingEdgeRuntimeProvider : DotNetRuntime.DotNetRuntimeProvider
    {
        public override IEnumerable<DotNetRuntime> Provide()
        {
            yield return Mono();
        }

        public static Mono Mono()
        {
            var bin = HostPlatform.IsLinux ? "bin-linux64" : "bin";
            return new Mono(Paths.UnityRoot.Combine($"External/MonoBleedingEdge/builds/monodistribution/{bin}/mono{HostPlatform.Exe}"));
        }
    }
}
