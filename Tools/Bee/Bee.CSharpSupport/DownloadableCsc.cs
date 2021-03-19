using System;
using Bee.Core;
using Bee.Stevedore;
using NiceIO;
using Unity.BuildSystem.CSharpSupport;
using Unity.BuildTools;

namespace Bee.CSharpSupport
{
    public static class DownloadableCsc
    {
        public static Csc Csc72 { get; } = new DownloadedCsc72();
    }

    class DownloadedCsc72 : DownloadedCsc
    {
        public override string ActionName { get; } = "Csc";

        public DownloadedCsc72() : base(
            "roslyn-csc-linux/6c0e221e95df_00fbe3124ee11391fab9fc30907daab0141d5e299a68683f652fb25fa6543b12.7z",
            "roslyn-csc-mac/6c0e221e95df_7101670cd0a4eee0627ccea84b4976117127409c3fb065f79d6ac7bbf8631774.7z",
            "roslyn-csc-win64/6c0e221e95df_ae56073e3848e11859fa7ba9219c356153b8c416f44f335d2a8972fef099e0f1.7z")
        {
        }
    }

    class DownloadedCsc : Csc
    {
        private RunnableProgram _compilerProgram;
        private string LinuxArtifact { get; }
        private string WindowsArtifact { get; }
        private string OsxArtifact { get; }

        public DownloadedCsc(string linuxArtifact, string osxArtifact, string windowsArtifact)
        {
            if (!CanBuild())
                return;
            LinuxArtifact = linuxArtifact;
            OsxArtifact = osxArtifact;
            WindowsArtifact = windowsArtifact;
        }

        private string RoslynArtifact
        {
            get
            {
                if (HostPlatform.IsLinux)
                    return LinuxArtifact;
                if (HostPlatform.IsWindows)
                    return WindowsArtifact;
                if (HostPlatform.IsOSX)
                    return OsxArtifact;
                throw new Exception("Unkown platform");
            }
        }

        protected override RunnableProgram CompilerProgram
        {
            get
            {
                if (_compilerProgram == null)
                {
                    var artifact = StevedoreArtifact.Public(RoslynArtifact);
                    Backend.Current.Register(artifact);
                    _compilerProgram = new NativeRunnableProgram(artifact.Path.Combine("csc" + HostPlatform.Exe));
                }

                return _compilerProgram;
            }
        }

        public override int PreferredUseScore { get; } = 1;
        public override bool CanBuild() => !(Backend.Current is IJamBackend);
    }
}
