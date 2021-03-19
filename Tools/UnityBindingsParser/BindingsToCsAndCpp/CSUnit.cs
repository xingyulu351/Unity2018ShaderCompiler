using ICSharpCode.NRefactory.CSharp;

namespace BindingsToCsAndCpp
{
    public class CSUnit : Unit
    {
        public override void Generate()
        {
            GenerateAttributes();
            GenerateICall();
        }

        protected virtual void GenerateICall()
        {
        }

        protected virtual void GeneratePublic()
        {
        }

        protected virtual void GenerateICallHeader()
        {
        }

        protected virtual void GenerateAttributes()
        {
            if (IsObsolete)
                if (ObsoleteKind != "planned")
                    output.AppendLine(ParsingHelpers.GenerateObsolete(ObsoleteKind, ObsoleteText));

            if (IsThreadSafe)
                output.AppendLine("[ThreadAndSerializationSafe ()]");
        }

        protected virtual string GetDirectionForCall(ParameterDeclaration parameter, int padding = 0)
        {
            var ret = parameter.GetDirectionForCall();
            return string.IsNullOrWhiteSpace(ret) ? string.Empty : ret.PadRight(ret.Length + padding);
        }
    }
}
