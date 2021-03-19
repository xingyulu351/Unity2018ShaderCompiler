using System;
using System.IO;
using System.Linq;
using System.Xml.Linq;
using UnderlyingModel.MemDoc;
using UnderlyingModel.MemDoc.Serializers;

namespace UnderlyingModel.ItemSerializers
{
    public class MemberItemIOMem2
    {
        private readonly NewDataItemProject _proj;

        public uint Version { private get; set; }

        //TODO: get rid of the dependency on NewDataItemProject
        public MemberItemIOMem2(NewDataItemProject proj)
        {
            _proj = proj;
            Version = 3;
        }

        public void MoveDoc(MemberItem item, string oldPlatform, string newPlatform)
        {
            var oldPath = DirectoryCalculator.GetFullPathWithPlatform(item, oldPlatform);
            var newPath = DirectoryCalculator.GetFullPathWithPlatform(item, newPlatform);
            try
            {
                var newPathDir = Path.GetDirectoryName(newPath);
                if (string.IsNullOrEmpty(newPathDir))
                {
                    throw new DirectoryNotFoundException("could not find directory for " + newPath);
                }
                Directory.CreateDirectory(newPathDir);
                File.Move(oldPath, newPath);
            }
            catch (DirectoryNotFoundException ex)
            {
                Console.WriteLine("old = {0}, new = {1}", oldPath, newPath);
                Console.WriteLine(ex.Message);
            }
        }

        //load all the DocModels from child items into this DocModel.ChildModels
        //then serialize that.
        public void SaveDoc(MemberItem cl, string platformName = "")
        {
            var xmlOutput = GetXmlOutputFromMember(cl);

            var fullPath = DirectoryCalculator.GetFullPathWithPlatform(cl, platformName);
            MakeDirIfNeeded(fullPath);

            XmlUtils.WriteToFileWithHeader(fullPath, xmlOutput);
        }

        public void SaveDocAtPath(MemberItem cl, string fullPath)
        {
            var xmlOutput = GetXmlOutputFromMember(cl);
            MakeDirIfNeeded(fullPath);
            XmlUtils.WriteToFileWithHeader(fullPath, xmlOutput);
        }

        private XElement GetXmlOutputFromMember(MemberItem cl)
        {
            cl.PopulateDocModelBitsFromMemberItem();
            cl.DocModel.NamespaceFromMember = cl.ActualNamespace;

            //copy things from assembly into DocModels, as we intend to save them to the mem2 file
            foreach (var child in cl.ChildMembers)
                child.PopulateDocModelBitsFromMemberItem();

            var childrenDocModels = cl.ChildMembers.Select(m => m.DocModel).ToList();

            //serialize the model into XElement
            var ser = new MemDocModelXmlSerializer {Version = this.Version};

            if (cl.DocModel != null && cl.ItemType != AssemblyType.Assembly)
                cl.DocModel.ChildModels = childrenDocModels;
            var xmlOutput = new XElement(XmlTags.Root, ser.Serialize(cl.DocModel));
            xmlOutput.SetAttributeValue(XmlTags.Version, this.Version);
            return xmlOutput;
        }

        private static void MakeDirIfNeeded(string fullFilePath)
        {
            var fullDir = Path.GetDirectoryName(fullFilePath);
            if (fullDir != null)
                Directory.CreateDirectory(fullDir);
        }

        public void LoadDoc(MemberItem member, string platformName = "")
        {
            Console.WriteLine("LoadDoc invoked");
            var fullPath = DirectoryCalculator.GetFullPathWithPlatform(member, platformName);
            var modelWithChildren = LoadDocFromPath(fullPath);
            CrossPolinateModelAndItem(member, modelWithChildren);
        }

        public void CrossPolinateModelAndItem(MemberItem member, MemDocModel modelWithChildren)
        {
            member.ExchangeInfoBetweenModelAndItem(modelWithChildren);
            PopulateMemberItemChildrenFromMemDocModel(member);
        }

        public MemDocModel LoadDocFromPath(string fullPath)
        {
            var ser = new MemDocModelXmlSerializer {Version = this.Version};
            return ser.Deserialize(fullPath);
        }

        private void PopulateMemberItemChildrenFromMemDocModel(MemberItem member)
        {
            var memDocModel = member.DocModel;
            foreach (var childDocModel in memDocModel.ChildModels)
            {
                var childID = childDocModel.ItemNameFromMember;
                var childItem = _proj.GetMember(childID) ?? _proj.CreateAndAddMemberItem(childID);
                if (childItem.ItemType == AssemblyType.Unknown)
                    childItem.ItemType = childDocModel.AssemblyKind;
                //Console.WriteLine("child {0} exists in the model but not in the MemberItem", childID);
                //throw new Exception(string.Format("child {0} exists in the model but not in the MemberItem", childID));
                childItem.ExchangeInfoBetweenModelAndItem(childDocModel);
                _proj.m_MapNameToItem[childID] = childItem;
            }
            memDocModel.ChildModels.Clear();
        }

        public void DeleteDoc(MemberItem item)
        {
            throw new NotImplementedException();
        }
    }
}
