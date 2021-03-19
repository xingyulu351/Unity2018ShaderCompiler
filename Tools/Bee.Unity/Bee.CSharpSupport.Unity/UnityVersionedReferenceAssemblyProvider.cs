using System;
using Bee.DotNet;
using NiceIO;
using Unity.BuildSystem.CSharpSupport;
using Unity.BuildTools;

namespace Bee.CSharpSupport.Unity
{
    public class UnityVersionedReferenceAssemblyProvider : ReferenceAssemblyProvider
    {
        readonly MonoInstallReferenceAssemblyProvider _monoProvider = new MonoInstallReferenceAssemblyProvider(Paths.UnityRoot.Combine("External/MonoBleedingEdge/builds/monodistribution"));

        public override int PreferenceScore => 1;

        public override bool TryFor(Framework framework, bool exactMatch, out NPath[] result, out string reason)
        {
            if (_monoProvider.TryFor(framework, exactMatch, out result, out reason))
                return true;

            if (framework == Framework.UAP)
            {
                result = Paths.UnityRoot.Combine("PlatformDependent/WinRT/External/ReferenceAssemblies/builds").Files("*.dll");
                return true;
            }

            if (framework == Framework.NetStandard13)
            {
                result = Paths.UnityRoot.Combine("External/NetStandard/builds/ref/1.3").Files("*.dll");
                return true;
            }

            if (framework == Framework.NetStandard20)
            {
                result = Paths.UnityRoot.Combine("External/NetStandard/builds/ref/2.0.0").Files("*.dll", true);
                return true;
            }

            reason = $"No reference assemblies found for {framework}";
            result = null;
            return false;
        }
    }
}
