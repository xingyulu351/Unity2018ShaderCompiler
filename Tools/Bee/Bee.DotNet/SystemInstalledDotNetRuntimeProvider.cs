using System;
using System.Collections.Generic;
using NiceIO;
using Unity.BuildTools;

namespace Bee.DotNet
{
    public class SystemInstalledDotNetRuntimeProvider : DotNetRuntime.DotNetRuntimeProvider
    {
        public override IEnumerable<DotNetRuntime> Provide()
        {
            var environmentVariable = Environment.GetEnvironmentVariable("MONO_EXECUTABLE");
            if (!HostPlatform.IsWindows && !string.IsNullOrEmpty(environmentVariable))
            {
                yield return new Mono(environmentVariable);
                yield break;
            }

            if (HostPlatform.IsOSX)
            {
                var mono = new NPath("/usr/local/bin/mono");
                if (mono.FileExists())
                    yield return new Mono(mono);

                mono = new NPath("/Library/Frameworks/Mono.framework/Versions/Current/bin/mono");
                if (mono.FileExists())
                    yield return new Mono(mono);
            }

            if (HostPlatform.IsLinux)
            {
                var mono = new NPath("/usr/bin/mono");
                if (mono.FileExists())
                    yield return new Mono(mono);
            }

            if (HostPlatform.IsWindows)
                yield return new SystemDotNet();
        }
    }
}
