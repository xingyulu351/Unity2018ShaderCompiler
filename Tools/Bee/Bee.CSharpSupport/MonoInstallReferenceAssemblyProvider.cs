using System;
using System.Linq;
using Bee.DotNet;
using NiceIO;
using Unity.BuildTools;

namespace Unity.BuildSystem.CSharpSupport
{
    public class MonoInstallReferenceAssemblyProvider : ReferenceAssemblyProvider
    {
        NPath Path { get; }

        public MonoInstallReferenceAssemblyProvider(NPath path)
        {
            Path = path;
        }

        public override int PreferenceScore => throw new NotImplementedException();

        public override bool TryFor(Framework framework, bool exactMatch, out NPath[] result, out string reason)
        {
            result = null;
            reason = null;
            if (framework == Framework.NetStandard20 || framework == Framework.NetStandard13)
            {
                reason = "No support yet for targetting netstandard targetframework from a mono install reference assembly";
                return false;
            }

            NPath monoPath = Path.Combine("lib/mono");
            if (!monoPath.DirectoryExists())
                throw new NotSupportedException($"No mono installation found at {monoPath}");


            var apiDirNameFor = ApiDirNameFor(framework);
            if (apiDirNameFor == null)
                return false;

            var apiDir = monoPath.Combine(apiDirNameFor);
            if (!apiDir.DirectoryExists())
            {
                if (framework == Framework.Framework40 && !exactMatch)
                {
                    apiDir = monoPath.Combine("4.0");
                    if (!apiDir.DirectoryExists())
                        return false;
                    if (!apiDir.Files("*.dll").Any())
                    {
                        apiDir = monoPath.Combine("4.5");
                        if (!apiDir.DirectoryExists())
                            return false;
                    }
                }
                else
                {
                    return false;
                }
            }

            result = apiDir.Files("*.dll").Where(f => MatchesWhiteList(f, framework)).ToArray();
            return true;
        }

        private bool MatchesWhiteList(NPath file, Framework framework)
        {
            //unfortunately, the XXX-api directories that mono ships, are not yet correct, in that there is more stuff in there, than is in the official
            //profile the directory represents. We're going to have to manually filter until the mono guys get that resolved.

            return ReferenceAssemblyNamesFor(framework).Contains(file.FileName, StringComparer.InvariantCultureIgnoreCase);
        }

        private static string ApiDirNameFor(Framework framework)
        {
            if (!framework.IsDotNetFramework)
                return null;
            // mono serves 3.5 and 2.0 from the same folder
            if (framework == Framework.Framework35)
                framework = Framework.Framework20;
            return framework.Name.StripStart("v") + "-api";
        }
    }
}
