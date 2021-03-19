using System.Text;
using MemDoc;

namespace UnderlyingModel.MemDoc
{
    public class ExampleBlock : TextBlock
    {
        public bool IsConvertExample { set; get; }
        public bool IsNoCheck { set; get; }
        public string JsExample { set; get; }
        public string CsExample { set; get; }
        public readonly CompLang DetectedSourceLang;

        public ExampleBlock(string js, string cs)
            : base(js + TxtToken.CSharpDelimiter + cs)
        {
            JsExample = js;
            CsExample = cs;
            if (js != "" && cs != "")
                DetectedSourceLang = CompLang.Dual;
            else
                DetectedSourceLang = js != "" ? CompLang.JavaScript : CompLang.CSharp;
        }

        public ExampleBlock(string example, CompLang sourceLang)
            : base(example)
        {
            JsExample = CsExample = "";
            if (sourceLang == CompLang.JavaScript)
                JsExample = example;
            else
                CsExample = example;
            DetectedSourceLang = sourceLang;
        }

        //default old format = UnityScript-only example
        public ExampleBlock(string txt) : base(txt)
        {
            CsExample = "";
            JsExample = "";
            var lines = txt.SplitUnixLines();
            var exampleNoMarkup = new StringBuilder();
            bool csStarted = false;
            DetectedSourceLang = CompLang.JavaScript;
            //parse the lines to extract CONVERTEXAMPLE and NOCHECK flags
            foreach (var line in lines)
            {
                if (line.StartsWith(TxtToken.ConvertExample))
                {
                    IsConvertExample = true;
                    //  if (line.Contains(TxtToken.CSharpSource))
                    //  SourceLang = CompLang.CSharp;
                }
                if (line.StartsWith(TxtToken.NoCheck))
                    IsNoCheck = true;
                if (line.StartsWith(TxtToken.CSharpDelimiter))
                {
                    csStarted = true;
                    JsExample = exampleNoMarkup.ToString();
                    DetectedSourceLang = JsExample.Trim().IsEmpty() ? CompLang.CSharp : CompLang.Dual;

                    exampleNoMarkup.Length = 0;
                    continue;
                }

                exampleNoMarkup.AppendUnixLine(line);
            }

            if (csStarted)
                CsExample = exampleNoMarkup.ToString();
            else
                JsExample = exampleNoMarkup.ToString();

            Text = ToString();
        }

        //includes BEGIN EX and END EX tags
        public override string ToString()
        {
            var sb = new StringBuilder();
            if (IsConvertExample)
            {
                sb.Append(TxtToken.ConvertExample);
                //if (SourceLang == CompLang.CSharp)
                //sb.AppendFormat(" {0} ", TxtToken.CSharpSource);
                sb.AppendUnixLine();
            }

            var nocheckString = IsNoCheck ? " " + TxtToken.NoCheck : "";
            //var langString = SourceLang == CompLang.JavaScript ? "" : " " + TxtToken.CSharpSource;
            //note that the last line of text will terminate in an endline
            sb.Append(TxtToken.BeginEx);
            sb.AppendUnixLine(nocheckString);
            if (!JsExample.IsEmpty())
                sb.AppendUnixLine(JsExample.TrimEndAndNewlines());
            if (!CsExample.IsEmpty())
            {
                sb.AppendUnixLine(TxtToken.CSharpDelimiter);
                sb.AppendUnixLine(CsExample.TrimEndAndNewlines());
            }
            sb.Append(TxtToken.EndEx);
            //sb.AppendFormat("{0}{1}\n{2}\n{3}", TxtToken.BeginEx, nocheckString, textNoEndline, TxtToken.EndEx);
            return sb.ToString();
        }

        //excludes BEGIN EX and END EX tags
        public string PureText()
        {
            var sb = new StringBuilder();
            if (!JsExample.IsEmpty())
                sb.AppendUnixLine(JsExample.TrimEndAndNewlines());
            if (!CsExample.IsEmpty())
            {
                sb.AppendUnixLine(TxtToken.CSharpDelimiter);
                sb.AppendUnixLine(CsExample.TrimEndAndNewlines());
            }
            return sb.ToString();
        }
    }
}
