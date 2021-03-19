using System.Collections.Generic;
using System.Linq;
using Mono.Cecil;

namespace Unity.InternalCallRegistrationWriter.CppWriter
{
    public class UnityInternalCallRegistrationsWriter
    {
        private readonly IOutputWriter _outputWriter;
        public IOutputWriter OutputWriter
        {
            get { return _outputWriter; }
        }

        public UnityInternalCallRegistrationsWriter(IOutputWriter outputWriter)
        {
            _outputWriter = outputWriter;
        }

        public void FullRegistrationCpp(SortedDictionary<string, List<MethodDefinition>> allTypes)
        {
            WriteHeader();
            AllRegistrationsForAssembly(allTypes);
            WriteFooter();
        }

        public void SummaryFile(SortedDictionary<string, List<MethodDefinition>> allTypes)
        {
            foreach (var kvp in allTypes)
            {
                foreach (var methodDefinition in kvp.Value)
                    _outputWriter.AppendLine(kvp.Key + "::" + methodDefinition.Name);
            }
        }

        private void WriteFooter()
        {
            _outputWriter.EndBlock();
        }

        private void WriteHeader()
        {
            WriteStrippedRegistrationFunction();
        }

        private void WriteStrippedRegistrationFunction()
        {
            _outputWriter.Append("void RegisterAllStrippedInternalCalls()");
            _outputWriter.BeginBlock();
            _outputWriter.AppendLine();
        }

        public void AllRegistrationsForAssembly(SortedDictionary<string, List<MethodDefinition>> allTypes)
        {
            foreach (var kvp in allTypes)
            {
                MarkerForType(kvp.Key, "Start");
                OutputWriter.AppendLine();
                OutputWriter.Indent();
                AllMethodDeclarationsAndInvocationsForType(kvp.Value);
                OutputWriter.Dedent();
                MarkerForType(kvp.Key, "End");
                OutputWriter.AppendLine();
            }
        }

        private void MarkerForType(string typeFullName, string marker)
        {
            OutputWriter.AppendComment(string.Format("{1} Registrations for type : {0}", typeFullName, marker));
        }

        public void AllMethodDeclarationsAndInvocationsForType(List<MethodDefinition> iCallMethodDefinitions)
        {
            foreach (var methodDefinition in iCallMethodDefinitions)
                MethodDeclarationAndInvocationFor(methodDefinition);
        }

        public void MethodDeclarationAndInvocationFor(MethodDefinition methodDefinition)
        {
            _outputWriter.AppendComment(methodDefinition.FullName);
            _outputWriter.AppendLine(SingleMethodRegistrationDeclarationFor(methodDefinition));
            _outputWriter.AppendLine(SingleMethodRegistrationInvokeFor(methodDefinition));
            _outputWriter.AppendLine();
        }

        private static string GetNamespace(TypeDefinition typeDefinition)
        {
            if (typeDefinition == null)
                return string.Empty;

            if (!typeDefinition.IsNested)
                return typeDefinition.Namespace;

            return GetNamespace(typeDefinition.DeclaringType);
        }

        private static string GetTypeName(TypeDefinition typeDefinition)
        {
            if (typeDefinition == null)
                return string.Empty;

            if (typeDefinition.IsNested)
                return GetTypeName(typeDefinition.DeclaringType) + "_" + typeDefinition.Name;

            return typeDefinition.Name;
        }

        private static string SingleMethodRegistrationInvokeFor(MethodDefinition methodDefinition)
        {
            //Should match what is done in cspreprocess
            var @namespace = GetNamespace(methodDefinition.DeclaringType).Replace(".", "_");
            var typeName = GetTypeName(methodDefinition.DeclaringType);
            return string.Format("Register_{0}_{1}_{2}();", @namespace, typeName, methodDefinition.Name.Replace(".", "_"));
        }

        private static string SingleMethodRegistrationDeclarationFor(MethodDefinition methodDefinition)
        {
            //Should match what is done in cspreprocess
            return string.Format("void {0}", SingleMethodRegistrationInvokeFor(methodDefinition));
        }
    }
}
