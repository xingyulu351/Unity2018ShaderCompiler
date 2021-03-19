namespace BindingsToCsAndCpp
{
    [Order(5)]
    internal class CppMetroExports : CppExports
    {
        public override string GenerateHeader(CppExports sender, string filename)
        {
            output.AppendLine("#elif ENABLE_DOTNET");
            return base.GenerateHeader(sender, filename);
        }

        public override string GenerateEntriesHeader(CppExports sender, string filename, int count)
        {
            if (IsTargetMetro)
            {
                output.AppendLine("#include \"Runtime/Scripting/WinRTHelper.h\"");
            }
            else
            {
                output.AppendLine("// This comment is here on purpose, so Jam won't pick WinRTHelper.h as dependency for non WinRT targets, thus not doing unneeded recompilation.");
                output.AppendLine("//#include \"Runtime/Scripting/WinRTHelper.h\"");
            }
            output.AppendLine("void Export{0}Bindings()", filename);
            output.AppendStartBlock();
            output.AppendLine("long long* p = GetWinRTFuncDefsPointers();");
            output.AppendLine("#define SET_METRO_BINDING(Name) p[k##Name##Func] = (long long)Name;");
            return base.GenerateEntriesHeader(sender, filename, count);
        }

        public override string GenerateEntry(CppExports sender, string filename, int count, CppUnit entry)
        {
            if (entry is CppText)
                return base.GenerateEntry(sender, filename, count, entry);

            sender.OutputConditions(entry);

            output.AppendLine(
                "SET_METRO_BINDING({1}_{3}); //  <- {0}.{2}::{4}", entry.Namespace, GetClassName(entry), GetClassName(entry, "/"), entry.CustomName, entry.InternalName);
            return base.GenerateEntry(sender, filename, count, entry);
        }

        public override string GenerateEntriesFooter(CppExports sender, string filename, int count)
        {
            sender.FinishConditions();
            output.AppendEndBlock();
            return base.GenerateEntriesFooter(sender, filename, count);
        }
    }
}
