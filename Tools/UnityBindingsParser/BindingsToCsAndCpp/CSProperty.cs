namespace BindingsToCsAndCpp
{
    internal class CSProperty : CSBaseProperty
    {
        public override string StaticModifier { get { return base.StaticModifier + " "; } }

        public CSProperty(string className, string content)
            : base(className, content)
        {
            NeedsInternal = AstReturnType.ShouldRewriteReturnValueAsParameterByReference();
            var format = InternalNameFormat;
            if (!NeedsInternal)
                format = "{0}_{1}";
            GetterInternalName = string.Format(format, "get", Name);
            SetterInternalName = string.Format(format, "set", Name);
        }

        public override void Generate()
        {
            GenerateICall();
        }

        protected override void GenerateICall()
        {
            if (NeedsInternal)
            {
                // Output internal icall last, so OBSOLETE does not get set for the internal icall.
                GeneratePublic();
                GenerateInternalICall();
            }
            else
            {
                if (ParentIsStruct)
                {
                    output.AppendLine("#if __MonoCS__");
                    GeneratePublicICall();
                    output.AppendLine("#else");
                    GenerateDummy();
                    output.AppendLine("#endif");
                }
                else
                    GeneratePublicICall();
            }
        }

        void GenerateDummy()
        {
            const string sig = "{0} {1}{2} {3}";
            const string getter = @"get {{return default({0});}}";
            const string setter = @"set {{}}";

            GenerateAttributes();

            output.AppendLineFormat(sig, Visibility, StaticModifier, ReturnType, Name);
            output.AppendStartBlock();
            output.AppendLine("#if ENABLE_DOTNET");
            GenerateICallHeader();
            output.AppendLine("#endif");
            output.AppendLineFormat(getter, ReturnType, GetterInternalName);
            if (HasSetter)
            {
                output.AppendLine("#if ENABLE_DOTNET");
                GenerateICallHeader();
                output.AppendLine("#endif");
                output.AppendLineFormat(setter, ReturnType, SetterInternalName);
            }
            output.AppendEndBlock();
        }

        protected void GenerateInternalICall()
        {
            const string sig = "extern private {0}void {1} ({2} {3} value) ;";

            GenerateICallHeader();
            output.AppendLineFormat(sig, StaticModifier, GetterInternalName, "out", ReturnType);
            output.AppendLineNoIndent();

            if (HasSetter)
            {
                GenerateICallHeader();
                output.AppendLineFormat(sig, StaticModifier, SetterInternalName, "ref", ReturnType);
            }
            output.AppendLineNoIndent();
        }

        protected override void GeneratePublic()
        {
            const string sig = "{0} {1}{2}{3} {4}";
            const string getter = @"get {{ {0} tmp; {1}(out tmp); return tmp;  }}";
            const string setter = @"set {{ {1}(ref value); }}";

            GenerateAttributes();

            output.AppendLineFormat(sig, Visibility, IsOverride ? "override" : (IsNew ? "new" : ""), StaticModifier, ReturnType, Name);
            output.AppendStartBlock();
            output.AppendLineFormat(getter, ReturnType, GetterInternalName);

            if (HasSetter)
                output.AppendLineFormat(setter, ReturnType, SetterInternalName);
            output.AppendEndBlock();
        }

        protected void GeneratePublicICall()
        {
            const string sig = "{0} extern {1}{2}{3} {4}";

            GenerateAttributes();

            output.AppendLineFormat(sig, Visibility, IsOverride ? "override " : (IsNew ? "new " : ""), StaticModifier, ReturnType, Name);
            output.AppendStartBlock();
            GenerateICallHeader();
            output.AppendLine("get;");
            if (HasSetter)
            {
                GenerateICallHeader();
                output.AppendLine("set;");
            }
            output.AppendEndBlock();
        }

        protected override void GenerateICallHeader()
        {
            output.AppendLine("[UnityEngine.Scripting.GeneratedByOldBindingsGeneratorAttribute] // Temporarily necessary for bindings migration");

            // InternalCall is not available in .NET Core
            output.AppendLine("[System.Runtime.CompilerServices.MethodImplAttribute((System.Runtime.CompilerServices.MethodImplOptions)0x1000)]");
        }
    }
}
