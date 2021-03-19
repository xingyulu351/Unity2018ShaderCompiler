using System;
using System.Linq;
using NiceIO;

namespace Unity.BuildSystem.JamStateToTundraConverter
{
    class Replacement
    {
        public string StringToReplace { get; }
        public string ReplacementContents { get; }
        public bool IsResponseReplacement { get; }
        public string OriginalName { get; }

        public Replacement(string stringToReplace, string replacementContents, bool isResponseReplacement, string originalName = null)
        {
            StringToReplace = stringToReplace;
            ReplacementContents = replacementContents;
            IsResponseReplacement = isResponseReplacement;
            OriginalName = originalName;
        }
    }

    class ShellCommand
    {
        public string Command { get; }
        public Replacement[] Replacements { get; }
        public NPath DepsFile { get; }
        public bool HasShowIncludes { get; }

        public string CommandWithNonResponseCommandsFlattened
        {
            get
            {
                var s = Command;
                foreach (var replacement in Replacements.Where(r => !r.IsResponseReplacement))
                    s = s.Replace(replacement.StringToReplace, replacement.ReplacementContents);
                return s;
            }
        }

        public ShellCommand(string command, NPath depsFile = null, Replacement[] replacements = null, bool hasShowIncludes = false)
        {
            Command = command;
            DepsFile = depsFile;
            HasShowIncludes = hasShowIncludes;
            Replacements = replacements ?? Array.Empty<Replacement>();
        }
    }
}
