using System;
using System.Collections.Generic;
using JamSharp.Runtime;
using NiceIO;
using Unity.BuildTools;

namespace Bee.TundraBackend
{
    public class TundraInvoker
    {
        public static Shell.ExecuteResult Invoke(NPath tundraPath, NPath dagFile, NPath workingDirectory, string frontendCommandLine, Shell.StdMode stdMode, bool forceDisableColors, IEnumerable<string> requestedTargets = null)
        {
            var preArg = HostPlatform.IsWindows ? "-U" : "";
            var executeArgs = new Shell.ExecuteArgs()
            {
                WorkingDirectory = workingDirectory.ToString(SlashMode.Native),
                Executable = tundraPath.ToString(SlashMode.Native),
                Arguments = $"{preArg} -R {dagFile} {requestedTargets?.SeparateWithSpace() ?? string.Empty}",
                StdMode = stdMode,
                EnvVars = new Dictionary<string, string>
                {
                    {
                        "TUNDRA_DAGTOOL_FULLCOMMANDLINE", frontendCommandLine
                    }
                }
            };
            if (forceDisableColors)
                executeArgs.EnvVars["DOWNSTREAM_STDOUT_CONSUMER_SUPPORTS_COLOR"] = "0";

            return Shell.Execute(executeArgs);
        }
    }
}
