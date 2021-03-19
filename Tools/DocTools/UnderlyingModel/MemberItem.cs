using System.Collections.Generic;
using System.IO;
using System.Linq;
using UnderlyingModel.MemDoc.Serializers;
using ELanguage = UnderlyingModel.LanguageUtil.ELanguage;
using MemDocModel = UnderlyingModel.MemDoc.MemDocModel;

namespace UnderlyingModel
{
    public partial class MemberItem
    {
        public string ItemName { get; private set; }
        public AssemblyType ItemType { get; internal set; }
        public AssemblyType RealOrSuggestedItemType
        {
            get
            {
                if (ItemType != AssemblyType.Unknown)
                    return ItemType;
                return DocModel != null && DocModel.IsDocOnly() ? DocModel.AssemblyKind : AssemblyType.Unknown;
            }
        }
        private readonly List<SignatureEntry> m_SignatureList = new List<SignatureEntry>();
        private List<SignatureEntry> m_PublicSignatureList;
        bool m_PublicSignaturesDirty;
        public List<SignatureEntry> Signatures
        {
            get
            {
                if (m_PublicSignaturesDirty)
                {
                    m_PublicSignatureList = m_SignatureList.Where(e => (!e.InAsm || !e.Asm.Private)).ToList();
                    m_PublicSignaturesDirty = false;
                }
                return m_PublicSignatureList;
            }
        }

        //hierarchy parent (e.g. PlayerSettings.Android's nestedness parent is the PlayerSettings class)
        //corresponds to DeclaringType in Mono.Cesil
        public MemberItem ContainerClassMember { get; internal set; }

        //inheritance parent, not hierarchy parent (i.e. Mono.Cecil's BaseType, rather than DeclaringType)
        public MemberItem ParentMember { get; internal set; }
        public List<MemberItem> ChildMembers { get; internal set; }
        public List<MemberItem> ImplementedInterfaces { get; internal set; }
        public MemDocModel DocModelTranslated { get; private set; }
        public MemDocModel DocModel { get; set; }

        public bool AreAllProtected()
        {
            return Signatures != null && Signatures.Count > 0 && Signatures.All(m => m.Asm != null &&  m.Asm.IsProtectedUnsealed);
        }

        public MemDocModel GetDocModel(bool translated)
        {
            //if the translated doc is unavailable, use the English one
            return !translated || DocModelTranslated.Summary.IsEmpty() ? DocModel : DocModelTranslated;
        }

        private void SetDocModel(bool translated, MemDocModel memDocModel)
        {
            if (translated)
                DocModelTranslated = memDocModel;
            else
                DocModel = memDocModel;
        }

        public bool MultipleSignaturesPossible { get { return AssemblyTypeUtils.MultipleSignaturesPossibleForType(ItemType); } }

        public MemberItem(string name, AssemblyType asmType)
        {
            ItemName = name;
            ItemType = asmType;
            ChildMembers = new List<MemberItem>();
            m_PublicSignaturesDirty = true;
        }

        private void InitializeDocSignaturesIfNone(MemDocModel memDocModel)
        {
            // If member can only have one signature and it doesn't have it, add it based on asm signature
            if (memDocModel.SignatureCount == 0 && memDocModel.SubSections.Count <= 1)
            {
                List<SignatureEntry> sigEntries = Signatures.Where(e => e.InAsm).OrderBy(e => e.Asm.ReturnType).ToList();
                if (sigEntries.Any())
                {
                    string prevReturnType = sigEntries[0].Asm.ReturnType ?? "";
                    int section = 0;
                    foreach (SignatureEntry sigEntry in sigEntries)
                    {
                        string returnType = sigEntry.Asm.ReturnType ?? "";
                        if (returnType != prevReturnType)
                        {
                            prevReturnType = returnType;
                            section++;
                        }
                        //if there are more sections in the Asm than the model, skip
                        if (memDocModel.SubSections.Count <= section)
                            continue;

                        //add information from the Asm to Subsections
                        memDocModel.SubSections[section].SignatureList.Add(sigEntry.Name);
                        memDocModel.SubSections[section].SignatureEntryList.Add(sigEntry);
                    }
                }
                if (memDocModel.IsDocOnly())
                {
                    memDocModel.SubSections[0].SignatureList.Add(memDocModel.ItemNameFromMember);
                }
            }
        }

        public bool ContainsSignature(string sig, bool includePrivate = false)
        {
            if (includePrivate)
                return m_SignatureList.Any(e => e.Name == sig);
            return Signatures.Any(e => e.Name == sig);
        }

        public SignatureEntry GetSignature(string sig, bool includePrivate = false)
        {
            if (includePrivate)
                return m_SignatureList.FirstOrDefault(e => e.Name == sig);
            return Signatures.FirstOrDefault(e => e.Name == sig);
        }

        internal void AddSignature(SignatureEntry signature)
        {
            m_SignatureList.Add(signature);
            m_PublicSignaturesDirty = true;
        }

        //should no longer be used with Xml format, use MemberItemIOMem2
        public MemDocModel LoadDoc(string memFileContent)
        {
            // Create new MemDocModel
            var memDocModel = new MemDocModel { Language = ELanguage.en };
            var ser = new MemDocModelStringSerializer(memDocModel);
            ser.Deserialize(memFileContent);

            ExchangeInfoBetweenModelAndItem(memDocModel);

            return memDocModel;
        }

        private void UpdateMemberFromDocModel(MemDocModel memDocModel)
        {
            DocModel = memDocModel;
            UpdateMemberSignatures(memDocModel);
            UpdateFlags();
        }

        public void ExchangeInfoBetweenModelAndItem(MemDocModel model)
        {
            InitializeDocSignaturesIfNone(model);
            UpdateMemberFromDocModel(model);
            DocModel.ProcessAsm(this);
            PopulateDocModelBitsFromMemberItem();
        }

        private void UpdateMemberSignatures(MemDocModel memDocModel)
        {
            for (int i = m_SignatureList.Count - 1; i >= 0; i--)
            {
                m_SignatureList[i].SetInDoc(false);

                if (!m_SignatureList[i].InAsm && !m_SignatureList[i].InDoc)
                {
                    m_SignatureList.RemoveAt(i);
                    m_PublicSignaturesDirty = true;
                }
            }

            // Populate member signature list with signatures from (updated) doc
            foreach (string signature in memDocModel.SignatureListFromAllBlocks)
            {
                if (!m_SignatureList.Any(e => (e.Name == signature)))
                {
                    var sigParams = StringConvertUtils.SignatureToParamList(signature).ToArray();

                    // Look in m_SignatureList for matching method with optional parameters and set SignatureEntry.Asm
                    var signaturesWithAsm = m_SignatureList.Where(s => s.InAsm);

                    AsmEntry asm = null;

                    foreach (var sigAsm in signaturesWithAsm)
                    {
                        var sigWithAsmParams = StringConvertUtils.SignatureToParamList(sigAsm.Name).ToArray();

                        if (sigWithAsmParams.Length >= sigParams.Length)
                        {
                            bool match = true;

                            // Check if all parameter types match.
                            for (int i = 0; i < sigParams.Length && match; i++)
                            {
                                if (sigParams[i] != sigWithAsmParams[i])
                                    match = false;
                            }

                            // Types match, check is parameter length matches or if the next parameter is optional/has default (which means all remaining parameters are optional/have defaults)
                            if (match &&
                                (sigWithAsmParams.Length == sigParams.Length ||
                                 sigAsm.Asm.ParamList[sigParams.Length].Optional ||
                                 !sigAsm.Asm.ParamList[sigParams.Length].DefaultValue.IsEmpty()))
                            {
                                asm = sigAsm.Asm;
                                break;
                            }
                        }
                    }

                    m_SignatureList.Add(new SignatureEntry(signature) {Asm = asm, IsOverload = true});
                    m_PublicSignaturesDirty = true;
                }
                SignatureEntry signatureEntry = m_SignatureList.First(e => e.Name == signature);
                var subSection = memDocModel.SubSectionOfSignature(signature);
                if (subSection == null)
                    continue;

                if (!subSection.SignatureEntryList.Contains(signatureEntry))
                {
                    subSection.SignatureEntryList.Add(signatureEntry);
                }
                signatureEntry.SetInDoc(true);

                //if (signatureEntry.InAsm && signatureEntry.Asm.Private)
                //  System.Console.WriteLine ("Private documented signature "+signature+" in member "+ItemName);
            }
        }

        public void PopulateDocModelBitsFromMemberItem()
        {
            DocModel.ItemNameFromMember = ItemName;
            if (ItemType != AssemblyType.Unknown)
                DocModel.AssemblyKind = ItemType;
            if (AssemblyTypeUtils.CanHaveChildren(ItemType))
                DocModel.NamespaceFromMember = ActualNamespace;
        }

        public string Mem2FileName
        {
            get
            {
                string pureClassName;
                if (FirstCecilType == null)
                    pureClassName = GetNames().ClassName;
                else
                {
                    pureClassName = FirstCecilType.Name;
                    if (FirstCecilType.DeclaringType != null)
                        pureClassName = FirstCecilType.DeclaringType.Name + "." + pureClassName;
                }
                pureClassName = CecilHelpers.Backticks2Underscores(pureClassName);
                var fname = ActualNamespace.IsEmpty()
                    ? pureClassName
                    : Path.Combine(ActualNamespace, pureClassName);

                // Special handling for Assemblies, because GetNames() parses the dot in the name, which we don't want here.
                if (ItemType == AssemblyType.Assembly)
                    fname = ItemName;

                fname = string.Format("{0}.{1}", fname, DirectoryUtil.Mem2Extension);
                return fname;
            }
        }
    }
}
