using System;
using System.Collections.Generic;
using System.Linq;
using Bee.Core;
using NiceIO;

namespace Bee.DotNet
{
    public abstract class DotNetRuntime
    {
        public abstract class DotNetRuntimeProvider
        {
            public abstract IEnumerable<DotNetRuntime> Provide();
        }

        public static DotNetRuntime[] AvailableRuntimes { get; } =
            DynamicLoader.FindAndCreateInstanceOfEachInAllAssemblies<DotNetRuntimeProvider>("*.dll")
                .SelectMany(p => p.Provide())
                .ToArray();

        public abstract IEnumerable<Framework> SupportedFrameworks { get; }
        public abstract IEnumerable<DebugFormat> DebugFormats { get; }

        public static DotNetRuntime FindFor(DotNetAssembly program)
        {
            return AvailableRuntimes.OrderByDescending(r => r.CanRunScoreFor(program)).FirstOrDefault();
        }

        private int CanRunScoreFor(DotNetAssembly program)
        {
            int score = 0;
            if (!SupportedFrameworks.Contains(program.Framework))
                return -100;
            if (DebugFormats.Contains(program.DebugFormat))
                score++;
            return score;
        }

        public abstract string InvocationStringFor(NPath programPath, bool debug = true);

        public string InvocationStringFor(DotNetAssembly program, bool debug = true) => InvocationStringFor(program.Path, debug);
    }
}
