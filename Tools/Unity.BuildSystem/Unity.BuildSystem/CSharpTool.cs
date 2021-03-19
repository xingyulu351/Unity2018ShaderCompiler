using JamSharp.Runtime;
using NiceIO;
using System;
using System.Collections.Generic;
using Bee.Core;
using Bee.CSharpSupport;
using Bee.DotNet;
using Unity.BuildSystem.Common;
using Unity.BuildSystem.CSharpSupport;
using Unity.BuildTools;

namespace Unity.BuildSystem
{
    /// <summary>
    /// Use this class to help you create a c# tool used in the buildsystem. We have many custom tools that we use to build, this class tries to make it easier to write one, by hiding all the tricky jam
    /// bits in this out-of-sight baseclass.
    /// </summary>
    public abstract class CSharpTool : BuildSystemTool
    {
        protected abstract DotNetAssembly DotNetAssembly();

        static readonly List<Type> s_InstantiatedTools = new List<Type>();

        public static JamTarget ProjectFilesTarget { get; }

        protected virtual DotNetRuntime DotNetRuntime => null;

        static CSharpTool()
        {
            ProjectFilesTarget = new JamTarget("CSharpToolProjectFiles");
            BuiltinRules.Depends("ManagedProjectFiles", ProjectFilesTarget);
        }

        protected CSharpTool()
        {
            if (s_InstantiatedTools.Contains(GetType()))
                throw new ArgumentException($"You're only supposed to instantiate {GetType()} once, but it's being instantiated twice");
            s_InstantiatedTools.Add(GetType());
            ExecutableTarget = new Lazy<JamTarget>(() => new JamTarget(DotNetAssembly().Path));
        }

        private static DotNetRuntime UnityMonoBleedingEdgeRuntime { get; } = MakeUnityMonoBleedingEdgeRuntime();

        private static Bee.DotNet.Mono MakeUnityMonoBleedingEdgeRuntime()
        {
            var distro = Paths.UnityRoot.Combine("External/MonoBleedingEdge/builds/monodistribution");
            return new Bee.DotNet.Mono(HostPlatform.IsLinux ? distro.Combine("bin-linux64/mono") : distro.Combine("bin/mono"));
        }

        protected JamTarget SetupInvocation(JamList target, JamList inputFiles, IEnumerable<string> commandLineArguments, Dictionary<string, string> environmentVariables = null)
        {
            var runtime = DotNetRuntime ?? UnityMonoBleedingEdgeRuntime;
            var command = runtime.InvocationStringFor(ExecutableTarget.Value.Path, debug: true);
            return base.SetupInvocation(target, inputFiles, commandLineArguments, command, environmentVariables);
        }
    }
}
