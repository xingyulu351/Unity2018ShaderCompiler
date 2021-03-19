using System;
using System.Collections.Generic;
using Bee.NativeProgramSupport.Running;
using JamSharp.Runtime;
using NiceIO;
using Unity.BuildSystem.NativeProgramSupport;
using Unity.BuildTools;

namespace Bee.Toolchain.Android
{
    public static class ADB
    {
        public static NPath Path => AndroidSdk.Path?.Combine("platform-tools/adb" + HostPlatform.Exe);

        public static Shell.ExecuteResult Execute(string arguments, Shell.IExecuteController executeController = null, int timeoutMs = -1)
        {
            return Shell.Execute(new Shell.ExecuteArgs() { Executable = Path.MakeAbsolute().ToString(), Arguments = arguments }, executeController, timeoutMs);
        }

        public static Shell.ExecuteResult Execute(string arguments, string deviceId, Shell.IExecuteController executeController = null, int timeoutMs = -1)
        {
            return Execute($"-s {deviceId} {arguments}", executeController, timeoutMs);
        }

        public static Architecture GetArchitecture(string abi)
        {
            switch (abi.ToLowerInvariant())
            {
                case "armeabi-v7a":
                    return Architecture.Armv7;
                case "arm64-v8a":
                    return Architecture.Arm64;
                case "x86":
                    return Architecture.x86;
                default:
                    return null;
            }
        }

        public static IEnumerable<Architecture> QuerySupportedArchitectures(string deviceId)
        {
            var result = Execute("shell getprop ro.product.cpu.abilist", deviceId);
            if (result.ExitCode != 0)
                throw new Exception($"Failed to query abilist from android device {deviceId}. Adb output:\n{result}");

            // Older devices don't have the abilist property
            if (string.IsNullOrWhiteSpace(result.StdOut))
            {
                result = Execute("shell getprop ro.product.cpu.abi", deviceId);
                if (result.ExitCode != 0)
                    throw new Exception($"Failed to query abilist from android device {deviceId}. Adb output:\n{result}");
            }

            foreach (var abi in result.StdOut.Split(','))
            {
                var arch = GetArchitecture(abi.Trim());
                if (arch != null)
                    yield return arch;
            }
        }

        public static string QueryDevices()
        {
            var result = Execute("devices");
            if (result.ExitCode != 0)
                throw new Exception($"\"adb devices\" failed with error code {result.ExitCode}.\nStdout:\n{result.StdOut}\nStderr:\n{result.StdErr}");
            return result.StdOut;
        }

        public static IEnumerable<Tuple<string, RunnerEnvironment.EnvironmentStatus>> ParseQueryDeviceString(string queryDevicesResult)
        {
            if (string.IsNullOrEmpty(queryDevicesResult))
                yield break;

            foreach (var line in queryDevicesResult.Split('\r', '\n'))
            {
                var trimmedLine = line.Trim();

                if (trimmedLine.EndsWith("device"))
                    yield return new Tuple<string, RunnerEnvironment.EnvironmentStatus>(line.Substring(0, trimmedLine.IndexOf('\t')), RunnerEnvironment.EnvironmentStatus.FullAccess);
                else if (trimmedLine.EndsWith("unauthorized") || trimmedLine.EndsWith("offline")) // unauthorized means that we can't query data, so for all practical purposes it is offline for us.
                    yield return new Tuple<string, RunnerEnvironment.EnvironmentStatus>(line.Substring(0, trimmedLine.IndexOf('\t')), RunnerEnvironment.EnvironmentStatus.Offline);
            }
        }
    }
}
