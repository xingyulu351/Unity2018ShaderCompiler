using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.CompilerServices;
using Bee.Core;
using Bee.DotNet;
using NiceIO;
using Unity.BuildSystem.CSharpSupport;
using Unity.BuildTools;

namespace Bee.CSharpSupport.Unity
{
    public class UnityVersionedMcs : Mcs
    {
        public UnityVersionedMcs() : base(new NativeRunnableProgram(Paths.MonoBleedingEdgeCLI.Parent.Combine("mcs")))
        {
        }

        public override string ActionName { get; } = "UnityMcs";
        public override int PreferredUseScore => 2;
        public override bool SupportedOnHostPlatform() => true;
        public override bool CanBuild() => true;

        public override Func<CSharpCompiler> StaticFunctionToCreateMe { get; } = () => new UnityVersionedMcs();
    }

    public class UnityVersionedCsc : Csc
    {
        public UnityVersionedCsc()
        {
            var binariesPath = Paths.UnityRoot.Combine("External/Roslyn/csc/builds/Binaries");
            if (HostPlatform.IsLinux)
                CompilerProgram = new DotNetRunnableProgram(new DotNetAssembly($"{binariesPath}/NET46/csc.exe", Framework.Framework46, DebugFormat.Pdb));
            if (HostPlatform.IsOSX)
                CompilerProgram = new NativeRunnableProgram($"{binariesPath}/Mac/csc");
            if (HostPlatform.IsWindows)
                CompilerProgram = new NativeRunnableProgram($"{binariesPath}/Windows/csc.exe");
        }

        protected override RunnableProgram CompilerProgram { get; }

        public override string ActionName { get; } = "UnityCsc";
        public override int PreferredUseScore => 2;
        public override bool SupportedOnHostPlatform() => HostPlatform.IsWindows;
        public override bool CanBuild() => SupportedOnHostPlatform();

        public override Func<CSharpCompiler> StaticFunctionToCreateMe { get; } = () => new UnityVersionedCsc();
    }

    public class MonoBleedingEdgeCsc : Csc
    {
        public MonoBleedingEdgeCsc()
        {
            var mono = MonoBleedingEdgeRuntimeProvider.Mono();

            CompilerProgram = new ProgramRanUsingMono(
                Paths.UnityRoot.Combine("External/MonoBleedingEdge/builds/monodistribution/lib/mono/4.5/csc.exe"), mono,
                //the mono guys tweaked the nursery like this in /Library/Frameworks/Mono.framework/Versions/Current/Commands/csc
                "--gc-params=nursery-size=64m");
        }

        protected override RunnableProgram CompilerProgram { get; }

        public override string ActionName { get; } = "MonoCsc";
        public override int PreferredUseScore => - 100;

        //on many windows boxes, this compiler errors with: error CS0041: Unexpected error writing debug information -- 'The version of Windows PDB writer is older than required: 'diasymreader.dll''
        public override bool SupportedOnHostPlatform() => !HostPlatform.IsWindows;

        public override bool CanBuild() => SupportedOnHostPlatform();

        public override Func<CSharpCompiler> StaticFunctionToCreateMe { get; } = () => new MonoBleedingEdgeCsc();

        class ProgramRanUsingMono : RunnableProgram
        {
            private readonly string _userMonoOptions;

            public ProgramRanUsingMono(NPath path, Mono mono, string userMonoOptions = null)
            {
                _userMonoOptions = userMonoOptions ?? string.Empty;
                Path = path;
                Mono = mono;
            }

            public Mono Mono { get; }
            public override string InvocationString => $"{Mono.PathToMonoBinary} {MonoOptions} {Path}";
            public object MonoOptions => $"--debug {_userMonoOptions}";
            public override NPath Path { get; }
        }
    }
}
