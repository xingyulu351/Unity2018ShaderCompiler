using System;
using System.CodeDom;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Reflection;
using System.Text;
using System.Threading;
using Bee.Core;
using Bee.Core.Tests;
using Bee.DotNet;
using Bee.Tools;
using JamSharp.Runtime;
using Moq;
using NiceIO;
using NUnit.Framework;
using Unity.BuildSystem.CSharpSupport;
using Unity.BuildTools;

namespace Bee.Tests
{
    public class BeeAPITestBackend_StandaloneBee<T> : BeeAPITestBackend<T>
    {
        public override bool RunsOutOfProcess => true;
        private static bool CanRunOnThisHost()
        {
            return new SystemInstalledReferenceAssemblyProvider().TryFor(Framework.Framework46, false, out var _1, out var _2);
        }

        public override string Name => "StandaloneBee";

        public override Shell.ExecuteResult Build(Func<T> firstCallThis, Action<T> thenCallThis, bool throwOnFailure, string[] requestedTargets)
        {
            if (!CanRunOnThisHost())
                Assert.Ignore("Unable to run standalone bee test, no .net46 reference assemblies on machine");

            var buildFunction = SerializedFunction.From(thenCallThis);

            var readTextFromResource =
                ResourceHelper.ReadTextFromResource(typeof(BeeAPITestBackend_StandaloneBee<T>).Assembly,
                    "BeeBackend_build");
            TestRoot.Combine("bee.cs").WriteAllText(readTextFromResource);

            var argElements = SerializedFunction.From(firstCallThis).AsStrings.Concat(buildFunction.AsStrings).ToArray();

            TestRoot.Combine("serialized_function_of_buildcode_under_test").WriteAllText(argElements.SeparateWith("\n"));

            return InvokeStandaloneBee(throwOnFailure, requestedTargets);
        }

        public Shell.ExecuteResult InvokeStandaloneBee(bool throwOnFailure = true, string[] requestedTargets = null)
        {
            var driverExeLocation = typeof(StandaloneBeeDriver).Assembly.Location;

            var executeArgs = new Shell.ExecuteArgs()
            {
                WorkingDirectory = TestRoot.ToString(SlashMode.Native),
                Executable = driverExeLocation,
                Arguments = requestedTargets?.SeparateWithSpace() ?? string.Empty,
                StdMode = Shell.StdMode.Capture,
                EnvVars = {{ConsoleColors.EnvironmentVariableNameIndicatingCanHandleColors, "0"}}
            };

            TestContext.Out.WriteLine($"Invoking {executeArgs.Executable} with {executeArgs.Arguments}");

            MultiProcessDebugHelper.Clear();
            MultiProcessDebugHelper.WriteLine($"Invoking {executeArgs.Executable} with {executeArgs.Arguments}");

            var executeResult = Shell.Execute(executeArgs);

            if (executeResult.ExitCode != 0 && throwOnFailure)
                throw new AssertionException("Build failed: " + executeResult.StdOut + executeResult.StdErr);

            TestContext.Out.WriteLine(executeResult.StdOut);
            TestContext.Out.WriteLine(executeResult.StdErr);

            return executeResult;
        }
    }
}
