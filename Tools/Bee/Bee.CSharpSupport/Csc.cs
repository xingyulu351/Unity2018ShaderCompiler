using Bee.Core;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Linq.Expressions;
using Bee.DotNet;
using NiceIO;
using Unity.BuildTools;

namespace Unity.BuildSystem.CSharpSupport
{
    public class SystemCsc : Csc
    {
        public SystemCsc() : base(new NativeRunnableProgram(FindCscPath()))
        {
        }

        private static NPath FindCscPath()
        {
            if (HostPlatform.IsOSX)
            {
                NPath candidate = "/Library/Frameworks/Mono.framework/Versions/Current/Commands/csc";
                if (candidate.FileExists())
                    return candidate;
            }

            if (HostPlatform.IsWindows)
            {
                NPath candidate = @"C:/Program Files (x86)/Microsoft Visual Studio/2017/Professional/MSBuild/15.0/Bin/Roslyn/csc.exe";
                if (candidate.FileExists())
                    return candidate;
            }

            return null;
        }

        public override bool SupportedOnHostPlatform() => true;

        public override bool CanBuild() => FindCscPath() != null;
        public override int PreferredUseScore => 0;
        public override Func<CSharpCompiler> StaticFunctionToCreateMe { get; } = () => new SystemCsc();
    }

    public abstract class Csc : CSharpCompiler
    {
        protected Csc(RunnableProgram cscProgram = null)
        {
            CompilerProgram = cscProgram;
        }

        protected override RunnableProgram CompilerProgram { get; }

        protected override void AddCompilerSpecificArguments(List<string> arguments, List<string> nonRspArgs)
        {
            arguments.Add("/debug:pdbonly");
            arguments.Add("/optimize+");
            arguments.Add("/nologo");
            arguments.Add("/deterministic");
        }

        protected override NPath PathOfGeneratedSymbolsFor(NPath targetFile) => targetFile.ChangeExtension("pdb");

        public override DebugFormat DebugSymbolFormat { get; } = DebugFormat.PortablePdb;

        public override bool SupportsRefOutAssembly => true;

        protected override string ArgumentForRefOutAssembly(NPath refOutAssembly) => $"/refout:{refOutAssembly.InQuotes(SlashMode.Native)}";

        public override bool SupportedOnHostPlatform() => false;
    }
}
