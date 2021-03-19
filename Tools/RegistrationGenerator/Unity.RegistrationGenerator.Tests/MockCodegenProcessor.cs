using Mono.Cecil;
using System.Collections.Generic;
using System.Text;

namespace Unity.RegistrationGenerator.Tests
{
    class MockCodegenProcessor : CodegenProcessor
    {
        public string m_MockData = "{1,2,3,4}";
        public List<string> m_Includes = new List<string>();
        public List<TypeDefinition> m_ClassDecls = new List<TypeDefinition>();
        public List<TypeDefinition> m_StructDecls = new List<TypeDefinition>();

        internal override void Process(ICppCodeGeneration generator)
        {
            foreach (var include in m_Includes)
                generator.AddInclude(include);
            foreach (var decl in m_ClassDecls)
                generator.AddClassDeclaration(decl);
            foreach (var decl in m_StructDecls)
                generator.AddStructDeclaration(decl);
        }

        internal override int CodegenRegistrationData(Dictionary<TypeDefinition, NativeName> types, StringBuilder output)
        {
            if (m_MockData == null)
                return 0;

            output.Append("    ");
            output.Append(m_MockData);
            output.Append("\n");
            return 1;
        }

        internal override bool HasData()
        {
            return true;
        }
    }
}
