using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Globalization;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading;
using Bee.NativeProgramSupport.Running;
using JamSharp.Runtime;
using Unity.BuildSystem.NativeProgramSupport;
using Unity.BuildTools;

namespace Bee.Toolchain.Android
{
    [RunnerEnvironmentProviderVersion(1)]
    internal class AndroidRunnerEnvironmentProvider : RunnerEnvironmentProviderImplementation<AndroidRunnerEnvironment>
    {
        class KnownAndroidDevice
        {
            public AndroidRunnerEnvironment CreateEnvironment(string deviceId, RunnerEnvironment.EnvironmentStatus status) => new AndroidRunnerEnvironment(this.SupportedArchitectures, deviceId) { Status = status};

            public Architecture[] SupportedArchitectures = Array.Empty<Architecture>();
            public bool Stale = false;
        }

        private const int QueryADBIntervalMs = 4000;
        private static readonly Version ADBMinimumTrackDevicesVersion = new Version(1, 0, 39);
        private const string ADBTrackDevicesArgs = "track-devices BeeProgramRunnerIdentifier"; // 'BeeProgramRunnerIdentifier' is ignored by ADB but helps us recognizing stale ADB processes.
        private readonly Dictionary<string, KnownAndroidDevice> knownDevices = new Dictionary<string, KnownAndroidDevice>();


        private IEnumerable<AndroidRunnerEnvironment> ReportEnvironments(string queryDevicesString)
        {
            foreach (var device in knownDevices.Values)
                device.Stale = true;

            foreach (var device in ADB.ParseQueryDeviceString(queryDevicesString))
            {
                if (knownDevices.TryGetValue(device.Item1, out KnownAndroidDevice knownDevice))
                    knownDevice.Stale = false;
                else
                {
                    knownDevice = new KnownAndroidDevice();
                    knownDevices.Add(device.Item1, knownDevice);
                }

                if (knownDevice.SupportedArchitectures.Empty()) // Can also happen with old devices if we started out in the offline state.
                    knownDevice.SupportedArchitectures = ADB.QuerySupportedArchitectures(device.Item1).CatchExceptions().ToArray();

                yield return knownDevice.CreateEnvironment(device.Item1, device.Item2);
            }

            foreach (var knownDevice in knownDevices.Where(kv => kv.Value.Stale))
                yield return knownDevice.Value.CreateEnvironment(knownDevice.Key, RunnerEnvironment.EnvironmentStatus.Offline);
        }

        public override IEnumerable<AndroidRunnerEnvironment> Provide()
        {
            if (ADB.Path == null || !ADB.Path.Exists())
                yield break;
            foreach (var env in ReportEnvironments(ADB.QueryDevices()))
                yield return env;
        }

        private void ReadBytesFromAdbOutputStream(char[] bufferToFill, StreamReader stream)
        {
            for (int bufferPos = 0; bufferPos < bufferToFill.Length; ++bufferPos)
            {
                stream.ReadBlock(bufferToFill, bufferPos, 1);

                // On windows adb sends \r\n but doesn't specify this in the number of expected characters.
                // (which is why we have this helper function in the first place)
                if (bufferToFill[bufferPos] == '\r')
                    bufferPos--;
            }
        }

        private void ReadTrackDevicesOutputThread(Process process, AutoResetEvent processDevicesEvent, StringBuilder rawDeviceList)
        {
            var stdout = process.StandardOutput;
            char[] numCharacteresExpectedRaw = new char[4];
            while (!process.HasExited)
            {
                ReadBytesFromAdbOutputStream(numCharacteresExpectedRaw, stdout);
                int numCharacteresExpected;
                if (!int.TryParse(new string(numCharacteresExpectedRaw), NumberStyles.HexNumber, CultureInfo.InvariantCulture, out numCharacteresExpected))
                    break;

                if (numCharacteresExpected != 0)
                {
                    char[] rawDeviceBuffer = new char[numCharacteresExpected];
                    ReadBytesFromAdbOutputStream(rawDeviceBuffer, stdout);
                    lock (rawDeviceList)
                        rawDeviceList.Append(rawDeviceBuffer);
                }

                processDevicesEvent.Set();
            }

            try
            {
                process.Kill();
                process.WaitForExit();
            }
            catch {}
            processDevicesEvent.Set();
        }

        public override IEnumerable<AndroidRunnerEnvironment> UpdateStream()
        {
            if (ADB.Path == null || !ADB.Path.Exists())
                yield break;

            var adbVersionLines = ADB.Execute("version").StdOut.Split('\n');
            var adbVersion = new Version(adbVersionLines[0].Substring("Android Debug Bridge version ".Length));
            if (adbVersion >= ADBMinimumTrackDevicesVersion)
            {
                // adb track-devices does not die with the ProgramRunner Daemon. So we try to kill the last one if there is any one still floating around.
                Func<int, string> getProcessArgsCommandLine;
                if (HostPlatform.IsWindows)
                    getProcessArgsCommandLine = pid => $"wmic.exe path Win32_Process where handle='{pid}' get commandline";
                else
                    getProcessArgsCommandLine = pid => $"ps -fp {pid}";
                Process.GetProcessesByName("adb")
                    .Select(p => Tuple.Create(p, Shell.Execute(getProcessArgsCommandLine(p.Id))))
                    .Where(pt => pt.Item2.StdOut.Trim().EndsWith(ADBTrackDevicesArgs))
                    .CatchExceptions()
                    .ForEach(pt => pt.Item1.Kill());

                using (var processDevicesEvent = new AutoResetEvent(false))
                {
                    using (var adbTrackDevicesProcess = Shell.NewProcess(new Shell.ExecuteArgs() {Executable = ADB.Path.ToString(), Arguments = ADBTrackDevicesArgs}))
                    {
                        adbTrackDevicesProcess.Exited += (o, a) => processDevicesEvent.Set();
                        adbTrackDevicesProcess.EnableRaisingEvents = true;

                        // Can't do a line by line read (BeginOutputReadLine) since in case of 0 devices we just get '0000' without any newlines.
                        // (every distinct device is always followed by a newline though)
                        var rawDeviceList = new StringBuilder();
                        var readOutputThread = new Thread(() => ReadTrackDevicesOutputThread(adbTrackDevicesProcess, processDevicesEvent, rawDeviceList));

                        adbTrackDevicesProcess.Start();
                        readOutputThread.Start();

                        while (processDevicesEvent.WaitOne() && !adbTrackDevicesProcess.HasExited)
                        {
                            string rawDeviceString;
                            lock (rawDeviceList)
                            {
                                rawDeviceString = rawDeviceList.ToString();
                                rawDeviceList.Clear();
                            }

                            foreach (var env in ReportEnvironments(rawDeviceString))
                                yield return env;
                        }

                        readOutputThread.Join();

                        if (adbTrackDevicesProcess.ExitCode != 0)
                            throw new Exception($"\"adb track-devices\" failed with error code {adbTrackDevicesProcess.ExitCode}");
                    }
                }
            }
            // 'adb track-devices' is only available in newer adb versions. Fall back to periodic polling if we can't use it.
            else
            {
                while (true)
                {
                    Thread.Sleep(QueryADBIntervalMs);

                    if (ADB.Path == null || !ADB.Path.Exists())
                        yield break;

                    foreach (var env in ReportEnvironments(ADB.QueryDevices()).CatchExceptions())
                        yield return env;
                }
            }
        }
    }
}
