using System;
using System.Collections.Generic;
using System.Linq;
using Bee.Core;
using Bee.NativeProgramSupport.Core;
using Unity.BuildSystem.NativeProgramSupport;
using NiceIO;
using Unity.BuildTools;

namespace Bee.NativeProgramSupport
{
    public static class NativeProgramFormatUtils
    {
        public static string ExecutableStringFor(NPath script, SlashMode slashMode = SlashMode.Native)
        {
            return (HostPlatform.IsWindows ? "" : "sh ") + script.InQuotes(slashMode);
        }

        public static NPath SetupHostInvocationScriptFor(InvocationResult result)
        {
            return HostPlatform.IsWindows
                ? SetupWindowsInvocationScriptFor(result)
                : SetupPosixInvocationScriptFor(result);
        }

        public static NPath SetupPosixInvocationScriptFor(InvocationResult result)
        {
            var script = result.Result.ChangeExtension("script.sh");
            Backend.Current.AddWriteTextAction(script, @"
                command=$1; stdout=$2; stderr=$3; ecfile=$4; ignore_errors=$5; echo_result=$6
                shift 6

                chmod +x ""$command""
                ""$command"" ""$@"" 1>""$stdout"" 2>""$stderr""; ec=$?; echo $ec >""$ecfile""
                if [ $echo_result != 0 ]; then
                    cat ""$stdout"" >&1
                    cat ""$stderr"" >&2
                fi

                if [ $ignore_errors = 0 ]; then
                    exit $ec
                fi"
            );
            return script;
        }

        public static NPath SetupWindowsInvocationScriptFor(InvocationResult result)
        {
            var script = result.Result.ChangeExtension("script.cmd");
            Backend.Current.AddWriteTextAction(script, @"
                @echo off
                setlocal EnableDelayedExpansion

                set command=""%1""
                set stdout=""%2""
                set stderr=""%3""
                set ecfile=""%4""
                set ignore_errors=%5
                set echo_result=%6

                set skip=6
                for %%a in (%*) do (
                  if not !position! lss !skip! (
                    set args=!args! %%a
                  ) else (
                    set /a ""position=!position!+1""
                  )
                )

                call ""%command%"" %args% 1>""%stdout%"" 2>""%stderr%""
                set ec=%errorlevel%
                echo %ec% > ""%ecfile%""
                if not %echo_result% == 0 (
                    type ""%stdout%""
                    type ""%stderr%"" 1>&2
                )
                if %ignore_errors == 0 (
                    exit %ec%
                )
                endlocal
                "
            );
            return script;
        }

        public static string[] GetInvocationScriptArgumentsFor(
            NPath program,
            InvocationResult result,
            bool enableConsoleOutput,
            bool ignoreProgramExitCode,
            IEnumerable<string> args = null)
        {
            return new[]
            {
                program.MakeAbsolute().InQuotes(SlashMode.Native),
                result.Stdout.InQuotes(SlashMode.Native),
                result.Stderr.InQuotes(SlashMode.Native),
                result.Result.InQuotes(SlashMode.Native),
                ignoreProgramExitCode ? "1" : "0",
                enableConsoleOutput ? "1" : "0"
            }.Concat(args ?? Array.Empty<string>()).ToArray();
        }
    }
}
