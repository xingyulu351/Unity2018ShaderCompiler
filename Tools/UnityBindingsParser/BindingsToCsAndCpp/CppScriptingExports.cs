using Mono.CSharp;

namespace BindingsToCsAndCpp
{
    [Order(1)]
    class CppScriptingExports : CppExports
    {
        private const string EntryHeaderFormat = "{0} {1}({2})";

        private const string EtwEntryFormat = "SCRIPTINGAPI_ETW_ENTRY({0})";
        private const string StackCheckFormat = "SCRIPTINGAPI_STACK_CHECK({0})";
        private const string ThreadCheckFormat = "SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK({0}){1}";

        public override string GenerateEntriesHeader(CppExports sender, string filename, int count)
        {
            output.AppendLine("#include \"UnityPrefix.h\"");
            output.AppendLine("#include \"Runtime/Scripting/ScriptingExportUtility.h\"");
            return base.GenerateEntriesHeader(sender, filename, count);
        }

        public override string GenerateEntry(CppExports sender, string filename, int count, CppUnit entry)
        {
            if (entry is CppText)
            {
                sender.OutputConditions(entry);
                output.Append(entry.OriginalContent);
                return base.GenerateEntry(sender, filename, count, entry);
            }

            sender.OutputConditions(entry);

            output.AppendLine("SCRIPT_BINDINGS_EXPORT_DECL");
            var className = GetClassName(entry);
            var fullName = className + "_" + entry.CustomName;
            var fullMappedName = fullName;

            output.AppendLine(EntryHeaderFormat, entry.MappedReturnType + " SCRIPT_CALL_CONVENTION", fullMappedName, entry.ParameterSignatureWithSelf.CommaSeparatedList());
            output.AppendStartBlock();

            output.AppendLine(EtwEntryFormat, fullMappedName);

            foreach (var param in entry.MappedParameterTypes)
            {
                if (param.Item2 == "void**")
                {
                    //vs2010 has a bug where it fails to compile "void** myvar(some_other_voidstarstar);", so we'll emit it as an assignment instead
                    output.AppendLine("{0} {1} = {1}_;", param.Item2, param.Item1);
                }
                else
                    output.AppendLine("{0} {1}({1}_);", param.Item2, param.Item1);

                output.AppendLine("UNUSED({0});", param.Item1);
            }

            output.AppendLine(StackCheckFormat, entry.InternalName);

            if (!entry.IsThreadSafe)
                output.AppendLine(ThreadCheckFormat, entry.InternalName, entry.SyncString);

            var body = entry.Body.Split('\n');
            foreach (var line in body)
                output.AppendLine("{0}", line);
            output.AppendEndBlock();
            return base.GenerateEntry(sender, filename, count, entry);
        }

        public override string GenerateFooter(CppExports sender, string filename)
        {
            sender.FinishConditions();
            return base.GenerateFooter(sender, filename);
        }
    }
}
