using System;
using System.Collections.Generic;
using System.Linq;
using Bee.NativeProgramSupport.Core;
using Bee.NativeProgramSupport.Running;
using NiceIO;
using Unity.BuildTools;

namespace Bee.Toolchain.Emscripten
{
    internal class EmscriptenRunner : RunnerImplementation<EmscriptenRunnerEnvironment>
    {
        private readonly NPath nodeExecutableOverride;

        internal EmscriptenRunner(EmscriptenRunnerEnvironment data, string nodeExecutableOverride) : base(data)
        {
            if (nodeExecutableOverride != null)
                this.nodeExecutableOverride = nodeExecutableOverride;
        }

        private NPath NodeBinary
        {
            get
            {
                if (nodeExecutableOverride != null && nodeExecutableOverride.Exists())
                    return nodeExecutableOverride;

                var unityNodeBinary = UnityNodeBinary();
                if (unityNodeBinary.FileExists())
                    return unityNodeBinary;

                return "node" + HostPlatform.Exe;
            }
        }

        private static NPath UnityNodeBinary()
        {
            if (HostPlatform.IsWindows)
                return Paths.UnityRoot.Combine("External/nodejs/builds/win64/node.exe");
            if (HostPlatform.IsOSX)
                return Paths.UnityRoot.Combine("External/nodejs/builds/osx/bin/node");
            if (HostPlatform.IsLinux)
                return Paths.UnityRoot.Combine("External/nodejs/builds/linux64/bin/node");
            throw new ArgumentException("Unknown platform");
        }

        public override Tuple<LaunchResult, int> Launch(NPath appExecutable, InvocationResult appOutputFiles, bool forwardConsoleOutput, IEnumerable<string> args = null, int timeoutMs = -1)
        {
            var nodeJsArgs = appExecutable.MakeAbsolute().InQuotes()     // Important to make absolute since otherwise this may be interpreted as being relative to the node installation
                .AsEnumerable().Concat(args ?? Array.Empty<string>());   // Concat args.
            return LaunchUtils.LaunchLocal(NodeBinary, appOutputFiles, forwardConsoleOutput, nodeJsArgs, timeoutMs, appExecutable.Parent);
        }
    }
}
