using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
using Bee.NativeProgramSupport.Core;
using Bee.NativeProgramSupport.Running;
using JamSharp.Runtime;
using NiceIO;
using Unity.BuildSystem.NativeProgramSupport;
using Unity.BuildSystem.VisualStudio;
using Unity.BuildTools;

namespace Bee.Toolchain.UWP
{
    public class UWPRunner : RunnerImplementation<UWPRunnerEnvironment>
    {
        private const string kStdoutFileName = "bee_runner_stdout.txt";
        private const string kStderrFileName = "bee_runner_stderr.txt";
        private const string kExitCodeFileName = "bee_runner_exitcode.txt";

        private static string ApplicationName(NPath appExecutable, string archString) => $@"Bee.UWP.{archString}.{appExecutable.FileNameWithoutExtension}";
        private static NPath ApplicationAppxDir(NPath appExecutable, string archString) => NPath.SystemTemp.Combine(ApplicationName(appExecutable, archString));
        private static string ApplicationAumidFileName => "aumid";
        private static NPath ApplicationAumidFile(NPath appExecutable, string archString) => ApplicationAppxDir(appExecutable, archString).Combine(ApplicationAumidFileName);

        private static string AppXManifest => ResourceHelper.ReadTextFromResource(typeof(UWPRunner).Assembly, "AppXManifest_uwp.xml");

        internal UWPRunner(UWPRunnerEnvironment data) : base(data)
        {
        }

        private string GetArchStringFromExistingBinary(NPath appExecutable)
        {
            Architecture arch = PEHeaderParser.DetectArchitectureFromExecutable(appExecutable);
            if (arch == Architecture.x64)
                return "x64";
            if (arch == Architecture.x86)
                return "x86";
            return "unknown";
        }

        // Returns path to manifest.
        private NPath SetupAppX(NPath appExecutable, string archString)
        {
            var appxDirectory = ApplicationAppxDir(appExecutable, archString);
            appxDirectory.CreateDirectory();

            // Delete unknown files
            var whiteListedFiles = new[] { appExecutable.FileName, ApplicationAumidFileName, "AppXManifest.xml", "Logo.png", "microsoft.system.package.metadata" };
            foreach (var path in Directory.EnumerateFileSystemEntries(appxDirectory.ToString()).Where(path => !whiteListedFiles.Contains(new NPath(path).FileName)))
                File.Delete(path);

            // Create manifest
            var manifestPath = $@"{appxDirectory}\AppXManifest.xml";
            if (!File.Exists(manifestPath))
            {
                string manifestText = AppXManifest
                    .Replace("__APPLICATION_NAME__", ApplicationName(appExecutable, archString))
                    .Replace("__EXECUTABLE_NAME__", appExecutable.FileName)
                    .Replace("__ARCHITECTURE__", archString);
                File.WriteAllText(manifestPath, manifestText);
            }

            // Write out logo.
            var logoPath = $@"{appxDirectory}\Logo.png";
            if (!File.Exists(logoPath))
                ResourceHelper.WriteResourceToDisk(typeof(VisualStudioToolchain).Assembly, "WindowsStoreLogo.png", logoPath);

            // Copy executable
            appExecutable.MakeAbsolute().Copy(appxDirectory.Combine(appExecutable.FileName));

            return manifestPath;
        }

        private bool ActivateAppX(string aumid, IEnumerable<string> args, out int processId)
        {
            // The one thing we can't do from powershell is starting the application AND wait for it to exit.
            // We could start it like this:
            //
            // $startCommand = ""shell:AppsFolder\\"" + $newPackage_FamilyName + ""!App""
            // start $startCommand /Wait
            //
            // But our /Wait will simply be ignored!
            // So instead, we drag in some COM interfaces and do the activation directly.

            var activationManager = new ApplicationActivationManager();
            try
            {
                string argString = args?.SeparateWithSpace() ?? "";
                activationManager.ActivateApplication(aumid, argString, ActivateOptions.NoErrorUI, out processId);
            }
            catch (Exception e)
            {
                WriteLogLine($"Failed launching the application: {e}");
                processId = -1;
                return false;
            }

            return true;
        }

        private LaunchResult WaitForExit(int processId, int timeoutMs, out int exitCode)
        {
            // Wait for application to exit. TODO: Timeout
            exitCode = 0;
            try
            {
                var process = Process.GetProcessById(processId);
                if (timeoutMs <= 0)
                    process.WaitForExit();
                else
                {
                    if (!process.WaitForExit(timeoutMs))
                    {
                        exitCode = -1;
                        try
                        {
                            process.Kill();
                        }
                        catch
                        {
                        }
                        return LaunchResult.Timeout;
                    }
                }
                exitCode = process.ExitCode;
            }
            catch // Get ProcessById will throw if process exits before we call it
            {
            }

            return exitCode == 0 ? LaunchResult.Success : LaunchResult.QuitWithError;
        }

        private static void ExtractResultFiles(InvocationResult appOutputFiles, bool forwardConsoleOutput, string aumid, ref int exitCode)
        {
            // Note that we realtime script forwarding is not easily feasable with UWP.
            // In case we find bee_runner_xy txt files in the applications's temp folder, we forward those.
            // This is the output that the default Main entry point would use.

            string packageFamilyName = aumid.Substring(0, aumid.IndexOf('!'));
            NPath packageOutputFilesDir = $"{Environment.GetFolderPath(Environment.SpecialFolder.ApplicationData)}/../Local/Packages/{packageFamilyName}/AC/Temp";
            NPath appxResultFile = packageOutputFilesDir.Combine(kExitCodeFileName);
            if (appxResultFile.FileExists())
            {
                int.TryParse(appxResultFile.ReadAllText(), out exitCode);
                if (appOutputFiles.Result != null)
                    appxResultFile.Copy(appOutputFiles.Result);
            }

            if (appOutputFiles.Stdout != null || forwardConsoleOutput)
            {
                NPath appxStdoutFile = packageOutputFilesDir.Combine(kStdoutFileName);
                if (appxStdoutFile.FileExists())
                {
                    if (appOutputFiles.Stdout != null)
                        appxStdoutFile.Copy(appOutputFiles.Stdout);
                    if (forwardConsoleOutput)
                    {
                        string stdOutContent = appxStdoutFile.ReadAllText();
                        Console.Out.Write(stdOutContent);
                        if (!stdOutContent.EndsWith("\n"))
                            Console.Out.WriteLine();
                    }
                }
            }

            if (appOutputFiles.Stderr != null || forwardConsoleOutput)
            {
                NPath appxStderrFile = packageOutputFilesDir.Combine(kStderrFileName);
                if (appxStderrFile.FileExists())
                {
                    if (appOutputFiles.Stderr != null)
                        appxStderrFile.Copy(appOutputFiles.Stderr);
                    if (forwardConsoleOutput)
                        Console.Error.Write(appxStderrFile.ReadAllText());
                }
            }
        }

        private bool RegisterAppX(NPath appExecutable, string archString, NPath manifestPath, out string aumid)
        {
            string applicationName = ApplicationName(appExecutable, archString);

            // It could be faster to use WinRT Apis directly as we do in Il2cpp's old WinRTRunner (which inspired this implementation) since starting up a powershell can be quite slow.
            // However then we would need to setup dependencies to WinRT libraries like Windows.Management which is not entirely trivial with our setup.
            // Once we have a powershell running, doing all the operations from there becomes rather convenient though!
            var scriptExecuteArgs = new Shell.ExecuteArgs()
            {
                Executable = "powershell.exe",
                Arguments =
                    $@"-Command
    # Install vclibs if not there already. This is mostly relevant for build machines. Ignore failures on this (package might be non-existing or in a different path)
    $vcLibsPath = ${{Env:ProgramFiles(x86)}} + ""\Microsoft SDKs\Windows Kits\10\ExtensionSDKs\Microsoft.VCLibs\14.0\Appx\Debug\{archString}\Microsoft.VCLibs.{archString}.Debug.14.00.appx""
    Add-AppxPackage $vcLibsPath

    $oldPackage_FullName = (Get-AppxPackage -name {applicationName}).PackageFullName
    if($oldPackage_FullName)
    {{
        echo ""Found prexisting appx with same name, uninstalling""
        Remove-AppxPackage $oldPackage_FullName
    }}

    Add-AppXPackage -Path {manifestPath.InQuotes()} -Register

    $newPackage_FamilyName = (Get-AppxPackage -name {applicationName}).PackageFamilyName
    if ($newPackage_FamilyName)
    {{
        echo $newPackage_FamilyName
    }}
    else
    {{
        echo ""Failed to register AppX!""
        exit 1
    }}".Replace("\"", "\\\"")
            };

            // Don't use Shell.Execute(string) since it splits our little script.
            // Since this is powershell, we can't just safe the script to disk - the user would need to change script execution policies then.
            var scriptResult = Shell.Execute(scriptExecuteArgs);

            if (scriptResult.ExitCode == 0)
            {
                aumid = scriptResult.StdOut.Split('\n').Last().Trim() + "!App";
                ApplicationAumidFile(appExecutable, archString).WriteAllText(aumid);
                return true;
            }

            // Something in the deployment went wrong
            Console.WriteLine(scriptResult.StdOut);
            aumid = "";
            return false;
        }

        // Todo: Be able to pass in a ready .appxmanifest alternatively to an exe.
        public override Tuple<LaunchResult, int> Launch(NPath appExecutable, InvocationResult appOutputFiles, bool forwardConsoleOutput, IEnumerable<string> args = null, int timeoutMs = -1)
        {
            var startTime = DateTime.Now;

            string archString = GetArchStringFromExistingBinary(appExecutable);
            var manifestPath = SetupAppX(appExecutable, archString);
            WriteLogLine($"Deployed: {DateTime.Now - startTime}");

            // Register only if not already registered.
            string aumid = null;
            bool registered = false;
            NPath aumidFile = ApplicationAumidFile(appExecutable, archString);
            if (aumidFile.Exists())
                aumid = aumidFile.ReadAllText();
            if (string.IsNullOrEmpty(aumid))
            {
                if (!RegisterAppX(appExecutable, archString, manifestPath, out aumid))
                    return new Tuple<LaunchResult, int>(LaunchResult.DeploymentFailed, -1);
                WriteLogLine($"Registered: {DateTime.Now - startTime}");
                registered = true;
            }

            // Try launching.
            if (!ActivateAppX(aumid, args, out int processId) && !registered)
            {
                WriteLogLine($"First launch attempt: {DateTime.Now - startTime}");

                // If we failed launching and haven't registered yet, try again with registring first.
                if (!RegisterAppX(appExecutable, archString, manifestPath, out aumid))
                    return new Tuple<LaunchResult, int>(LaunchResult.DeploymentFailed, -1);
                WriteLogLine($"Registered: {DateTime.Now - startTime}");

                if (!ActivateAppX(aumid, args, out processId))
                    return new Tuple<LaunchResult, int>(LaunchResult.DeploymentFailed, -1);
            }
            WriteLogLine($"Launched: {DateTime.Now - startTime}");

            LaunchResult launchResult = WaitForExit(processId, GetRemainingTimeout(startTime, timeoutMs), out int exitCode);
            ExtractResultFiles(appOutputFiles, forwardConsoleOutput, aumid, ref exitCode);

            // Recheck exitCode since we might have gotten a different one from the files.
            // (exit code returned from ActivateAppXAndWaitForExit may not be reliable since we may fail to capture it from the dying process)
            if (launchResult != LaunchResult.Timeout && launchResult != LaunchResult.DeploymentFailed)
                launchResult = exitCode == 0 ? LaunchResult.Success : LaunchResult.QuitWithError;

            return new Tuple<LaunchResult, int>(launchResult, exitCode);
        }
    }
}
