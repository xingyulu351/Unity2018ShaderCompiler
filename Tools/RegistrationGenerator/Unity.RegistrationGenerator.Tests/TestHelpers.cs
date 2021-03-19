using Mono.Cecil;
using System.IO;

namespace Unity.RegistrationGenerator.Tests
{
    static class TestHelpers
    {
        internal static AssemblyDefinition LoadExecutingAssembly()
        {
            var assemblyResolver = new DefaultAssemblyResolver();
            var readerParameters = new ReaderParameters()
            {
                AssemblyResolver = assemblyResolver
            };

            var executingAssemblyLocation = System.Reflection.Assembly.GetExecutingAssembly().Location;
            assemblyResolver.AddSearchDirectory(Path.GetDirectoryName(executingAssemblyLocation));

            return AssemblyDefinition.ReadAssembly(executingAssemblyLocation, readerParameters);
        }
    }
}
