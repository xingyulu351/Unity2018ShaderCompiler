using System.Collections.Generic;
using System.Linq;

namespace UnderlyingModel
{
    public partial class MemberItem
    {
        public bool AnyHaveAsm { get; private set; }
        public bool AnyHaveDoc { get; private set; }
        public bool AnyNestedChildrenHaveDoc { get; set; }

        internal bool AllThatHaveAsmHaveDoc { get; private set; }
        internal bool AnyThatHaveAsmHaveDoc { get; private set; }
        internal bool AllThatHaveDocHaveAsm { get; private set; }
        internal bool AnyThatHaveDocHaveAsm { get; private set; }

        public bool AllPrivate { get; private set; }

        public PresenceSimple PresenceFromAllAndAny(bool all, bool any)
        {
            return all ? PresenceSimple.All : (any ? PresenceSimple.SomeButNotAll : PresenceSimple.None);
        }

        internal void UpdateFlags()
        {
            if (!MultipleSignaturesPossible && Signatures.Count <= 1)
            {
                AnyHaveAsm = ItemType != AssemblyType.Unknown;
                AnyHaveDoc = DocModel != null && !DocModel.IsEmpty() && !DocModel.IsUndoc();

                AllThatHaveAsmHaveDoc = !AnyHaveAsm || AnyHaveDoc;
                AnyThatHaveAsmHaveDoc = !AnyHaveAsm || AnyHaveDoc;
                AllThatHaveDocHaveAsm = !AnyHaveDoc || AnyHaveAsm || DocModel.IsDocOnly();
                AnyThatHaveDocHaveAsm = !AnyHaveDoc || AnyHaveAsm || DocModel.IsDocOnly();
            }
            else
            {
                AnyHaveAsm = Signatures.Any(e => e.InAsm);
                if (ItemType == AssemblyType.Unknown)
                    AnyHaveDoc = Signatures.Any(e => e.InDoc) || DocModel != null && !DocModel.IsEmpty() && !DocModel.IsUndoc();
                else
                    AnyHaveDoc = Signatures.Any(e => e.InDoc) && DocModel != null && !DocModel.IsEmpty() && !DocModel.IsUndoc();

                IEnumerable<SignatureEntry> asmOrDoc = Signatures.Where(s => (s.InAsm || s.InDoc)).ToList();

                AllThatHaveAsmHaveDoc = !AnyHaveAsm || (AnyHaveDoc && asmOrDoc.All(e => e.InDoc));
                AnyThatHaveAsmHaveDoc = !AnyHaveAsm || (AnyHaveDoc && asmOrDoc.Any(e => e.InDoc));
                AllThatHaveDocHaveAsm = !AnyHaveDoc || asmOrDoc.All(e => e.InAsm) || DocModel.IsDocOnly();
                AnyThatHaveDocHaveAsm = !AnyHaveDoc || asmOrDoc.Any(e => e.InAsm) || DocModel.IsDocOnly();
            }

            AllPrivate = m_SignatureList.Any(s => s.InAsm && s.Asm.Private) && !Signatures.Any(s => (s.InAsm || s.InDoc));
        }

        public PresenceSimple AsmPresenceComparedToDoc
        {
            get
            {
                return PresenceFromAllAndAny(AnyHaveAsm && AllThatHaveDocHaveAsm,
                    AnyHaveAsm && AnyThatHaveDocHaveAsm);
            }
        }

        public PresenceSimple DocPresenceComparedToAsm
        {
            get
            {
                return PresenceFromAllAndAny(AnyHaveDoc && AllThatHaveAsmHaveDoc,
                    AnyHaveDoc && AnyThatHaveAsmHaveDoc);
            }
        }
    }
}
