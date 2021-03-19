using Mono.Cecil;
using System.Collections.Generic;
using System.Reflection;

namespace Unity.RegistrationGenerator.Tests
{
    class MockCppCodeGeneration : Unity.RegistrationGenerator.ICppCodeGeneration
    {
        public List<CodegenProcessor> m_Processors = new List<CodegenProcessor>();
        public List<string> m_Includes = new List<string>();
        public List<TypeDefinition> m_ClassDecls = new List<TypeDefinition>();
        public List<TypeDefinition> m_StructDecls = new List<TypeDefinition>();
        public AssemblyDefinition m_Assembly;

        public MockCppCodeGeneration(AssemblyDefinition a)
        {
            m_Assembly = a;
        }

        public AssemblyDefinition GetAssembly()
        {
            return m_Assembly;
        }

        public void AddProcessor(CodegenProcessor p)
        {
            m_Processors.Add(p);
        }

        public void AddInclude(string path)
        {
            m_Includes.Add(path);
        }

        public void AddClassDeclaration(TypeDefinition type)
        {
            if (type != null)
                m_ClassDecls.Add(type);
        }

        public void AddStructDeclaration(TypeDefinition type)
        {
            if (type != null)
                m_StructDecls.Add(type);
        }
    }
}
