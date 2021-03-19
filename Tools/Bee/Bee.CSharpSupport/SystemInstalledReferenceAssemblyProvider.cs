using System;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Runtime.Remoting.Messaging;
using Bee.DotNet;
using Microsoft.Win32;
using NiceIO;
using Unity.BuildTools;

namespace Unity.BuildSystem.CSharpSupport
{
    public class SystemInstalledReferenceAssemblyProvider : ReferenceAssemblyProvider
    {
        private readonly MonoInstallReferenceAssemblyProvider _monoProvider;

        public SystemInstalledReferenceAssemblyProvider()
        {
            _monoProvider  = FindReferenceAssemblyProvider();
        }

        private MonoInstallReferenceAssemblyProvider FindReferenceAssemblyProvider()
        {
            if (HostPlatform.IsOSX)
            {
                NPath homeBrewDir = "/usr/local/lib/mono";
                if (homeBrewDir.DirectoryExists())
                    return new MonoInstallReferenceAssemblyProvider("/usr/local");

                NPath xamarinInstallDir = "/Library/Frameworks/Mono.Framework/Versions/Current";
                if (xamarinInstallDir.DirectoryExists())
                    return new MonoInstallReferenceAssemblyProvider(xamarinInstallDir);
            }

            if (HostPlatform.IsLinux)
                return new MonoInstallReferenceAssemblyProvider("/usr");

            return null;
        }

        public override int PreferenceScore => 0;

        public override bool TryFor(Framework framework, bool exactMatch, out NPath[] result, out string reason)
        {
            if (_monoProvider != null)
                return _monoProvider.TryFor(framework, exactMatch, out result, out reason);

            if (HostPlatform.IsWindows)
            {
                NPath allReferencePacks = @"C:\Program Files (x86)\Reference Assemblies\Microsoft\Framework\.NETFramework";
                if (allReferencePacks.DirectoryExists())
                {
                    var dirName = framework.Name;
                    if (dirName != null)
                    {
                        var refDir = allReferencePacks.Combine(dirName);
                        if (refDir.DirectoryExists())
                        {
                            result = refDir.Files("*.dll");
                            reason = null;
                            return true;
                        }
                    }
                }

                //todo: query Software\Microsoft\NET Framework Setup\NDP\v4\Full  for Version entry and InstallPath entry
                NPath dir = @"C:\Windows\Microsoft.NET\Framework64\v4.0.30319";
                if (dir.DirectoryExists())
                {
                    result = dir.Files("*.dll").Where(f => ReferenceAssemblyNamesFor(framework).Contains(f.FileName, StringComparer.InvariantCultureIgnoreCase)).ToArray();
                    reason = null;
                    return true;
                }
            }

            reason = "System installed reference assemblies not yet supported on non-osx";
            result = null;
            return false;
        }
    }
}
