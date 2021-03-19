using System.Collections.Generic;
using System.Linq;
using JamSharp.Runtime;
using NiceIO;
using Unity.BuildTools;
using static JamSharp.Runtime.BuiltinRules;

namespace Unity.BuildSystem.Common
{
    public class TgzCompressor : BuildSystemTool
    {
        public void SetupInvocation(JamList target, JamList inputFiles, NPath inputDirectory)
        {
            var tgzToolPath = "Tools/Build/TgzWrapper.pl";
            Depends(target, inputFiles);
            Depends(target, tgzToolPath);
            var commandLineArguments = new[]
            {
                $"{Paths.UnityRoot}/{tgzToolPath}",
                target.Single(),
                inputDirectory.ToString()
            }
            ;

            base.SetupInvocation(target, inputFiles, commandLineArguments, "perl");
        }

        static TgzCompressor s_Instance;
        public static TgzCompressor Instance() => s_Instance ?? (s_Instance = new TgzCompressor());
    }
}
