using System;
using System.Collections.Generic;
using System.Linq;
using System.Xml.Linq;
using UnderlyingModel;

namespace XMLFormatter
{
    internal class ClassMemberCategories
    {
        internal ClassMemberCategories()
        {
            m_CategoryMap = new Dictionary<string, List<MemberItem>>();
            m_CategoryMap["StaticFieldAndProps"] = new List<MemberItem>();
            m_CategoryMap["RegularFieldsAndProps"] = new List<MemberItem>();
            m_CategoryMap["ProtectedVars"] = new List<MemberItem>();
            m_CategoryMap["StaticProtectedVars"] = new List<MemberItem>();
            m_CategoryMap["Constructors"] = new List<MemberItem>();
            m_CategoryMap["Messages"] = new List<MemberItem>();
            m_CategoryMap["ProtectedFunctions"] = new List<MemberItem>(); //non-static protected
            m_CategoryMap["RegularFunctions"] = new List<MemberItem>(); //non-static public
            m_CategoryMap["StaticFunctions"] = new List<MemberItem>();
            m_CategoryMap["Operators"] = new List<MemberItem>();
            m_CategoryMap["Events"] = new List<MemberItem>();
            m_CategoryMap["Delegates"] = new List<MemberItem>();
            m_CategoryMap["ImplementedInterfaces"] = new List<MemberItem>();
        }

        internal void SetCategoryValue(string category, List<MemberItem> list)
        {
            m_CategoryMap[category] = list;
        }

        private readonly Dictionary<string, List<MemberItem>> m_CategoryMap;

        internal void Append(ClassMemberCategories other)
        {
            foreach (var category in m_CategoryMap.Keys)
            {
                //don't append ancestors' implemented interfaces, that's already done in the UnderlyingModel
                if (category == "ImplementedInterfaces")
                    continue;
                m_CategoryMap[category].AddRange(other.m_CategoryMap[category]);
            }
        }

        public void Sort()
        {
            //the map gets modified during iteration, save the keys
            var saveKeys = new string[m_CategoryMap.Keys.Count];
            m_CategoryMap.Keys.CopyTo(saveKeys, 0);

            foreach (var category in saveKeys)
            {
                SortMemberCategory(category);
            }
        }

        internal void CleanMembersWithKnownSigs(List<string> leafSigs)
        {
            RemoveItemsWithKnownSignatures(m_CategoryMap["Messages"], leafSigs);
            RemoveItemsWithKnownSignatures(m_CategoryMap["ProtectedFunctions"], leafSigs);
            RemoveItemsWithKnownSignatures(m_CategoryMap["StaticFunctions"], leafSigs);
            RemoveItemsWithKnownSignatures(m_CategoryMap["RegularFunctions"], leafSigs);
        }

        internal List<string> GetKnownSigs()
        {
            var stuffWithSignatures = new List<MemberItem>();
            stuffWithSignatures.AddRange(m_CategoryMap["Messages"]);
            stuffWithSignatures.AddRange(m_CategoryMap["ProtectedFunctions"]);
            stuffWithSignatures.AddRange(m_CategoryMap["StaticFunctions"]);
            stuffWithSignatures.AddRange(m_CategoryMap["RegularFunctions"]);

            var leafSigs = new List<string>(); //signatures that exist in the leaf class - exclude them from ancestors
            foreach (var stuff in stuffWithSignatures)
                leafSigs.AddRange(stuff.Signatures.Select(m => m.Name));
            return leafSigs;
        }

        private static void RemoveItemsWithKnownSignatures(List<MemberItem> list, List<string> excludeSigs)
        {
            var removeList = new List<MemberItem>();
            foreach (var mes in list)
                if (mes.Signatures.Any(m => excludeSigs.Contains(m.Name)))
                    removeList.Add(mes);
            foreach (var del in removeList)
                list.Remove(del);
        }

        private void SortMemberCategory(string categoryName)
        {
            if (m_CategoryMap.ContainsKey(categoryName) && m_CategoryMap[categoryName] != null)
                m_CategoryMap[categoryName] = m_CategoryMap[categoryName].OrderBy(m => m.DisplayName).ToList();
        }

        internal List<XElement> GetXML(Func<MemberItem, XElement> formMemberFunction, bool eliminateConstructors = false)
        {
            var modelElement = new List<XElement>
            {
                new XElement("StaticVars", m_CategoryMap["StaticFieldAndProps"].Select(formMemberFunction)),
                new XElement("Vars", m_CategoryMap["RegularFieldsAndProps"].Select(formMemberFunction)),
                new XElement("ProtVars", m_CategoryMap["ProtectedVars"].Select(formMemberFunction)),
                new XElement("StaticProtVars", m_CategoryMap["StaticProtectedVars"].Select(formMemberFunction)),
                new XElement("Constructors", eliminateConstructors ? new List<XElement>() : m_CategoryMap["Constructors"].Select(formMemberFunction)),
                new XElement("MemberFunctions", m_CategoryMap["RegularFunctions"].Select(formMemberFunction)),
                CreateSimpleCategoryXElement("ProtectedFunctions", formMemberFunction),
                CreateSimpleCategoryXElement("StaticFunctions", formMemberFunction),
                CreateSimpleCategoryXElement("Operators", formMemberFunction),
                CreateSimpleCategoryXElement("Messages", formMemberFunction),
                CreateSimpleCategoryXElement("Events", formMemberFunction),
                CreateSimpleCategoryXElement("Delegates", formMemberFunction),
            };
            var ii = m_CategoryMap["ImplementedInterfaces"];
            if (ii != null && ii.Count > 0)
            {
                modelElement.Add(new XElement("ImplementedInterfaces", ii.Select(formMemberFunction)));
            }
            return modelElement;
        }

        private XElement CreateSimpleCategoryXElement(string category, Func<MemberItem, XElement> formMemberFunction)
        {
            return new XElement(category, m_CategoryMap[category].Select(formMemberFunction));
        }
    }
}
