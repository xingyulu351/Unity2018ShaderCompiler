using System.Text;

namespace Unity.InternalCallRegistrationWriter.CppWriter
{
    public class CodeWriter : IOutputWriter
    {
        private readonly StringBuilder _stringBuilder;
        private int _indentation;

        public CodeWriter()
        {
            _stringBuilder = new StringBuilder();
            _indentation = 0;
        }

        public void Indent()
        {
            _indentation++;
        }

        public void Dedent()
        {
            _indentation--;
        }

        public void AppendLine()
        {
            _stringBuilder.AppendLine();
        }

        public void AppendLine(string singleMethodRegistrationInvocationFor)
        {
            AppendIndentation();
            _stringBuilder.AppendLine(singleMethodRegistrationInvocationFor);
        }

        private void AppendIndentation()
        {
            var i = _indentation;
            while (i-- > 0)
                _stringBuilder.Append("\t");
        }

        public void AppendComment(string comment)
        {
            AppendLine(string.Format("//{0}", comment));
        }

        public void Append(string s)
        {
            _stringBuilder.Append(s);
        }

        public string Output()
        {
            return _stringBuilder.ToString();
        }

        public void BeginBlock()
        {
            AppendLine();
            Append("{");
            Indent();
        }

        public void EndBlock()
        {
            Dedent();
            AppendLine("}");
        }
    }
}
