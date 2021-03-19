using System;
using JamSharp.Runtime;
using Unity.BuildTools;
using static JamSharp.Runtime.BuiltinRules;

namespace Unity.BuildSystem.Common
{
    public class FailWithError
    {
        const string ActionName = "Unity.BuildSystem.Common.FailWithError";

        static readonly Lazy<JamList> RegisterAction = new Lazy<JamList>(() =>
        {
            var commentExpr = HostPlatform.IsWindows ? "::" : "#";
            var exitExpr = HostPlatform.IsWindows ? "exit /b 1" : "false";

            JamCore.RegisterAction(ActionName, $"{commentExpr} Failed to build $(<): $(MESSAGE)\n{exitExpr}", ActionsFlags.Quietly);
            return null;
        });

        public static JamList Setup(JamList targets, string failureMessage)
        {
            RegisterAction.EnsureValueIsCreated();

            NotFile(targets);
            "MESSAGE".On(targets).Assign(failureMessage);

            InvokeJamAction(ActionName, targets);
            return targets;
        }
    }
}
