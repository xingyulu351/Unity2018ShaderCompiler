using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using Unity.BindingsGenerator.Core.Nodes;

namespace Unity.BindingsGenerator.Core
{
    public class RegistrationGenerator
    {
        public string Generate(FileNode file, string inputFilePath)
        {
            var sb = new StringBuilder();

            GenerateRegisterInternalCallMacro(sb);
            GenerateRegistrationFunctions(file, sb);
            GenerateExportFunctions(sb, file, inputFilePath);

            return sb.ToString();
        }

        private static void GenerateRegistrationFunctions(FileNode file, StringBuilder sb)
        {
            foreach (var functionNode in file.DefaultNamespace.Functions)
            {
                sb.AppendLine($"void Register_{NormalizeManagedNameOf(functionNode)}() {{");
                sb.AppendLine($@"    REGISTER_INTERNAL_CALL(""{functionNode.ManagedName}"", {functionNode.Name});");
                sb.AppendLine("}\n");
            }
        }

        private static void GenerateRegisterInternalCallMacro(StringBuilder sb)
        {
            sb.AppendLine();
            sb.AppendLine("#if ENABLE_DOTNET");
            sb.AppendLine(@"#include ""Runtime/Scripting/WinRTHelper.h""");
            sb.AppendLine();
            sb.AppendLine("#define REGISTER_INTERNAL_CALL(MANAGED_NAME_, FUNCTION_) \\");
            sb.AppendLine("    long long* p = GetWinRTFuncDefsPointers(); \\");
            sb.AppendLine("    p[k##FUNCTION_##Func] = (long long)FUNCTION_");
            sb.AppendLine("#else");
            sb.AppendLine("#define REGISTER_INTERNAL_CALL(MANAGED_NAME_, FUNCTION_) \\");
            sb.AppendLine("    scripting_add_internal_call(MANAGED_NAME_, (gpointer)& FUNCTION_)");
            sb.AppendLine("#endif");
            sb.AppendLine();
        }

        private void GenerateExportFunctions(StringBuilder sb, FileNode file, string inputFilePath)
        {
            sb.AppendLine("#if !INTERNAL_CALL_STRIPPING");

            var fileName = Path.GetFileName(inputFilePath);
            var fileNameWithNoExtension = fileName.Substring(0, fileName.IndexOf('.'));

            sb.AppendLine($"void Export{fileNameWithNoExtension}BindingsNew() {{");

            foreach (var truncTypeName in DistinctTruncatedTypeNames(file))
            {
                var funcs = FunctionsWithTruncatedTypeName(file, truncTypeName);

                foreach (var functionNode in funcs)
                    sb.AppendLine($"    Register_{NormalizeManagedNameOf(functionNode)}();");
            }

            sb.AppendLine("}");

            sb.AppendLine("#endif");
        }

        private static string NormalizeManagedNameOf(FunctionNode functionNode)
        {
            return functionNode.ManagedName.Replace(".", "_").Replace("::", "_").Replace("/", "_");
        }

        private static string GetTruncatedTypeName(string fullMethodName)
        {
            return fullMethodName.Split(':')[0].Replace(".", "").Replace("/", "");
        }

        private static IEnumerable<string> DistinctTruncatedTypeNames(FileNode file)
        {
            return file.DefaultNamespace.Functions.Select(f => GetTruncatedTypeName(f.ManagedName)).Distinct();
        }

        private IEnumerable<FunctionNode> FunctionsWithTruncatedTypeName(FileNode file, string truncTypeName)
        {
            return file.DefaultNamespace.Functions.Where(f => GetTruncatedTypeName(f.ManagedName) == truncTypeName);
        }
    }
}
