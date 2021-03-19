using System;
using System.Collections.Generic;
using System.Linq;
using MemDoc;

namespace UnderlyingModel.MemDoc
{
    //a collection of signatures which have a common description
    public class MemberSubSection
    {
        //the list of signatures that all the docs for this block apply to
        public List<string> SignatureList { set; get; }
        public List<SignatureEntry> SignatureEntryList { set; get; }

        //union of all parameters for this meaningful block, derived from the signature list
        public List<ParameterWithDoc> Parameters { set; get; }
        public bool ContainsParameter(string name) { return Parameters.Any(e => e.Name == name); }
        public ParameterWithDoc GetParameter(string name) { return Parameters.FirstOrDefault(e => e.Name == name); }

        //the return type for this meaningful block (there must be only 1 return type in such a block
        public ReturnWithDoc ReturnDoc { set; get; }

        public string Summary { set; get; }

        //assuming each CSNONE function will have its own description and does not come in a list of overloaded functions, we can have 1 flag for the whole MeaningfulBlock
        public bool IsDocOnly { get; set; }
        public bool IsUndoc { get; set; }
        public List<TextBlock> TextBlocks { set; get; }

        public MemberSubSection(string memberDoc)
        {
            InitBasics();
            var remainingLines = new List<string>(memberDoc.SplitUnixLines());
            MiniBlockParser miniBlockParser = new MemMiniBlockParser(this);
            miniBlockParser.ProcessOneMeaningfulBlock(ref remainingLines);
            //Assert.IsFalse (remainingLines.Any (), "memInput="+ memberDoc);
            //EnforcePunctuation ();
        }

        private void InitBasics()
        {
            Parameters = new List<ParameterWithDoc>();
            ReturnDoc = null;
            Summary = "";
            TextBlocks = new List<TextBlock>();
            SignatureList = new List<string>();
            SignatureEntryList = new List<SignatureEntry>();
            IsDocOnly = false;
        }

        public MemberSubSection()
        {
            InitBasics();
        }

        public ParameterWithDoc GetOrCreateParameter(string name)
        {
            var p = Parameters.FirstOrDefault(m => m.Name == name);
            if (p == null)
            {
                p = new ParameterWithDoc(name);
                Parameters.Add(p);
            }
            return p;
        }

        public ReturnWithDoc GetOrCreateReturnDoc()
        {
            return ReturnDoc ?? (ReturnDoc = new ReturnWithDoc());
        }

        public void ProcessAsm(MemberItem member)
        {
            foreach (var sig in SignatureList)
            {
                // Don't include private signatures. We don't want the parameter list to include parameters used in private signatures only.
                SignatureEntry signature = member.GetSignature(sig);
                if (signature == null)
                    continue;
                if (!signature.InAsm)
                    continue;

                // Handle parameters
                System.Diagnostics.Debug.Assert(signature.Asm != null, "SignatureEntry.m_Asm for " + sig + " is null in ProcessAsm.");
                if (signature.Asm == null)
                    throw new Exception("SignatureEntry.m_Asm for " + sig + " is null in ProcessAsm.");
                System.Diagnostics.Debug.Assert(signature.Asm.ParamList != null, "SignatureEntry.m_Asm.m_ParamList for " + sig + " is null in ProcessAsm.");
                if (signature.Asm.ParamList == null)
                    throw new Exception("SignatureEntry.m_Asm.m_ParamList for " + sig + " is null in ProcessAsm.");
                foreach (ParamElement param in signature.Asm.ParamList)
                {
                    ParameterWithDoc paramWithDoc = GetParameter(param.Name);
                    if (paramWithDoc == null)
                    {
                        paramWithDoc = new ParameterWithDoc(param.Name);
                        Parameters.Add(paramWithDoc);
                    }
                    paramWithDoc.AddType(param.Type);
                }

                // Handle return type
                if (ReturnDoc != null)
                {
                    if (ReturnDoc.HasAsm)
                    {
                        if (ReturnDoc.ReturnType != signature.Asm.ReturnType)
                            throw new Exception("Different return types in the same MeaningfulBlock.");
                    }
                    else
                        ReturnDoc.ReturnType = signature.Asm.ReturnType;
                }
            }
        }

        public bool IsEmpty()
        {
            //Assert.IsNotNull (Parameters);
            //Assert.IsNotNull (TextBlocks);
            return
                Summary == "" &&
                (ReturnDoc == null || ReturnDoc.Doc == "") &&
                Parameters.All(e => e.Doc == "") &&
                TextBlocks.All(e => e.ToString() == "") &&
                !IsUndoc;
        }

        internal void SanitizeForEditing()
        {
            // If there's no text or example, add one
            if (TextBlocks.Count == 0)
                TextBlocks.Add(new DescriptionBlock(""));

            // If the last text or example is not a text, add one
            if (!(TextBlocks[TextBlocks.Count - 1] is DescriptionBlock))
                TextBlocks.Add(new DescriptionBlock(""));

            // Make sure there's a text before each example
            for (int i = TextBlocks.Count - 1; i >= 0; i--)
            {
                if (TextBlocks[i] is ExampleBlock && (i == 0 || !(TextBlocks[i - 1] is DescriptionBlock)))
                    TextBlocks.Insert(i, new DescriptionBlock(""));
            }

            // When there's two descriptions after each other, merge them together
            for (int i = TextBlocks.Count - 2; i >= 0; i--)
            {
                if (TextBlocks[i] is DescriptionBlock && TextBlocks[i + 1] is DescriptionBlock)
                {
                    TextBlocks[i].Text += "\n\n" + TextBlocks[i + 1].Text;
                    TextBlocks.RemoveAt(i + 1);
                }
            }
        }

        public void EnforcePunctuation()
        {
            foreach (var param in Parameters)
                param.Doc = EnforcePunctuation(param.Doc);
            if (ReturnDoc != null)
                ReturnDoc.Doc = EnforcePunctuation(ReturnDoc.Doc);
            Summary = EnforcePunctuation(Summary);
            foreach (TextBlock block in TextBlocks)
                if (block is DescriptionBlock)
                    block.Text = EnforcePunctuation(block.Text);
        }

        public static string EnforcePunctuation(string str)
        {
            str = EnforcePunctuationStart(str);
            str = EnforcePunctuationEnd(str);
            return str;
        }

        private static string EnforcePunctuationStart(string str)
        {
            if (str == null)
                return str;
            str = str.TrimStart();
            if (str == string.Empty)
                return str;
            char firstChar = str.First();
            int replaceAt = 0;

            if (firstChar == '\'')
            {
                if (str.Length > 2 && str.StartsWith(@"''") && str.EndsWith(@"''"))
                {
                    firstChar = str[2];
                    replaceAt = 2;
                }
            }

            if (char.IsLower(firstChar) && (str.Length < replaceAt + 2 || !char.IsUpper(str[replaceAt + 1])))
                str = str.Substring(0, replaceAt) + char.ToUpper(firstChar) + str.Substring(replaceAt + 1);
            return str;
        }

        private static string EnforcePunctuationEnd(string str)
        {
            if (str == null)
                return str;
            str = str.TrimEnd();
            if (str == string.Empty)
                return str;
            char lastChar = str.Last();
            int insertAt = str.Length;

            // If last character is paranthesis, look in second last char for punctuation.
            if (lastChar == ')')
            {
                if (str.EndsWith("(RO)"))
                {
                    lastChar = str[str.Length - 6];
                    insertAt = str.Length - 5;
                }
                else if (str.EndsWith("(Default)"))
                {
                    lastChar = str[str.Length - 11];
                    insertAt = str.Length - 10;
                }
                else
                    lastChar = str[str.Length - 2]; // char right before closing paranthesis
            }
            else if (lastChar == '\'')
            {
                if (str.EndsWith(@"''"))
                {
                    // If string ends with '', then look if punctuation before it.
                    lastChar = str[str.Length - 3];
                    int lastLineStartIndex = str.LastIndexOf('\n') + 1;
                    // If line started with '' too, put the punctuation inside formatting start and end, otherewise after it.
                    if (str[lastLineStartIndex] == '\'')
                        insertAt = str.Length - 2;
                }
            }
            else if (lastChar == '\\')
            {
                if (str.EndsWith(@"\\"))
                {
                    // If string ends with \\, then look if punctuation before it.
                    lastChar = str[str.Length - 3];
                    insertAt = str.Length - 2;
                }
            }

            if (lastChar == '.' || // Allow basic punctuation that ends a sentence.
                lastChar == ':' || // Allow basic punctuation that ends a sentence.
                lastChar == '?' || // Allow basic punctuation that ends a sentence.
                lastChar == '!' || // Allow basic punctuation that ends a sentence.
                lastChar == '>' || // Ignore if string ends with greater than sign, which is likely part of a html tag.
                lastChar == ';' || // Allow semi-colon in end. There's sometimes inline code that's not code examples per-se.
                lastChar == '_') // Ignore if string ends in underscore which is likely part of formatting.
                return str;

            str = str.Substring(0, insertAt) + "." + str.Substring(insertAt);
            return str;
        }
    }
}
