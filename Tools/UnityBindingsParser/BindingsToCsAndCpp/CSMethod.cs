using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;

namespace BindingsToCsAndCpp
{
    public class CSMethod : CSBaseMethod
    {
        public override string StaticModifier
        {
            get
            {
                return (base.StaticModifier + " ").ToStringOrEmpty(!IsStatic);
            }
        }

        public CSMethod()
        {
        }

        public CSMethod(string className, string content)
            : base(className, content)
        {
        }

        public override void Generate()
        {
            // We don't want to generate attributes at this point
            GenerateICall();
        }

        protected override void GenerateICall()
        {
            if (NeedsInternal)
            {
                GeneratePublic();
                GenerateDefaultOverloads(!IsStatic, string.Empty, true);
                GeneratePrivateICall();
            }
            else
            {
                GeneratePublicICall();
                GenerateDefaultOverloads(false, string.Empty, true);
            }
        }

        protected void GenerateDefaultOverloads(bool needsThis, string whereClause, bool isICall)
        {
            if (FullDefaultParams.Count == 0)
                return;

            var ret = ReturnType != "void" && ReturnType != "" ? "return " : "";

            const string sig = @"{0} {1}{2} {3}{4} ({5}){6} {{";
            const string defaultVar = @"{0} {1}{2};";

            var reversedDefaultParams = FullDefaultParams.ToList();
            reversedDefaultParams.Reverse();

            var normalParamSigs = NormalParamSigs.Select(x => x.Trim()).CommaSeparatedList().Split(',', StringSplitOptions.RemoveEmptyEntries);
            var callSignature = (needsThis ? new[] {"this"} : new string[] {}).Union(CleanParameterCall.Split(','));

            var genericParameters = HasGenericParameters ? GenericParametersString : string.Empty;

            var formattedWhereClause = whereClause;
            if (!string.IsNullOrEmpty(whereClause))
            {
                formattedWhereClause = " " + whereClause;
            }
            for (var i = FullDefaultParams.Count - 1; i >= 0; i--)
            {
                GenerateAttributes();
                output.AppendLine("[uei.ExcludeFromDocs]");

                var current = normalParamSigs.Union(FullDefaultParams.Take(i).Select(x => x.Item2)).Join(",");
                // Cheap formatting problem if the where clause is empty, we get a double space if we add it to the formatting above.
                // Eliminate this by adding the space if needed.
                output.AppendLineFormat(sig, Visibility, StaticModifier, ReturnType, Name.Trim(), genericParameters, current, formattedWhereClause);

                output.StartBlock();

                foreach (var param in reversedDefaultParams.Take(FullDefaultParams.Count - i))
                    output.AppendLineFormat(defaultVar, param.Item1.Type, param.Item1.Name, param.Item3.TrimEnd());

                if (!isICall || !AstReturnType.ShouldRewriteReturnValueAsParameterByReference())
                {
                    const string call = @"{0}{1}{2} ( {3} );";
                    output.AppendLineFormat(call, ret, InternalName, genericParameters, callSignature.CommaSeparatedList());
                }
                else
                {
                    const string call = @"{0}{1} ( {2} );";
                    callSignature = callSignature.Concat(new[] { "out result" });

                    output.AppendLineFormat("{0} result;", ReturnType);
                    output.AppendLineFormat(call, InternalName, genericParameters, callSignature.CommaSeparatedList());
                    output.AppendLine("return result;");
                }

                output.AppendEndBlock();
            }
        }

        protected void GeneratePublicICall()
        {
            GenerateAttributes();
            GenerateICallHeader();

            const string sig = @"extern {0} {1}({2}) {3};";

            var retType = IsConstructor ? "" : ReturnType;
            var str = new[] {Visibility, IsOverride ? "override" : (IsNew ? "new" : ""), StaticModifier, retType, Name }.Where(s => !string.IsNullOrWhiteSpace(s)).Join(" ");

            output.AppendLineFormat(sig, str, GenericParametersString, CleanParameterSignature, ConstraintString);
            output.AppendLineNoIndent();
        }

        protected override void GeneratePublic()
        {
            const string sig = @"{0} {1}{2} {3} ({4}) {{";

            var ret = IsConstructor || ReturnType == "void" ? "" : "return ";
            var retType = IsConstructor ? "" : ReturnType;

            var callParameters = new List<string>();
            if (!IsStatic)
            {
                if (ParentIsStruct)
                    callParameters.Add("ref this");
                else
                    callParameters.Add("this");
            }
            callParameters.AddRange(Parameters.Select(x => GetDirectionForCall(x, 1) + x.Name));

            GenerateAttributes();

            output.AppendLineFormat(sig, Visibility, StaticModifier, retType, Name, CleanParameterSignature);
            output.StartBlock();

            if (!AstReturnType.ShouldRewriteReturnValueAsParameterByReference())
            {
                const string body = @"{0}{1} ( {2} );";
                output.AppendLineFormat(body, ret, InternalName, callParameters.CommaSeparatedList());
            }
            else
            {
                const string body = @"{0} ( {1} );";
                callParameters.Add("out result");

                output.AppendLineFormat("{0} result;", ReturnType);
                output.AppendLineFormat(body, InternalName, callParameters.CommaSeparatedList());
                output.AppendLine("return result;");
            }

            output.AppendEndBlock();
        }

        protected void GeneratePrivateICall()
        {
            GenerateICallHeader();

            const string sig = @"private extern static {0} {1} ({2});";

            var callParameters = new List<string>();
            if (!IsStatic)
                callParameters.Add(string.Format(SelfFormat, ParentIsStruct ? "ref " : "", ClassName));
            callParameters.AddRange(Parameters.Select(x => GetDirectionForCall(x, 1) + x.GetParamSignature()));

            if (!AstReturnType.ShouldRewriteReturnValueAsParameterByReference())
            {
                output.AppendFormat(sig, ReturnType, InternalName, callParameters.CommaSeparatedList());
            }
            else
            {
                callParameters.Add(string.Format("out {0} value", ReturnType));
                output.AppendFormat(sig, "void", InternalName, callParameters.CommaSeparatedList());
            }
            output.AppendLineNoIndent();
        }

        protected override void GenerateICallHeader()
        {
            output.AppendLine("[UnityEngine.Scripting.GeneratedByOldBindingsGeneratorAttribute] // Temporarily necessary for bindings migration");

            // InternalCall is not available in .NET Core
            output.AppendLine("[System.Runtime.CompilerServices.MethodImplAttribute((System.Runtime.CompilerServices.MethodImplOptions)0x1000)]");
        }
    }
}
