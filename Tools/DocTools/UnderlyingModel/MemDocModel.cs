using System;
using System.Collections.Generic;
using System.Linq;
using UnderlyingModel.MemDoc.Serializers;

namespace UnderlyingModel.MemDoc
{
    public class MemDocModel
    {
        public List<MemberSubSection> SubSections { get; set; }
        public List<MemDocModel> ChildModels; //needed for models of class's children
        private int m_blockToWriteTo;

        // The type coming from assembly, consider also serializing this with the .mem file
        public AssemblyType AssemblyKind { get; set; }
        public string ItemNameFromMember { get; set; }
        public string NamespaceFromMember { get; set; }
        public LanguageUtil.ELanguage Language { get; internal set; }

        public string ErrorMessage { get; private set; }

        public MemDocModel()
        {
            SubSections = new List<MemberSubSection>();
            ChildModels = new List<MemDocModel>();
            ErrorMessage = null;
            m_blockToWriteTo = 0;
            NamespaceFromMember = "";
            ItemNameFromMember = "";
        }

        public void ProcessAsm(MemberItem member)
        {
            try
            {
                foreach (MemberSubSection section in SubSections)
                    section.ProcessAsm(member);
            }
            catch (Exception e)
            {
                ErrorMessage = e.Message + e.StackTrace;
            }
        }

        //this should no longer be here, as we factored this logic out into the
        public void ParseFromString(string text)
        {
            try
            {
                MiniParser miniParser = new MemMiniParser(this);
                miniParser.ParseString(text);
            }
            catch (Exception e)
            {
                ErrorMessage = e.Message + e.StackTrace;
            }
        }

        public override string ToString()
        {
            var memDocModelStringSerializer = new MemDocModelStringSerializer(this);
            var content = memDocModelStringSerializer.Serialize();
            return content;
        }

        public void WriteToAnotherBlockNextTime()
        {
            var block = new MemberSubSection();
            SubSections.Add(block);
            m_blockToWriteTo++;
        }

        internal List<string> SignatureListFromAllBlocks
        {
            get
            {
                var allSigs = new List<string>();
                if (SubSections.Count == 0)
                    return allSigs;
                foreach (var section in SubSections)
                    allSigs.AddRange(section.SignatureList);
                return allSigs;
            }
        }

        internal MemberSubSection SubSectionOfSignature(string sig)
        {
            return SubSections.FirstOrDefault(section => section.SignatureList.Contains(sig));
        }

        public bool SignatureListContains(string st)
        {
            return SignatureListFromAllBlocks.Contains(st);
        }

        public int SignatureCount
        {
            get { return SignatureListFromAllBlocks.Count; }
        }

        public void AddSignatureToCurrentBlock(string sig)
        {
            if (m_blockToWriteTo > SubSections.Count - 1)
                SubSections.Add(new MemberSubSection());
            SubSections[m_blockToWriteTo].SignatureList.Add(sig);
        }

        public void SanitizeForEditing()
        {
            foreach (MemberSubSection section in SubSections)
                section.SanitizeForEditing();
        }

        public void EnforcePunctuation()
        {
            foreach (MemberSubSection section in SubSections)
                section.EnforcePunctuation();
        }

        public bool IsEmpty()
        {
            return SubSections.All(e => e.IsEmpty());
        }

        public bool IsUndoc()
        {
            return SubSections.Count > 0 && SubSections.All(s => s.IsUndoc);
        }

        public bool IsDocOnly()
        {
            return SubSections.Count > 0 && SubSections.All(s => s.IsDocOnly);
        }

        public void AddNewSectionWithSummary(string summary)
        {
            SubSections.Add(new MemberSubSection { Summary = summary });
        }

        #region Functions useful for APIDocumentationGenerator

        public string Summary
        {
            get { return SubSections.Count > 0 ? SubSections[0].Summary : ""; }
        }

        public List<TextBlock> TextBlocks
        {
            get { return SubSections.Count > 0 ? SubSections[0].TextBlocks : new List<TextBlock>(); }
        }

        #endregion
    }
}
