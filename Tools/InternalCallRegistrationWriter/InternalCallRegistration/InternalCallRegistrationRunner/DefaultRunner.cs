using System;
using System.Collections.Generic;
using System.Linq;
using Mono.Cecil;
using Unity.InternalCallRegistrationWriter.Collector;
using Unity.InternalCallRegistrationWriter.CppWriter;

namespace Unity.InternalCallRegistrationWriter.Runner
{
    public class DefaultRunner
    {
        public static void ParseOptionsAndRun(string[] args)
        {
            var options = InternalCallOptions.ParseOptionsFrom(args);
            Run(options);
        }

        private static void Run(InternalCallOptions options)
        {
            var callsByType = new SortedDictionary<string, List<MethodDefinition>>();
            foreach (var assembly in options.InputAssemblies)
            {
                var callsByTypeAssembly = new InternalCallCollector(assembly).InternalCallsByType;
                foreach (var entry in callsByTypeAssembly)
                    callsByType.Add(entry.Key, entry.Value);
            }
            var codeWriter = new CodeWriter();

            new UnityInternalCallRegistrationsWriter(codeWriter).FullRegistrationCpp(callsByType);

            var outputCppString = codeWriter.Output();
            System.IO.File.WriteAllText(options.OutputFile, outputCppString);

            if (options.OutputSummary != null)
            {
                codeWriter = new CodeWriter();
                new UnityInternalCallRegistrationsWriter(codeWriter).SummaryFile(callsByType);

                outputCppString = codeWriter.Output();
                System.IO.File.WriteAllText(options.OutputSummary, outputCppString);
            }
        }
    }
}
