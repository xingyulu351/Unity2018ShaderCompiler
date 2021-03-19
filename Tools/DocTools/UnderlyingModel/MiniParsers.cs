using System.Collections.Generic;
using System.Linq;
using MemDoc;

namespace UnderlyingModel.MemDoc.Serializers
{
    public abstract class MiniParser
    {
        protected MemDocModel _memDocModel;

        protected MiniParser(MemDocModel memDocModel)
        {
            _memDocModel = memDocModel;
        }

        internal abstract void ParseString(string st);
    }

    public class MemMiniParser : MiniParser
    {
        public MemMiniParser(MemDocModel memDocModel)
            : base(memDocModel)
        {
        }

        internal override void ParseString(string st)
        {
            var remainingLines = new List<string>(st.SplitUnixLines());
            ProcessSuggestedTypesIfAny(ref remainingLines);
            while (remainingLines.Any())
            {
                var block = new MemberSubSection();
                var miniBlockParser = new MemMiniBlockParser(block);
                miniBlockParser.ProcessOneMeaningfulBlock(ref remainingLines);
                //block.EnforcePunctuation ();
                _memDocModel.SubSections.Add(block);
            }
        }

        private void ProcessSuggestedTypesIfAny(ref List<string> remainingLines)
        {
            bool inSuggestedType = false;
            while (remainingLines.Any())
            {
                var line = remainingLines.First();
                var shortLine = line.Trim();
                if (shortLine.IsEmpty())
                {
                    remainingLines.RemoveAt(0);
                    continue;
                }
                if (shortLine.StartsWith(MemToken.SuggestedOpen))
                {
                    inSuggestedType = true;
                    remainingLines.RemoveAt(0);
                }
                else if (inSuggestedType)
                {
                    remainingLines.RemoveAt(0);
                    if (shortLine.StartsWith(MemToken.SuggestedClose))
                        return;

                    string shortLineLower = shortLine.ToLower();
                    if (shortLineLower == "class")
                        _memDocModel.AssemblyKind = AssemblyType.Class;
                    else if (shortLineLower == "method")
                        _memDocModel.AssemblyKind = AssemblyType.Method;
                    else if (shortLineLower == "property")
                        _memDocModel.AssemblyKind = AssemblyType.Property;
                    else if (shortLineLower == "constructor")
                        _memDocModel.AssemblyKind = AssemblyType.Constructor;
                    else
                        _memDocModel.AssemblyKind = AssemblyType.Unknown;
                }
                else
                    return;
            }
        }
    }
}
