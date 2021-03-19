using System.Collections.Generic;
using System.Linq;
using Mono.Cecil;

namespace UnderlyingModel
{
    public class AssemblyHierarchyTemp
    {
        private Dictionary<string, List<string>> m_ClassToImplementedInterfacesMap;
        private Dictionary<string, string> m_ClassToParentMap;
        private Dictionary<string, string> m_ClassToContainerMap;

        public AssemblyHierarchyTemp()
        {
            m_ClassToImplementedInterfacesMap = new Dictionary<string, List<string>>();
            m_ClassToParentMap = new Dictionary<string, string>();
            m_ClassToContainerMap = new Dictionary<string, string>();
        }

        internal void PopulateContainerMap(TypeDefinition typeDefinition, string itemName)
        {
            if (typeDefinition.DeclaringType == null)
                return;
            var declaringType = typeDefinition.DeclaringType;
            if (!m_ClassToContainerMap.ContainsKey(itemName))
                m_ClassToContainerMap[itemName] = CecilHelpers.GetTypeName(declaringType);
        }

        internal void PopulateInterfacesAndParentsMap(TypeDefinition typeDefinition, string itemName)
        {
            if (typeDefinition.HasInterfaces)
            {
                if (!m_ClassToImplementedInterfacesMap.ContainsKey(itemName))
                {
                    var interfaceNamesList = typeDefinition.Interfaces.Select(m => CecilHelpers.GetTypeName(m.InterfaceType)).ToList();
                    m_ClassToImplementedInterfacesMap[itemName] = interfaceNamesList;
                }
            }
            if (typeDefinition.BaseType != null)
            {
                var baseType = typeDefinition.BaseType;
                if (!CecilHelpers.IsSystemObject(baseType) && !m_ClassToParentMap.ContainsKey(itemName))
                {
                    m_ClassToParentMap[itemName] = CecilHelpers.GetTypeName(baseType);
                }
                else
                    m_ClassToParentMap[itemName] = typeDefinition.Module.Assembly.Name.Name;
            }
        }

        internal void PopulateParentsContainersAndInterfaces(NewDataItemProject project)
        {
            PopulateParentClasses(project);
            PopulateContainerClasses(project);
            PopulateInterfaces(project);
        }

        private void PopulateInterfaces(NewDataItemProject project)
        {
            foreach (var clName in m_ClassToImplementedInterfacesMap.Keys)
            {
                if (m_ClassToImplementedInterfacesMap == null || !m_ClassToImplementedInterfacesMap.ContainsKey(clName))
                    continue;

                var classItem = project.GetMember(clName);
                classItem.ImplementedInterfaces = new List<MemberItem>();

                var interfaceNamesList = m_ClassToImplementedInterfacesMap[clName];
                AddInterfacesFromClass(project, interfaceNamesList, classItem);
                var parentItem = classItem.ParentMember;
                while (parentItem != null)
                {
                    if (m_ClassToImplementedInterfacesMap != null && m_ClassToImplementedInterfacesMap.ContainsKey(parentItem.ItemName))
                    {
                        var parentInterfaceList = m_ClassToImplementedInterfacesMap[parentItem.ItemName];
                        AddInterfacesFromClass(project, parentInterfaceList, classItem);
                    }
                    parentItem = parentItem.ParentMember;
                }
            }
        }

        private void AddInterfacesFromClass(NewDataItemProject project, List<string> interfaceNamesList, MemberItem classItem)
        {
            foreach (var interfaceName in interfaceNamesList)
            {
                var interfaceItem = project.GetMember(interfaceName);
                if (interfaceItem != null)
                {
                    classItem.ImplementedInterfaces.Add(interfaceItem);
                }
            }
        }

        private void PopulateParentClasses(NewDataItemProject project)
        {
            foreach (var clName in m_ClassToParentMap.Keys)
            {
                var classItem = project.GetMember(clName);
                var parentName = m_ClassToParentMap[clName];
                var parentItem = project.GetMember(parentName);
                if (parentItem != null)
                {
                    classItem.ParentMember = parentItem;
                }
            }
        }

        private void PopulateContainerClasses(NewDataItemProject project)
        {
            foreach (var clName in m_ClassToContainerMap.Keys)
            {
                var classItem = project.GetMember(clName);
                var containerClassName = m_ClassToContainerMap[clName];
                var containerItem = project.GetMember(containerClassName);
                if (containerItem != null)
                {
                    classItem.ContainerClassMember = containerItem;
                    if (classItem.AnyHaveDoc)
                        containerItem.AnyNestedChildrenHaveDoc = true;
                }
            }
        }
    }
}
