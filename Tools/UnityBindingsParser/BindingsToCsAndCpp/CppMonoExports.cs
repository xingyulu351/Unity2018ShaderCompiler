namespace BindingsToCsAndCpp
{
    [Order(3)]
    class CppMonoExports : CppExports
    {
        public CppMonoExports()
        {
            EntryRepeats = 3;
        }

        public override string GenerateHeader(CppExports sender, string filename)
        {
            output.AppendLine("#elif ENABLE_MONO || ENABLE_IL2CPP");
            return base.GenerateHeader(sender, filename);
        }

        public override string GenerateEntriesHeader(CppExports sender, string filename, int count)
        {
            output.AppendIndent();
            if (count == 0)
            {
                output.AppendLine(@"static const char* s_{0}_IcallNames [] =", filename);
                output.AppendStartBlock();
            }
            else if (count == 1)
            {
                output.AppendLine(@"static const void* s_{0}_IcallFuncs [] =", filename);
                output.AppendStartBlock();
            }
            else
            {
                output.AppendLine("void Export{0}Bindings();", filename);
                output.AppendLine("void Export{0}Bindings()", filename);
                output.AppendStartBlock();
                output.AppendLine("for (int i = 0; s_{0}_IcallNames [i] != NULL; ++i )", filename);
            }
            return base.GenerateEntriesHeader(sender, filename, count);
        }

        public override string GenerateEntry(CppExports sender, string filename, int count, CppUnit entry)
        {
            if (entry is CppText)
                return base.GenerateEntry(sender, filename, count, entry);

            switch (count)
            {
                case 0:
                {
                    sender.OutputConditions(entry);
                    var name = "\"{0}.{1}::{2}\"";
                    name = string.Format(name, entry.Namespace, GetClassName(entry, "/"), entry.InternalName).PadRight(40);
                    output.AppendLine("{0},    // -> {1}_{2}", name, GetClassName(entry), entry.CustomName);
                }
                break;
                case 1:
                {
                    sender.OutputConditions(entry);
                    var name = "{0}_{1}";
                    name = string.Format(name, GetClassName(entry), entry.CustomName).PadRight(40);
                    output.AppendLine("(const void*)&{0},  //  <- {1}.{2}::{3}", name, entry.Namespace, GetClassName(entry, "/"), entry.InternalName);
                }
                break;
            }

            return base.GenerateEntry(sender, filename, count, entry);
        }

        public override string GenerateEntriesFooter(CppExports sender, string filename, int count)
        {
            sender.FinishConditions();

            if (count < 2)
            {
                output.AppendLine("NULL");
                output.AppendEndBlock(";");
            }
            else
            {
                output.StartBlock();
                output.AppendLine("scripting_add_internal_call( s_{0}_IcallNames [i], s_{0}_IcallFuncs [i] );", filename);
                output.EndBlock();
                output.AppendEndBlock();
            }
            return base.GenerateEntriesFooter(sender, filename, count);
        }
    }
}
