using System;
using System.Diagnostics;
using System.IO;
using System.IO.Ports;
using System.Linq;
using Bee.Core;
using Bee.DotNet;
using Bee.NativeProgramSupport.Core;
using Bee.NativeProgramSupport.Running;
using NiceIO;
using Unity.BuildTools;

namespace Bee.ProgramRunner.CLI
{
    public static class ProgramRunnerCLI
    {
        public static readonly NPath ProgramRunnerExePath = typeof(ProgramRunnerCLI).Assembly.Location;
        public static readonly RunnableProgram ProgramRunnerExe = new DotNetRunnableProgram(new DotNetAssembly(ProgramRunnerExePath, Framework.Framework46));
        public static readonly string InvocationString = ProgramRunnerExe.InvocationString + " run";

        private static ProgramRunnerOptions options = new ProgramRunnerOptions();

        public static void WriteLogLine(string line = "")
        {
            Console.Error.Write($"[BeeProgramRunner] {line}\n");
        }

        public static void WriteOutputLine(string line = "")
        {
            Console.Out.Write($"{line}\n");
        }

        static RunnerEnvironment[] ResolvePassedEnvironmentIdentifier()
        {
            if (options.runnerEnvironmentIdentifier == null)
            {
                WriteLogLine("No runner environment specified!");
                return null;
            }

            RunnerEnvironment[] environments = null;
            try
            {
                environments = ProgramRunnerDaemon.FindRunnerEnvironments(options.runnerEnvironmentIdentifier)?.ToArray() ?? Array.Empty<RunnerEnvironment>();
            }
            catch (Exception e)
            {
                WriteLogLine(e.Message);
                return null;
            }

            if (environments.Empty())
            {
                WriteLogLine($"Did not find a matching runner environment for environment identifier \"{options.runnerEnvironmentIdentifier}\"!");
                return null;
            }

            // Some more information for the user if we have multiple environments.
            if (environments.Length > 1)
            {
                WriteLogLine($"Found multiple matching runner environments for identifier \"{options.runnerEnvironmentIdentifier}\":");
                foreach (RunnerEnvironment environment in environments)
                    WriteLogLine($"* {environment.Identifier}");
                WriteLogLine();
            }

            return environments;
        }

        static RunnerEnvironment GetFirstEnvironmentThatCanLaunch(RunnerEnvironment[] environments)
        {
            RunnerEnvironment firstEnvironmentThatCanLaunch = environments.FirstOrDefault(x => x.Status == RunnerEnvironment.EnvironmentStatus.FullAccess);
            if (firstEnvironmentThatCanLaunch != null)
                return firstEnvironmentThatCanLaunch;

            WriteLogLine($"Did not find any environment(s) that can launch applications for the given identifier \"{options.runnerEnvironmentIdentifier}\".");

            foreach (var environment in environments)
            {
                if (environment.Status == RunnerEnvironment.EnvironmentStatus.FullAccess)
                    continue;

                switch (environment.Status)
                {
                    case RunnerEnvironment.EnvironmentStatus.Offline:
                        WriteLogLine($"Can't launch application on runner enviroment \"${environment.Identifier}\" since the enviroment is not reachable.");
                        break;
                    case RunnerEnvironment.EnvironmentStatus.NoLaunchPermission:
                        WriteLogLine($"Can't launch application on runner enviroment \"${environment.Identifier}\" since we do have not full access/permission right now.");
                        break;
                }
            }

            return null;
        }

        static int Launch(RunnerEnvironment runnerEnvironment)
        {
            if (options.appPath == null || string.IsNullOrEmpty(options.appPath.ToString()))
            {
                WriteLogLine("No executable given to execute. Specify --app-path");
                return 1;
            }
            if (!options.appPath.Exists())
            {
                WriteLogLine($"Given executable \"{options.appPath}\" does not exist.");
                return 1;
            }

            WriteLogLine($"Launching application \"{options.appPath}\" on runner environment \"{runnerEnvironment.Identifier}\"...");

            var invocationResult = new InvocationResult()
            {
                Result = options.appExitCode?.MakeAbsolute(),
                Stderr = options.appErr?.MakeAbsolute(),
                Stdout = options.appOut?.MakeAbsolute()
            };

            Tuple<LaunchResult, int> launchResult;
            try
            {
                launchResult = runnerEnvironment.InstantiateRunner(options.runnerArgs).Launch(
                    options.appPath,
                    invocationResult,
                    options.forwardConsoleOutput,
                    options.extra);
            }
            catch (Exception e)
            {
                WriteLogLine(e.ToString());
                return 1;
            }

            switch (launchResult.Item1)
            {
                case LaunchResult.Success:
                case LaunchResult.QuitWithError:
                    if (options.appIgnoreFailure || launchResult.Item2 == 0)
                    {
                        WriteLogLine($"Application quit with exit code {launchResult.Item2}");
                        return 0;
                    }
                    else
                    {
                        WriteLogLine($"Application quit with exit code {launchResult.Item2} (forwarding exit code)");
                        return launchResult.Item2;
                    }

                case LaunchResult.Timeout:
                    WriteLogLine("Launched application reached timeout and was cancelled.");
                    return options.appIgnoreFailure ? 0 : 1;

                default:
                    new NotImplementedException();
                    break;
            }

            return 0;
        }

        /// <summary>
        /// Entrypoint for invoking ProgramRunner when not embedded into Bee.
        /// </summary>
        private static void Main(string[] args)
        {
            var runTopLevelCommand = new RunTopLevelBeeCommand();
            string firstArg = args.FirstOrDefault();
            if (firstArg == runTopLevelCommand.Name || firstArg == runTopLevelCommand.Abbreviation)
                RunnerMain(args.Skip(1).ToArray());
            else
                throw new Exception($"First argument must be '{runTopLevelCommand.Name}' (for 'bee' compatibility)");
        }

        public static void RunnerMain(string[] args)
        {
            options.ParseCommandLine(args);
            bool didSomething = false;

            if (options.daemonMode)
            {
                didSomething = true;
                try
                {
                    using (var daemon = new DaemonProgram())
                    {
                        daemon.Run();
                    }
                }
                catch (Exception e)
                {
                    RunnerEnvironment.RunnerEnvironmentFilesDirectory.Combine("crashlog.txt").WriteAllText($"Daemon with pid {Process.GetCurrentProcess().Id} crashed:\n\n{e}");
                    Environment.Exit(2);
                }
            }

            if (options.listAvailableEnvironments)
            {
                didSomething = true;
                foreach (var environment in ProgramRunnerDaemon.LoadAllRunnerEnvironments())
                {
                    if (environment.Status == RunnerEnvironment.EnvironmentStatus.FullAccess)
                        WriteOutputLine(environment.Identifier);
                }
            }

            if (options.checkCanLaunch || options.launch)
            {
                didSomething = true;

                var environments = ResolvePassedEnvironmentIdentifier();

                if (environments == null)
                    Environment.Exit(1);

                var firstEnvironmentThatCanLaunch = GetFirstEnvironmentThatCanLaunch(environments);
                if (firstEnvironmentThatCanLaunch == null)
                    Environment.Exit(1);

                if (options.launch)
                    Environment.Exit(Launch(firstEnvironmentThatCanLaunch));
            }


            if (!didSomething)
            {
                WriteLogLine("No action to execute. Use --help for a full list of commands.");
                Environment.Exit(1);
            }
        }
    }
}
