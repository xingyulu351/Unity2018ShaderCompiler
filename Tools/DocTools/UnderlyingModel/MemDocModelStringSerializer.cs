using System;
using System.Linq;
using System.Text;

namespace UnderlyingModel.MemDoc.Serializers
{
    public class MemDocModelStringSerializer
    {
        private readonly MemDocModel m_Model;

        public MemDocModelStringSerializer(MemDocModel model)
        {
            m_Model = model;
        }

        public string Serialize()
        {
            // Create signature block even if there are no signatures in cases where there
            // can be more than one signature. Otherwise the parameters / return type
            // docs and descriptions for the different sections get all mixed up.
            bool includeSignatureBlock = AssemblyTypeUtils.MultipleSignaturesPossibleForType(m_Model.AssemblyKind);
            if (m_Model.AssemblyKind == AssemblyType.Unknown && m_Model.SignatureCount == 0)
                includeSignatureBlock = false;

            var sb = new StringBuilder();
            foreach (var block in m_Model.SubSections)
            {
                var blockSerializer = new MemberSubSectionStringSerializer(block, includeSignatureBlock);
                sb.Append(blockSerializer.Serialize());
                if (block != m_Model.SubSections.Last())
                    sb.AppendUnixLine();
            }

            return sb.ToString();
        }

        public void Deserialize(string memFileContent)
        {
            var miniParser = new MemMiniParser(m_Model);
            miniParser.ParseString(memFileContent);
        }
    }

    public class MemberSubSectionStringSerializer
    {
        private readonly MemberSubSection m_Section;
        private readonly bool m_IncludeSignatureBlock;

        public MemberSubSectionStringSerializer(MemberSubSection section, bool includeSignatureBlock)
        {
            m_Section = section;
            m_IncludeSignatureBlock = includeSignatureBlock;
        }

        public string Serialize()
        {
            if (m_Section.IsEmpty() && !m_Section.SignatureList.Any())
                return string.Empty;

            var sb = new StringBuilder();
            if (m_IncludeSignatureBlock)
                OutputSignatureList(sb);
            if (m_Section.IsDocOnly || m_Section.IsUndoc)
            {
                sb.AppendUnixLine(MemToken.TxtTagOpen);
                if (m_Section.IsDocOnly)
                    sb.AppendUnixLine(TxtToken.CsNone);
                if (m_Section.IsUndoc)
                    sb.AppendUnixLine(MemToken.Undoc);
                sb.AppendUnixLine(MemToken.TxtTagClose);
            }
            OutputForSummary(sb);
            OutputForTextBlocks(sb);
            OutputForParameters(sb);
            OutputForReturnDoc(sb);

            return sb.ToString().Trim();
        }

        public void Deserialize(string memFileContent)
        {
            throw new NotImplementedException();
        }

        private void OutputSignatureList(StringBuilder sb)
        {
            sb.AppendUnixLine(MemToken.SignatureOpen);
            foreach (var sig in m_Section.SignatureList)
                sb.AppendUnixLine(sig);
            sb.AppendUnixLine(MemToken.SignatureClose);
        }

        private void OutputForSummary(StringBuilder sb)
        {
            if (!m_Section.Summary.IsEmpty())
                sb.AppendUnixLine(m_Section.Summary);
        }

        private void OutputForReturnDoc(StringBuilder sb)
        {
            if (m_Section.ReturnDoc != null && m_Section.ReturnDoc.HasDoc)
                sb.AppendUnixLine(m_Section.ReturnDoc.ToString());
        }

        private void OutputForParameters(StringBuilder sb)
        {
            foreach (var param in m_Section.Parameters)
                if (param.HasDoc)
                    sb.AppendUnixLine(param.ToString());
        }

        private void OutputForTextBlocks(StringBuilder sb)
        {
            foreach (var textOrExample in m_Section.TextBlocks)
            {
                string str = textOrExample.ToString().TrimEndAndNewlines();
                if (!str.IsEmpty())
                {
                    //note, we do not prepend slashes to examples regardless
                    sb.AppendUnixLine(str);
                }
            }
        }
    }
}
