using System;
using System.Collections.Generic;
using Mono.Options;
using NiceIO;

namespace Bee.ProgramRunner.CLI
{
    class ProgramRunnerOptions
    {
        public bool daemonMode;
        public string runnerEnvironmentIdentifier;
        public bool listAvailableEnvironments;
        public bool launch;
        public bool checkCanLaunch;
        public NPath appPath;
        public NPath appOut;
        public NPath appErr;
        public NPath appExitCode;
        public bool appIgnoreFailure;
        public bool forwardConsoleOutput;
        public string runnerArgs;
        public bool printHelp;

        public List<string> extra;

        public void ParseCommandLine(string[] args)
        {
            var options = new OptionSet
            {
                {"daemon", "daemon mode for maintaining RunnerEnvironment files. If there is an already running daemon, the program returns immediately if it uses the same executable path and arguments. Otherwise it will attempt to shutdown the old daemon.", p => daemonMode = true, true},

                {"list-available-environments", "lists all available environments", p => listAvailableEnvironments = true },
                {"environment=", "An identifier of the form \"<Platform>[.<environmentId>][@<requiredArchitecture>,<requiredArchitecture>,...]\" which identifies on which environment we operate", p => runnerEnvironmentIdentifier = p},
                {"launch", "start application and take all necessary steps to make it work (includes deploying etc.). Argumentes after '--' will be passed on the application.", p => { launch = true; }},
                {"can-launch", "check whether we can launch applications on the given environment - return value is 0 if yes", p => { checkCanLaunch = true; }},
                {"app-path=", "path to application (on local machine)", p => appPath = p},
                {"app-std=", "path to where std-out output from application should be written to", p => appOut = p},
                {"app-err=", "path to where std-err output from application should be written to", p => appErr = p},
                {"app-exit=", "path to where text file with exit code from application should be written to", p => appExitCode = p},
                {"app-ignore-failure", "do not forward error code of application", p => { appIgnoreFailure = true; }},
                {"app-forward-console-output", "forward console output of application", p => { forwardConsoleOutput = true; }},
                {"runner-args=", "optional argument string passed to the runner on instantiation. " +
                 "These are *not* arguments that are passed to any programs that might be run by the runner. " +
                 "Instead, they allow Runner/RunnerEnvironment specific control on how the application is run.", p => runnerArgs = p},

                {"?|help", "print help", (b) => { printHelp = true; }}
            };

            try
            {
                // parse the command line
                extra = options.Parse(args);
            }
            catch (OptionException e)
            {
                // output some error message
                ProgramRunnerCLI.WriteLogLine("Error parsing: ");
                ProgramRunnerCLI.WriteLogLine(e.Message);
                ProgramRunnerCLI.WriteLogLine("Try '--help' for more information.");
                Environment.Exit(1);
            }

            if (printHelp)
            {
                options.WriteOptionDescriptions(Console.Out);
                Environment.Exit(0);
            }
        }
    }
}
