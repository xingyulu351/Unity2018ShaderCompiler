using System;
using Bee.NativeProgramSupport;
using NiceIO;
using Unity.BuildSystem.NativeProgramSupport;
using Unity.BuildTools;

namespace Bee.Toolchain.Linux
{
    public class WSLSdk : Sdk
    {
        public override Version Version => NativeSdk.Version;
        public override Platform Platform => NativeSdk.Platform;
        public override NPath Path => NativeSdk.Path;

        public override bool SupportedOnHostPlatform => HostPlatform.IsWindows;

        public override NPath CppCompilerPath => NativeSdk.CppCompilerPath;
        public override NPath StaticLinkerPath => NativeSdk.StaticLinkerPath;
        public override NPath DynamicLinkerPath => NativeSdk.DynamicLinkerPath;

        public override string CppCompilerCommand => ProxyScriptFile.InQuotes(SlashMode.Native) + " " + NativeSdk.CppCompilerCommand;
        public override string StaticLinkerCommand => ProxyScriptFile.InQuotes(SlashMode.Native) + " " + NativeSdk.StaticLinkerCommand;
        public override string DynamicLinkerCommand => ProxyScriptFile.InQuotes(SlashMode.Native) + " " + NativeSdk.DynamicLinkerCommand;

        // Workaround for WSL stdout/stderr bug (Error : 0x80070057)
        private const string WslProxyScriptVersion = "5";
        private const string WslProxyScript = @"@echo off
                setlocal enabledelayedexpansion

                rem -------------------------------------------------------------------
                rem Allocate temp files for stdout and stderr in proxy script directory
                rem -------------------------------------------------------------------
                :getTemp
                set ""tempFile=%~dp0%time::=%""
                set ""tempLockFile=%tempFile%.lock""
                set ""tempOutFile=%tempFile%.out""
                set ""tempErrFile=%tempFile%.err""
                9>&2 2>nul (2>&9 8>""%tempLockFile%"" call :invoke_wsl %*) || goto :getTemp
                del /q ""%tempLockFile%"" ""%tempOutFile%"" ""%tempErrFile%""
                exit %errorlevel%

                rem ------------------------------------------
                rem Figure out where wsl is located
                rem ------------------------------------------
                :invoke_wsl
                if exist C:\Windows\Sysnative\wsl.exe (
                    set WSL=C:\Windows\Sysnative\wsl.exe
                ) else (
                    set WSL=C:\Windows\System32\wsl.exe
                )

                rem ------------------------------------------
                rem Fix slashes and drive prefix
                rem ------------------------------------------
                set linuxCmdLine=%*
                set linuxCmdLine=!linuxCmdLine:\=/!
                set linuxCmdLine=!linuxCmdLine:C^:=/mnt/c!
                set linuxCmdLine=!linuxCmdLine:D^:=/mnt/d!
                set linuxCmdLine=!linuxCmdLine:E^:=/mnt/e!
                set linuxCmdLine=!linuxCmdLine:F^:=/mnt/f!

                rem ------------------------------------------
                rem Invoke WSL and pipe results to out and err
                rem ------------------------------------------
                start /min /I /w cmd /c %WSL% !linuxCmdLine! ^1^>""%tempOutFile%"" ^2^>""%tempErrFile%""
                set startResult=%errorlevel%
                type ""%tempOutFile%""
                type ""%tempErrFile%"" 1>&2
                exit %startResult%
            ";

        public static NPath GetOrCreateProxyScriptFile()
        {
            var proxyScriptFile = NPath.SystemTemp.Combine("Bee-WSL/WSLProxy" + WslProxyScriptVersion + ".bat");
            if (!proxyScriptFile.Exists())
                proxyScriptFile.WriteAllText(WslProxyScript);
            return proxyScriptFile;
        }

        internal WSLSdk(Sdk sdk)
        {
            NativeSdk = sdk;
            ProxyScriptFile = GetOrCreateProxyScriptFile();
        }

        private Sdk NativeSdk { get; }
        internal NPath ProxyScriptFile { get; }
    }
}
