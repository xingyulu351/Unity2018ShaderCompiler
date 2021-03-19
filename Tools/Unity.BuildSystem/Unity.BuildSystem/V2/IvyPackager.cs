using Bee.Core;
using Bee.CSharpSupport;
using Bee.DotNet;
using NiceIO;
using Projects.Jam;
using Unity.BuildSystem.CSharpSupport;

namespace Unity.BuildSystem.V2
{
    class IvyPackager : CSharpTool
    {
        private static IvyPackager _instance;
        public static IvyPackager Instance() => _instance ?? (_instance = new IvyPackager());

        public static CSharpProgram DataContract { get; }

        static IvyPackager()
        {
            DataContract = new CSharpProgram
            {
                Path = "artifacts/PackageManager/Unity.DataContract.dll",
                Sources = { "Tools/PackageManager/DataContract" }
            };

            IvyParser = new CSharpProgram
            {
                Path = "artifacts/PackageManager/Unity.IvyParser.dll",
                Sources = { "Tools/PackageManager/IvyParser" },
                References = { DataContract }
            };
        }

        static CSharpProgram IvyParser { get; }

        private IvyPackager()
        {
        }

        protected override DotNetAssembly DotNetAssembly()
        {
            var mp = new CSharpProgram
            {
                Path = "artifacts/PackageManager/Packager.exe",
                Sources = { "Tools/PackageManager/Packager" },
                References = { DataContract, IvyParser },
                PrebuiltReferences = { "External/Mono/builds/monodistribution/lib/mono/2.0/ICSharpCode.SharpZipLib.dll", "External/Mono/builds/monodistribution/lib/mono/2.0/Mono.Posix.dll" },
            };
            ProjectFilesTarget.DependOn(new SolutionFileBuilder().WithProgram(mp).Complete().Path);
            return mp.SetupDefault();
        }

        public NPath SetupInvocation(NPath targetIvyFile, NPath templateIvyFile)
        {
            var commandLineArguments = new[]
            {
                "--ivy",
                "-i",
                templateIvyFile.InQuotes(),
                "-o", targetIvyFile.InQuotes(),
                $"-v {UnityVersion.Version}",
                $"-u {UnityVersion.Exact}",
            };
            base.SetupInvocation(targetIvyFile, templateIvyFile, commandLineArguments);
            return targetIvyFile;
        }
    }
}
