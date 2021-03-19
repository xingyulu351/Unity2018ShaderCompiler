using System;
using System.Collections.Generic;
using System.Linq;
using Mono.Cecil;
using Mono.Collections.Generic;

namespace UnderlyingModel
{
    public partial class NewDataItemProject
    {
        private readonly List<string> m_DllLocations = new List<string>();
        private string[] m_AsmModuleNames;
        private bool m_ScanForPrivateMembers = false;
        private readonly List<TypeDefinition> m_FoundTypes = new List<TypeDefinition>();

        private string m_FullPathToCurrentDll;
        private readonly AssemblyHierarchyTemp m_AssemblyHierarchyTemp = new AssemblyHierarchyTemp();

        public string[] GetAsmModuleNames()
        {
            return m_AsmModuleNames;
        }

        private void LoadAsmModules(bool scanForNonPrivate, bool scanForPrivate)
        {
            if (m_AsmModuleNames == null)
            {
                m_AsmModuleNames = new string[m_DllLocations.Count];
            }
            for (int i = 0; i < m_DllLocations.Count; i++)
            {
                using (ModuleDefinition module = ModuleDefinition.ReadModule(m_DllLocations[i], new ReaderParameters { InMemory = false }))
                {
                    m_AsmModuleNames[i] = module.FileName;
                    if (scanForNonPrivate)
                    {
                        m_ScanForPrivateMembers = false;
                        PopulateFromDll(module);
                    }
                    if (scanForPrivate)
                    {
                        m_ScanForPrivateMembers = true;
                        PopulateFromDll(module);
                    }
                }
            }
        }

        private void PopulateFromDll(ModuleDefinition moduleDefinition)
        {
            Collection<TypeDefinition> moduleTypes = null;

            m_FullPathToCurrentDll = moduleDefinition.FileName;

            try
            {
                moduleTypes = moduleDefinition.Types;
            }
            catch (Exception rte)
            {
                Console.Out.WriteLine("ReflectionTypeLoadException:{0}", rte.Message);
            }

            var assembly = moduleDefinition.Assembly;
            var assemblyName = assembly.Name.Name;
            var entry = new AsmEntry(assemblyName, AssemblyType.Assembly, false, null, assembly);
            var parent = HandleMemberAndSignature(assemblyName, assemblyName, entry, null);

            foreach (TypeDefinition td in moduleTypes)
            {
                if (!m_FoundTypes.Contains(td))
                {
                    PopulateFromTypeDefinition(td, parent);
                    m_FoundTypes.Add(td);
                }
                else
                {
                    //Console.WriteLine("found duplicate type {0}", td);
                }
            }
        }

        private void PopulateFromTypeDefinition(TypeDefinition typeDefinition, MemberItem parent)
        {
            if (typeDefinition.IsPrimitive)
                return;

            if (CecilHelpers.IsNamespaceInternal(typeDefinition.Namespace))
                return;

            if (CecilHelpers.ExcludedFromDocs(typeDefinition))
                return;

            if (CecilHelpers.IsCompilerGenerated(typeDefinition))
                return;

            //bool documentIt = m_ScanForPrivateMembers || typeDefinition.IsPublic || typeDefinition.IsNestedPublic;

            //documentIt = documentIt && !CecilHelpers.IsObsolete(typeDefinition);

            bool documentIt = m_ScanForPrivateMembers || typeDefinition.IsPublic || typeDefinition.IsNestedPublic || typeDefinition.IsNestedFamily;

            if (!documentIt)
                return;

            try
            {
                if (CecilHelpers.IsDelegate(typeDefinition))
                    PopulateDelegate(typeDefinition, parent);
                else if (typeDefinition.IsEnum)
                    PopulateEnum(typeDefinition, parent);
                else if ((typeDefinition.IsClass || typeDefinition.IsValueType || typeDefinition.IsInterface) && !typeDefinition.IsPrimitive)
                    PopulateClassOrStruct(typeDefinition, parent);
                else
                    Console.Out.WriteLine("Unknown Item: {0}", typeDefinition);
            }
            catch
            {
                Console.WriteLine("Error populating from type {0}", typeDefinition);
                throw;
            }
        }

        private void PopulateEnum(TypeDefinition typeDefinition, MemberItem parent)
        {
            var asm = new AsmEntry(typeDefinition.Name, AssemblyType.Enum, typeDefinition.IsStatic(), typeDefinition.DeclaringType, typeDefinition);

            var memberItem = HandleMemberAndSignature(CecilHelpers.GetTypeName(typeDefinition), typeDefinition.Name, asm, parent);

            m_AssemblyHierarchyTemp.PopulateContainerMap(typeDefinition, memberItem.ItemName);

            PopulateEnumChildren(typeDefinition, memberItem);
        }

        private void PopulateEnumChildren(TypeDefinition enumTypeDef, MemberItem parentItem)
        {
            foreach (var field in enumTypeDef.Fields)
            {
                if (field.Name == "value__" || CecilHelpers.ExcludedFromDocs(field))
                    continue;

                var asm = new AsmEntry(field.Name, AssemblyType.EnumValue, field.IsStatic, enumTypeDef, field);
                HandleMemberAndSignature(GetMemberName(field), field.Name, asm, parentItem);
            }
        }

        private void PopulateClassOrStruct(TypeDefinition typeDefinition, MemberItem parent)
        {
            //workarounds for cases where a type was moved and yet its filename / id remains the same
            if (CecilHelpers.IsEditorSpritesDataUtility(typeDefinition) || CecilHelpers.IsAssetModificationProcessor(typeDefinition))
            {
                return;
            }
            var asmType = AssemblyType.Unknown;

            //delegates are seen as Class by Cecil, but we don't handle them here
            if (CecilHelpers.IsDelegate(typeDefinition))
                PopulateDelegate(typeDefinition, null);
            else if (typeDefinition.IsClass)
                asmType = typeDefinition.IsValueType ? AssemblyType.Struct : AssemblyType.Class;
            else if (typeDefinition.IsInterface)
                asmType = AssemblyType.Interface;

            var asm = new AsmEntry(typeDefinition.Name, asmType, typeDefinition.IsStatic(), typeDefinition.DeclaringType, typeDefinition);
            var memberItem = HandleMemberAndSignature(CecilHelpers.GetTypeName(typeDefinition), typeDefinition.Name, asm, parent);

            var itemName = memberItem.ItemName;
            m_AssemblyHierarchyTemp.PopulateInterfacesAndParentsMap(typeDefinition, itemName);
            m_AssemblyHierarchyTemp.PopulateContainerMap(typeDefinition, itemName);
            PopulateClassOrStructChildren(typeDefinition, memberItem);
        }

        private void PopulateClassOrStructChildren(TypeDefinition typeDefinition, MemberItem parentItem)
        {
            PopulateNestedTypes(typeDefinition, parentItem);
            PopulateMethods(typeDefinition, parentItem);
            PopulateProperties(typeDefinition, parentItem);
            PopulateFields(typeDefinition, parentItem);
        }

        private void PopulateFields(TypeDefinition cecilType, MemberItem parentItem)
        {
            foreach (var field in cecilType.Fields)
            {
                bool documentIt = m_ScanForPrivateMembers || cecilType.IsPublic || cecilType.IsNestedPublic || cecilType.IsNestedFamily;
                //documentIt = (field.Name[1] == '_') ? false : documentIt;
                if (!documentIt || !field.IsPublic || CecilHelpers.ExcludedFromDocs(field))
                    continue;

                //if(Protected)
                //    Console.WriteLine(string.Format("Detected a protected field by the name of {0} in {1}, total fields now {2}", field.Name, parentItem.ItemName, protected_m_count++));

                var asm = new AsmEntry(field.Name, AssemblyType.Field, field.IsStatic, cecilType, field)
                {
                    ReturnType = field.FieldType.ToString().SimplifyTypes()
                };
                asm.IsProtected = !field.IsPrivate && !field.IsPublic;
                // Console.WriteLine("adding a protected member "+field.Name);
                HandleMemberAndSignature(GetMemberName(field), field.Name, asm, parentItem);
            }
        }

        private void PopulateProperties(TypeDefinition cecilType, MemberItem parentItem)
        {
            foreach (PropertyDefinition prop in cecilType.Properties)
            {
                if (CecilHelpers.ExcludedFromDocs(prop))
                    continue;

                MethodDefinition method = (prop.GetMethod != null) ? prop.GetMethod : prop.SetMethod;
                if (method == null)
                    continue;

                bool isProtectedUnsealed = method.IsFamily;
                if (!m_ScanForPrivateMembers && !method.IsPublic && !isProtectedUnsealed)
                    continue;

                var signatureName = MemberNameGenerator.SignatureNameFromPropertyDefinition(prop);
                var asm = new AsmEntry(signatureName, AssemblyType.Property, prop.IsStatic(), cecilType, prop)
                {
                    ReturnType = prop.PropertyType.ToString().SimplifyTypes(),
                    IsOverride = CecilHelpers.IsOverride(method)
                };
                if (isProtectedUnsealed)
                    asm.IsProtectedUnsealed = true;
                string memberName = GetMemberName(prop);
                HandleMemberAndSignature(memberName, signatureName, asm, parentItem);
            }
        }

        private void PopulateMethods(TypeDefinition cecilType, MemberItem parentItem)
        {
            foreach (var method in cecilType.Methods)
            {
                //workaround for the case where the method name was changed, but it resolves to the same ID
                if (method.Name == "SetLODS" && method.DeclaringType.Name == "LODGroup")
                    continue;

                bool isProtectedUnsealed = method.IsFamily && !method.IsConstructOrDestruct();
                if (!m_ScanForPrivateMembers && !method.IsPublic)
                    if (!isProtectedUnsealed)
                        continue;

                if (method.IsGetter || method.IsSetter)
                    continue;

                if (CecilHelpers.ExcludedFromDocs(method))
                    continue;

                var asmType = AssemblyType.Method;

                var methodName = method.Name;
                if (method.IsConstructor)
                {
                    methodName = "constructor";
                    asmType = AssemblyType.Constructor;
                }
                else if (CecilHelpers.IsMethodImplop(method))
                {
                    methodName = method.ReturnType.ToString().SimplifyTypes();
                    asmType = AssemblyType.ImplOperator;
                }
                else if (CecilHelpers.IsMethodOp(method))
                {
                    methodName = StringConvertUtils.ConvertOperatorFromAssembly(methodName).Substring(3); // Skip the op_ part
                    asmType = AssemblyType.Operator;
                }
                else if (CecilHelpers.IsMethodEvent(method))
                {
                    if (method.Name.StartsWith("remove_"))
                        continue;
                    methodName = method.Name.Replace("add_", "");
                    asmType = AssemblyType.Event;
                }

                var asmEntry = new AsmEntry(methodName, asmType, method.IsStatic, method.DeclaringType, method)
                {
                    IsOverride = CecilHelpers.IsOverride(method)
                };
                if (isProtectedUnsealed)
                    asmEntry.IsProtectedUnsealed = true;

                try
                {
                    asmEntry.PopulateParametersAndReturnType(method);
                }
                catch
                {
                    Console.WriteLine("Error populating from method {0}", method);
                    throw;
                }

                var signatureName = MemberNameGenerator.SignatureNameFromMethodDefinition(method);
                HandleMemberAndSignature(GetMemberName(method), signatureName, asmEntry, parentItem);
            }
        }

        private void PopulateNestedTypes(TypeDefinition cecilType, MemberItem parentItem)
        {
            foreach (var nested in cecilType.NestedTypes)
            {
                bool isDelegate = CecilHelpers.IsDelegate(nested);
                if (isDelegate)
                {
                    //treat as a function, even though Cecil sees it as a class
                    PopulateDelegate(nested, parentItem);
                }
                else
                {
                    PopulateFromTypeDefinition(nested, parentItem);
                }
            }
        }

        private void PopulateDelegate(TypeDefinition del, MemberItem parentItem)
        {
            var asmEntry = new AsmEntry(del.Name, AssemblyType.Delegate, del.IsStatic(), del.DeclaringType, del);
            var invokeMethods = del.Methods.Where(m => m.Name == "Invoke").ToList();
            if (invokeMethods.Count() != 1)
                return;
            asmEntry.PopulateParametersAndReturnType(invokeMethods.First());
            var signatureName = MemberNameGenerator.SignatureNameFromDelegate(del);
            HandleMemberAndSignature(CecilHelpers.GetTypeName(del), signatureName, asmEntry, parentItem);
        }

        /// <summary>
        /// if item not in the map, create it.
        /// if signature not in the item, add it
        /// if signature doesn't have the assembly entry, add it
        /// add the item to parent if it doesn't already contain it
        /// </summary>
        /// <param name="itemName">item identifier</param>
        /// <param name="signatureName">signature for that item</param>
        /// <param name="asmEntry">assembly entry for this signature</param>
        /// <param name="parentItem">parent class item</param>
        /// <returns>The member item with the signature included</returns>
        private MemberItem HandleMemberAndSignature(string itemName, string signatureName, AsmEntry asmEntry, MemberItem parentItem)
        {
            itemName = CecilHelpers.Backticks2Underscores(itemName);
            asmEntry.Private = m_ScanForPrivateMembers;

            //HACK SPECIAL CASE
            if (SkipSpecialCaseMember(itemName))
                return null;

            // Create member item if one does not already exist
            if (!m_MapNameToItem.ContainsKey(itemName))
            {
                if (m_ScanForPrivateMembers)
                    return null;

                m_MapNameToItem[itemName] = new MemberItem(itemName, asmEntry.EntryType);
            }
            //else if (!m_ScanForPrivateMembers && !m_MapNameToItem[itemName].MultipleSignaturesPossible)
            //  Console.WriteLine ("Same member name found twice for {0} (multiple signatures should not be possible for this type)", itemName);

            // Get member item
            var item = m_MapNameToItem[itemName];

            // Create signature
            if (!item.ContainsSignature(signatureName))
            {
                var sigEntry = new SignatureEntry(signatureName) {Asm = asmEntry};
                item.AddSignature(sigEntry);
            }
            else
            {
                if (m_ScanForPrivateMembers)
                {
                    var sigEntry = item.GetSignature(signatureName, true);
                    if (!sigEntry.InAsm)
                        sigEntry.Asm = asmEntry;
                }
                //else
                //  Console.WriteLine("We should not see the same signature entry twice!! for {0} : {1}.", itemName, signatureName);
            }
            if (parentItem != null && !parentItem.ChildMembers.Contains(item))
                parentItem.ChildMembers.Add(item);

            item.AddDllPath(m_FullPathToCurrentDll);
            return item; //the item we just populated, may or may not be used
        }

        private string GetMemberName(MemberReference memberReference)
        {
            var methodDefinition = memberReference as MethodDefinition;
            string memberName = "";
            if (methodDefinition != null)
            {
                if (methodDefinition.IsConstructor)
                    memberName = "ctor";
                else if (CecilHelpers.IsMethodImplop(methodDefinition))
                    memberName = "implop_" + methodDefinition.ReturnType.ToString().SimplifyTypes() + "(" +
                        methodDefinition.Parameters[0].ParameterType.ToString().SimplifyTypes() + ")";
                else if (methodDefinition.Name.StartsWith("op_"))
                    memberName = StringConvertUtils.ConvertOperatorFromAssembly(memberReference.Name);
                else if (CecilHelpers.IsMethodEvent(methodDefinition))
                    memberName = StringConvertUtils.LowerCaseNeedsUnderscore(memberReference.Name.Replace("add_", ""));
                else
                    memberName = StringConvertUtils.LowerCaseNeedsUnderscore(memberReference.Name);
            }
            else
            {
                var memName = memberReference.Name;
                memberName = memName == "Item" ? "this" : StringConvertUtils.LowerCaseNeedsUnderscore(memName);
            }

            return CecilHelpers.GetTypeName(memberReference.DeclaringType) + "." + memberName;
        }

        private bool checkShouldDocumentMember(TypeDefinition cecilType, MemberItem parentItem = null)
        {
            //bool documentIt = m_ScanForPrivateMembers || cecilType.IsPublic || cecilType.IsNestedPublic || cecilType.IsNestedFamily || !field.IsPublic && !field.IsPrivate;
            return true;
        }
    }
}
