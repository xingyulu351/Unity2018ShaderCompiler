using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using JamSharp.Runtime;
using Unity.BuildTools;

namespace Unity.BuildSystem.Common
{
    public abstract class BuildSystemTool
    {
        protected Lazy<object> ActionSetup;
        protected Lazy<JamTarget> ExecutableTarget;

        protected BuildSystemTool()
        {
            ActionName = GetType().Name;

            ActionSetup = new Lazy<object>(() =>
            {
                if (ActionsBody != DefaultActionBody && !IRealizeHowDangerousItIsToNotUseDefaultActionBody())
                    throw new Exception($"You are using a non default actionbody in {GetType()}, please read the comments in {nameof(IRealizeHowDangerousItIsToNotUseDefaultActionBody)}");

                JamCore.RegisterAction(ActionName, ActionsBody, ActionFlags);
                return null;
            });
        }

        protected virtual string ActionName { get; }

        string DefaultActionBody => SupportsResponseFile
        ? "$(ENVIRONMENTSETUPPREFIX)$(PROGRAM) @@($(ARGS))"
        : "$(ENVIRONMENTSETUPPREFIX)$(PROGRAM) $(ARGS)";

        protected virtual ActionsFlags ActionFlags => ActionsFlags.Response;
        protected virtual string ActionsBody => DefaultActionBody;
        protected virtual bool SupportsResponseFile => false;

        protected virtual bool IRealizeHowDangerousItIsToNotUseDefaultActionBody()
        {
            //If you find yourself overriding this method to return true, please make sure you're aware of all the following
            //
            //The only need to not use the default action body, is if you need to have variables in the actionbody, which at the time
            //of SetupInvocation() do not have the value yet you want them to have. In all other cases you can and should
            //use the default action body.
            //
            //The danger of not using the default action body, is that it's the value of $(ARGS) inside of it, that's used
            //for "UseCommandLine", which is the system that figures out of the results from previous build of this target
            //can be recycled for the current build.  if the value of $(ARGS) is not complete, it means we have incomplete information
            //on the commandline used, which means that it's possible for the commandline to change, jam not to realize that,
            //and jam would incorrectly decide that the target does not require rebuilding.
            //
            //So why even have the option to use a non default actionbody at all? The only valid reason is if you want to
            //turn an existing tool into a CSharpTool, but you cannot in one go change all the buildcode that uses the tool,
            //and the existing buildcode is written in a way that it uses variables in the actionbody that do not yet have
            //their correct values or binding set.

            return false;
        }

        readonly HashSet<string> _setupTargets = new HashSet<string>();

        protected virtual JamTarget SetupInvocation(
            JamList targets,
            JamList inputFiles,
            IEnumerable<string> commandLineArguments,
            string executableString,
            Dictionary<string, string> environmentVariables = null)
        {
            ActionSetup.EnsureValueIsCreated();

            var targetName = targets.First();

            var hasTogetherFlag = (ActionFlags & ActionsFlags.Together) != 0;
            if (!hasTogetherFlag && (!_setupTargets.Add(targetName)))
                throw new ArgumentException($"A target is being setup more than once, without ActionFlags.Together: {targetName}");

            var environmentSetupPrefix = new StringBuilder();

            if (environmentVariables != null)
            {
                if (HostPlatform.IsWindows)
                    environmentSetupPrefix.Append($"perl {Paths.UnityRoot}/Tools/Build/EnvironmentHelper.pl ");
                foreach (var en in environmentVariables)
                    environmentSetupPrefix.Append($"{en.Key}={en.Value} ");
            }

            var targetToCreate = new JamTarget(targetName);
            "ENVIRONMENTSETUPPREFIX".On(targets).Assign(environmentSetupPrefix.ToString());
            "ARGS".On(targets).Assign(commandLineArguments.SeparateWithSpace());
            "PROGRAM".On(targets).Assign(executableString);

            if (ExecutableTarget != null)
                targetToCreate.DependOn(ExecutableTarget.Value);

            BuiltinRules.Depends(targets, inputFiles);

            BuiltinRules.InvokeJamAction(ActionName, targets);
            return targetToCreate;
        }
    }
}
