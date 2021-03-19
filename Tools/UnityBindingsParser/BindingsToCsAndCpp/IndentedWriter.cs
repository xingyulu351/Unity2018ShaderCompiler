#pragma warning disable 414

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace BindingsToCsAndCpp
{
    public class IndentedWriter
    {
        public enum IndentStyleName
        {
            Tab = '\t',
            Space = ' '
        };

        public readonly IndentStyleName IndentStyle;
        public readonly int IndentSize;

        public IndentedWriter() : this(IndentStyleName.Space, 4) {}

        public IndentedWriter(IndentStyleName indentStyle)
        {
            IndentStyle = indentStyle;
            switch (IndentStyle)
            {
                case IndentStyleName.Tab:
                    IndentSize = 1;
                    break;
                case IndentStyleName.Space:
                    IndentSize = 4;
                    break;
                default:
                    throw new NotImplementedException();
            }
        }

        public IndentedWriter(IndentStyleName indentStyle, int indentSize)
        {
            IndentStyle = indentStyle;
            IndentSize = indentSize;
        }

        private char IndentChar
        {
            get
            {
                return (char)IndentStyle;
            }
        }

        bool _normalized;
        bool _isNewLine = true;
        public int CurrentIndentationLevel { get; set; }

        StringBuilder output = new StringBuilder();
        public string Output { get { return ToString(); } }

        void Normalize()
        {
            if (_normalized)
                return;
            output = output.NormalizeUnix();
            _normalized = true;
        }

        public int Length { get { return output.Length; }}

        public void StartBlock()
        {
            CurrentIndentationLevel++;
        }

        public void EndBlock()
        {
            CurrentIndentationLevel--;
        }

        public void Clear()
        {
            output.Clear();
            _normalized = true;
        }

        public override string ToString()
        {
            Normalize();
            return output.ToString();
        }

        public void AppendStartBlock(string padding = "")
        {
            AppendLine("{" + padding);
            StartBlock();
        }

        public void AppendEndBlock(string padding = "")
        {
            EndBlock();
            AppendLine("}" + padding);
            AppendLineNoIndent();
        }

        public void AppendStartConditions(List<string> conditions, bool prependNewLine = false)
        {
            if (conditions.Count == 0)
                return;
            var str = string.Format("#if {0}",
                conditions.Count > 1
                ? string.Join(" && ", conditions.Select(x => "(" + x + ")"))
                : string.Join("", conditions));
            AppendLineNoIndent(str);
        }

        public void AppendEndConditions(List<string> conditions, bool prependNewLine = false)
        {
            if (conditions.Count == 0)
                return;
            if (prependNewLine)
                AppendLine();
            AppendLineNoIndent("#endif");
        }

        public void AppendEndConditions(bool prependNewLine = false)
        {
            if (prependNewLine)
                AppendLine();
            AppendLineNoIndent("#endif");
        }

        public void AppendNoIndent(string s, params object[] args)
        {
            if (args.Length == 0)
                output.Append(s);
            else
                output.AppendFormat(s, args);
            _normalized = false;
            _isNewLine = false;
        }

        public void Append(string s, params object[] args)
        {
            if (_isNewLine)
                AppendIndent();

            if (args.Length == 0)
                output.Append(s);
            else
                output.AppendFormat(s, args);
            _normalized = false;
            _isNewLine = false;
        }

        public void AppendLine()
        {
            AppendIndent();
            output.AppendLineUnix();
            _isNewLine = true;
        }

        public void AppendLine(string s, params object[] args)
        {
            AppendIndent();
            output.AppendLineUnix(args.Length == 0 ? s : string.Format(s, args));
            _normalized = false;
            _isNewLine = true;
        }

        public void AppendLineNoIndent()
        {
            output.AppendLineUnix();
            _normalized = false;
            _isNewLine = true;
        }

        public void AppendLineNoIndent(string s, params object[] args)
        {
            output.AppendLineUnix(args.Length == 0 ? s : string.Format(s, args));
            _normalized = false;
            _isNewLine = true;
        }

        public void AppendIndent()
        {
            if (CurrentIndentationLevel > -1)
                output.Append(IndentChar, IndentSize * CurrentIndentationLevel);
            _isNewLine = false;
        }

        public void AppendFormat(string format, params object[] args)
        {
            if (_isNewLine)
                AppendIndent();
            output.AppendFormat(format, args);
            _isNewLine = false;
        }

        public void AppendLineFormat(string format, params object[] args)
        {
            AppendIndent();
            output.AppendLineFormat(format, args);
            _isNewLine = true;
        }

        public void Append(char value, int repeatCount)
        {
            if (_isNewLine)
                AppendIndent();
            output.Append(value, repeatCount);
            _isNewLine = false;
        }
    }
}
