using System.IO;
using Bee.Core;
using NiceIO;

namespace Bee.DotNet
{
    public class DotNetRunnableProgram : RunnableProgram
    {
        public DotNetRunnableProgram(DotNetAssembly program, DotNetRuntime dotNetRuntime = null)
        {
            Program = program;
            DotNetRuntime = dotNetRuntime ?? DotNet.DotNetRuntime.FindFor(program);
        }

        private DotNetAssembly Program { get; }
        private DotNetRuntime DotNetRuntime { get; }
        public override NPath Path => Program.Path;
        public override string InvocationString => DotNetRuntime?.InvocationStringFor(Program, true) ?? "no_dotnetruntime_found";
    }
}
