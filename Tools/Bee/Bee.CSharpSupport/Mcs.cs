using System;
using System.Collections.Generic;
using System.IO;
using Bee.Core;
using Bee.DotNet;
using NiceIO;
using Unity.BuildTools;

namespace Unity.BuildSystem.CSharpSupport
{
    public class SystemMcs : Mcs
    {
        public SystemMcs() : base(new NativeRunnableProgram(FindMcsPath()))
        {
        }

        private static NPath FindMcsPath()
        {
            if (HostPlatform.IsOSX)
            {
                var candidate = new NPath("/Library/Frameworks/Mono.framework/Versions/Current/Commands/mcs");
                if (candidate.FileExists())
                    return candidate;
            }

            return null;
        }

        public override int PreferredUseScore => 0;


        public override bool SupportedOnHostPlatform() => true;

        public override bool CanBuild()
        {
            return FindMcsPath() != null;
        }

        public override Func<CSharpCompiler> StaticFunctionToCreateMe { get; } = () => new SystemMcs();
    }

    public abstract class Mcs : CSharpCompiler
    {
        protected override RunnableProgram CompilerProgram { get; }

        protected Mcs(RunnableProgram mcsProgram)
        {
            CompilerProgram = mcsProgram;
        }

        protected override void AddCompilerSpecificArguments(List<string> arguments, List<string> nonRspArgs)
        {
            arguments.Add("-debug");
        }

        protected override NPath PathOfGeneratedSymbolsFor(NPath targetFile)
        {
            return targetFile + ".mdb";
        }

        public override DebugFormat DebugSymbolFormat { get; } = DebugFormat.Mdb;

        public override bool SupportedOnHostPlatform() => true;
        public override bool CanBuild() => true;
    }
}
