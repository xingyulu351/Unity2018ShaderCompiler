namespace BindingsToCsAndCpp
{
    [Order(2)]
    class CppStrippingExports : CppExports
    {
        private const string RegisterLine1 =
            "{0} Register_{1}_{2}_{3}()";

        private const string RegisterLine2 =
            "scripting_add_internal_call( \"{0}.{1}::{3}\" , (gpointer)& {2}_{4} );";

        public override string GenerateHeader(CppExports sender, string filename)
        {
            output.AppendLine("#if !defined(INTERNAL_CALL_STRIPPING)");
            output.AppendLine("#   error must include unityconfigure.h");
            output.AppendLine("#endif");
            output.AppendLine("#if INTERNAL_CALL_STRIPPING");
            return base.GenerateHeader(sender, filename);
        }

        public override string GenerateEntry(CppExports sender, string filename, int count, CppUnit entry)
        {
            if (entry is CppText)
                return base.GenerateEntry(sender, filename, count, entry);

            sender.OutputConditions(entry);
            output.AppendLine(RegisterLine1, "void", entry.Namespace.Replace('.', '_'), GetClassName(entry), entry.InternalName.Replace('.', '_'));
            output.AppendStartBlock();
            output.AppendLine(RegisterLine2, entry.Namespace, GetClassName(entry, "/"), GetClassName(entry), entry.InternalName, entry.CustomName);
            output.AppendEndBlock();
            return base.GenerateEntry(sender, filename, count, entry);
        }

        public override string GenerateEntriesFooter(CppExports sender, string filename, int count)
        {
            sender.FinishConditions();
            return base.GenerateEntriesFooter(sender, filename, count);
        }
    }
}
