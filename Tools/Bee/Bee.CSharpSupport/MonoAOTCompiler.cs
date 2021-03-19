using System;
using Bee.Core;
using NiceIO;

namespace Unity.BuildSystem.CSharpSupport
{
    public class MonoAOTCompiler
    {
        private MonoAOTCompiler()
        {
        }

        public NPath SetupInvocation(NPath targetDylib, NPath inputAssembly)
        {
            if (targetDylib != $"{inputAssembly}.dylib")
                throw new ArgumentException($"{nameof(targetDylib)} has to be the input assembly with a dylib extension");

            Backend.Current.AddAction(
                typeof(MonoAOTCompiler).Name,
                new[] {targetDylib},
                new[] {inputAssembly},
                "External/MonoBleedingEdge/builds/monodistribution/bin/mono",
                new[] {"--aot", inputAssembly.InQuotes()}
            );

            return targetDylib;
        }

        static MonoAOTCompiler _instance;
        public static MonoAOTCompiler Instance() => _instance ?? (_instance = new MonoAOTCompiler());
    }
}
