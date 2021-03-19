using System;
using System.Collections.Generic;
using JamSharp.Runtime;
using NiceIO;
using Unity.BuildTools;

namespace Unity.BuildSystem.Common
{
    public class Copy
    {
        const string CopyActionName = "Unity.BuildSystem.Common.Copy";

        private static readonly Lazy<JamList> RegisterAction = new Lazy<JamList>(() =>
        {
            JamCore.RegisterAction(CopyActionName,
                HostPlatform.IsWindows
                /*
                 * copy on windows by default does not update the timestamp of the destination file
                 * but if after doing the copy, you do `copy /b mydestinationfile.exe+,, mydestinationfile.exe`
                 * (note the `+,,` part AND the mentioning the file twice)
                 * that will have the same effect as `touch mydestinationfile.exe` would on unix
                 */
                ? $"copy \"$(2:\\)\" \"$(1:\\)\" && copy /b \"$(1:\\)\" +,, \"$(1:\\)\""
                : $"cp \"$(2)\" \"$(1)\"");
            return null;
        });


        static Dictionary<NPath, NPath> s_AlreadySetupCopies = new Dictionary<NPath, NPath>();

        public static JamTarget Setup(JamTarget from, JamTarget dest)
        {
            RegisterAction.EnsureValueIsCreated();

            if (from.Equals(dest))
                throw new ArgumentException($"You are trying to copy {from} ontop of itself");

            NPath previouslySetupSource;

            if (s_AlreadySetupCopies.TryGetValue(dest.Path, out previouslySetupSource))
            {
                if (from.Path == previouslySetupSource)
                    return dest;
                throw new ArgumentException($"You're trying to copy to {dest} more than once. Previously from {previouslySetupSource} and now from {from}");
            }

            s_AlreadySetupCopies.Add(dest.Path, from.Path);

            BuiltinRules.InvokeJamAction(CopyActionName, dest, from);
            BuiltinRules.Depends(dest, from);
            return dest;
        }
    }
}
