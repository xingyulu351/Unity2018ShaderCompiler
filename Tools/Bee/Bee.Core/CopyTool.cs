using System;
using System.Collections.Generic;
using NiceIO;
using Unity.BuildTools;

namespace Bee.Core
{
    public class CopyTool
    {
        static CopyTool _instance;
        public static CopyTool Instance() => _instance ?? (_instance = new CopyTool());

        internal CopyTool()
        {
        }

        private readonly Dictionary<IBackend, Dictionary<NPath, NPath>> _alreadySetup = new Dictionary<IBackend, Dictionary<NPath, NPath>>();

        Dictionary<NPath, NPath> AlreadySetupCacheFor(IBackend backend)
        {
            if (_alreadySetup.TryGetValue(backend, out var result))
                return result;
            result = new Dictionary<NPath, NPath>();
            _alreadySetup[backend] = result;
            return result;
        }

        private NPath OriginalSourceOf(NPath target)
        {
            var alreadySetupCache = AlreadySetupCacheFor(Backend.Current);
            while (alreadySetupCache.ContainsKey(target))
                target = alreadySetupCache[target];
            return target;
        }

        public NPath Setup(NPath target, NPath from)
        {
            var alreadySetupCache = AlreadySetupCacheFor(Backend.Current);
            if (alreadySetupCache.TryGetValue(target, out var previouslySetupSource))
            {
                if (from == previouslySetupSource)
                {
                    return target;
                }

                if (OriginalSourceOf(from) == OriginalSourceOf(previouslySetupSource))
                {
                    return target;
                }

                throw new ArgumentException(
                    $"You're trying to copy to {target} more than once. Previously from {previouslySetupSource} and now from {from}");
            }

            alreadySetupCache.Add(target, from);

            Backend.Current.AddAction(
                "CopyTool",
                new[] {target},
                new[] {from},
                ExecutableStringForCopyOf(target, @from),
                new string[] {},  //from.InQuotes(SlashMode.Native), target.InQuotes(SlashMode.Native)}
                allowedOutputSubstrings: new[] { ".*" } // Swallow all output from a successful run
            );


            return target;
        }

        private static string ExecutableStringForCopyOf(NPath target, NPath @from)
        {
            return HostPlatform.IsWindows
                /*
                 * we have to shove the entire command into the executable name because if we try to run two separate copy
                 * command actions, we will throw an exception with "A target is being setup more than once, without
                 * ActionsFlags.Together", because the second copy command modifies the destination file in place.
                 *
                 * then, we have to do the same thing on *nix that we do on windows, because the command line arguments
                 * are shared.
                 */
                ? $"copy {@from.InQuotes(SlashMode.Native)} {target.InQuotes(SlashMode.Native)} && copy /b {target.InQuotes(SlashMode.Native)} +,, {target.InQuotes(SlashMode.Native)}"
                : $"cp {@from.InQuotes(SlashMode.Native)} {target.InQuotes(SlashMode.Native)}";
        }
    }
}
