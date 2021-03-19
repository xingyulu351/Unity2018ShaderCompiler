using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using UnderlyingModel;

// here we populate history from LatestAPI.mem, forming history.xml
// not to be confused with HistoryGenerator, which actually updates LatestAPI.mem, based on DLLs from adjacent versions.
namespace HistoryGenerator
{
    public class HistoryPopulator
    {
        private readonly IMemberGetter m_MemberItemProject;

        public HistoryPopulator(IMemberGetter proj)
        {
            m_MemberItemProject = proj;
        }

        private static string MemberIDFromTxtLine(string name, string owner, AssemblyType asmType)
        {
            string memberID = string.Format("{0}.{1}", owner, name);
            if (asmType == AssemblyType.Constructor)
                memberID = owner + ".ctor";
            else if (AssemblyTypeUtils.CanHaveChildren(asmType))
                memberID = name;
            else if (asmType == AssemblyType.Property || asmType == AssemblyType.Field || asmType == AssemblyType.EnumValue)
                memberID = string.Format("{0}.{1}", owner, StringConvertUtils.LowerCaseNeedsUnderscore(name));
            return memberID;
        }

        //one-shot: for decyphering UnityAPI.txt's syntax into memberIDs and AssemblyTypes
        public List<APIVersionEntry> PopulateAPIVersionEntriesFromTxt(string unityAPIPath)
        {
            var items = new List<APIVersionEntry>();

            var reader = new StreamReader(unityAPIPath);

            string line;

            while ((line = reader.ReadLine()) != null)
            {
                var entry = ConvertTxtToMemLine(line);
                if (items.All(m => m.MemberID != entry.MemberID))
                    items.Add(entry);
            }
            return items;
        }

        public List<APIVersionEntry> PopulateHistoryFromMem(string historyMemFile)
        {
            var items = new List<APIVersionEntry>();

            var reader = new StreamReader(historyMemFile);
            string line;
            while ((line = reader.ReadLine()) != null)
            {
                var entry = new APIVersionEntry();

                var bodyAndVersion = line.Split(new[] {"|||"}, StringSplitOptions.None);
                var body = bodyAndVersion[0];

                var fields = body.Split(new[] {'|'}); //note, empty entries are possible, but the structure is always the same
                var asmAndStaticString = fields[1].Trim();
                entry.MemberID = fields[0].Trim();

                entry.IsStatic = asmAndStaticString.Contains("(static)");
                asmAndStaticString = asmAndStaticString.Replace("(static)", "").Trim();

                entry.AssemblyType = AssemblyTypeUtils.AssemblyTypeFromString(asmAndStaticString);

                entry.Namespace = fields[2].Trim();
                entry.VersionAdded = new VersionNumber(bodyAndVersion[1]);

                items.Add(entry);
            }
            return items;
        }

        public APIVersionEntry ConvertTxtToMemLine(string line)
        {
            var whitespaceChars = new[] { ' ', '\t' };

            string[] fields = line.Split(whitespaceChars, 2);

            var entry = new APIVersionEntry
            {
                AssemblyType = AssemblyType.Unknown,
                IsStatic = false
            };

            var candidateType = GetAssemblyTypeFromString(fields[0]);
            entry.IsStatic = fields[0] == "staticprop" || fields[0] == "staticmethod";
            string[] bodyAndVersion = fields[1].Split(new[] { "|||" }, StringSplitOptions.None);

            string[] nameAndOwner = bodyAndVersion[0].Split(new[] {' '}, StringSplitOptions.RemoveEmptyEntries);
            var owner = nameAndOwner[0];
            var name = owner;
            if (!AssemblyTypeUtils.CanHaveChildren(candidateType) && nameAndOwner.Length > 1)
                name = nameAndOwner[1];
            entry.VersionAdded = new VersionNumber(bodyAndVersion[1]);

            var candidateID = MemberIDFromTxtLine(name, owner, candidateType);
            var memberItem = m_MemberItemProject.GetMember(candidateID);

            if (memberItem != null)
            {
                entry.MemberID = candidateID;
                entry.Namespace = memberItem.ActualNamespace;
                var probablyDelegate = memberItem.RealOrSuggestedItemType == AssemblyType.Delegate
                    || memberItem.RealOrSuggestedItemType == AssemblyType.Method && candidateID.Contains(".On")
                    || memberItem.RealOrSuggestedItemType == AssemblyType.Unknown && candidateID.Contains(".On");
                entry.AssemblyType = memberItem.ItemType == AssemblyType.Unknown ? candidateType : memberItem.ItemType;
                if (entry.Namespace.IsEmpty() && memberItem.ItemType == AssemblyType.Unknown && candidateID.Contains('.'))
                {
                    var parentID = candidateID.Substring(0, candidateID.LastIndexOf('.'));
                    var parentItem = m_MemberItemProject.GetMember(parentID);
                    if (parentItem != null)
                        entry.Namespace = parentItem.ActualNamespace;
                }

                if (probablyDelegate)
                    entry.AssemblyType = AssemblyType.Delegate;
            }
            else
            {
                entry.AssemblyType = candidateType;
                var potentialTypes = new List<AssemblyType> {candidateType};
                if (entry.AssemblyType == AssemblyType.Class)
                    potentialTypes.Add(AssemblyType.Interface); //look for interfaces too

                List<MemberItem> potentialMatches;

                if (AssemblyTypeUtils.CanHaveChildren(candidateType) || candidateType == AssemblyType.Interface)
                {
                    potentialMatches = m_MemberItemProject.GetAllMembers().
                        Where(m =>
                            potentialTypes.Contains(m.ItemType) &&
                            m.GetNames().ClassName == candidateID).
                        ToList();
                }
                else
                {
                    if (candidateType == AssemblyType.Method && candidateID.Contains("operator"))
                    {
                        bool isImplOp = nameAndOwner.Count() > 3 && nameAndOwner[3] == "implicit";
                        candidateID = !isImplOp
                            ? string.Format("{0}.op_{1}", owner, StringConvertUtils.ConvertOperator(nameAndOwner[2]))
                                : string.Format("{0}.implop_{1}({2})", owner, nameAndOwner[5], nameAndOwner[6].TrimStart(new[] {'('}));
                        potentialTypes.Add(AssemblyType.Operator);
                        potentialTypes.Add(AssemblyType.ImplOperator);
                        potentialMatches = m_MemberItemProject.GetAllMembers().
                            Where(m =>
                                potentialTypes.Contains(m.ItemType) &&
                                m.ItemName.Contains(candidateID))
                            .ToList();
                    }
                    else
                    {
                        potentialMatches = m_MemberItemProject.GetAllMembers().
                            Where(m =>
                                potentialTypes.Contains(m.ItemType) &&
                                m.GetNames().ClassAndMember == candidateID)
                            .ToList();
                    }
                }

                if (potentialMatches.Count() == 1)
                {
                    MemberItem item = potentialMatches.First();
                    memberItem = m_MemberItemProject.GetMember(item.ItemName);
                    if (memberItem != null)
                    {
                        entry.AssemblyType = memberItem.ItemType;
                        entry.MemberID = memberItem.GetNames().FullName;
                        entry.Namespace = memberItem.ActualNamespace;
                    }
                    else
                    {
                        Console.WriteLine("Could not find a memberItem for {0}", item.ItemName);
                    }
                }
                else
                {
                    entry.MemberID = candidateID;
                }
            }
            return entry;
        }

        //this function uses names specified in the old UnityAPI.txt, not generalizable to other cases
        private AssemblyType GetAssemblyTypeFromString(string assType)
        {
            var asmType = AssemblyType.Unknown;

            switch (assType)
            {
                case "class": asmType = AssemblyType.Class; break;
                case "constructor": asmType = AssemblyType.Constructor; break;
                case "struct": asmType = AssemblyType.Struct; break;
                case "enum": asmType = AssemblyType.Enum; break;
                case "enumval": asmType = AssemblyType.EnumValue; break;
                case "method": asmType = AssemblyType.Method; break;
                case "overmethod": asmType = AssemblyType.Method; break;
                case "prop": asmType = AssemblyType.Property; break;
                case "staticmethod": asmType = AssemblyType.Method; break;
                case "staticprop": asmType = AssemblyType.Property; break;
            }
            return asmType;
        }
    }
}
