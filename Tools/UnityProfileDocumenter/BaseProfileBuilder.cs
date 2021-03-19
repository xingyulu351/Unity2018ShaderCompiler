using System;
using System.Collections.Generic;
//using Mono.Collections.Generic;
using Mono.Cecil;

public class BaseProfileBuilder : IAssemblyApiVisitor
{
    UnityProfilesDocumentation documentation;
    string currentAssembly;

    DocumentedNamespace currentNamespace;
    DocumentedClass currentClass;
    List<DocumentedClass> currentNestedClasses;

    public BaseProfileBuilder(UnityProfilesDocumentation documentation)
    {
        this.documentation = documentation;
        currentAssembly = null;
        currentNamespace = null;
        currentClass = null;
        currentNestedClasses = new List<DocumentedClass>();
    }

    void IAssemblyApiVisitor.ModuleStart(string moduleName)
    {
        currentAssembly = moduleName;
    }

    void IAssemblyApiVisitor.ModuleEnd()
    {
        currentAssembly = null;
    }

    void IAssemblyApiVisitor.TypeStart(bool isValueType, string nameSpace, string name)
    {
        currentNamespace = documentation.AddNamespace(nameSpace);
        currentClass = currentNamespace.AddClass(name, currentAssembly, isValueType);
        currentClass.AddSupportedProfile(UnityProfiles.Base);
    }

    void IAssemblyApiVisitor.TypeEnd()
    {
        currentNamespace = null;
        currentClass = null;
    }

    void IAssemblyApiVisitor.NestedTypeStart(bool isValueType, string name)
    {
        currentNestedClasses.Add(currentClass);
        currentClass = currentNamespace.AddClass(currentClass.KeyName + "." + name, currentAssembly, isValueType);
        currentClass.AddSupportedProfile(UnityProfiles.Base);
    }

    void IAssemblyApiVisitor.NestedTypeEnd()
    {
        currentClass = currentNestedClasses[currentNestedClasses.Count - 1];
        currentNestedClasses.Remove(currentClass);
    }

    public void Field(string keyName, string typeDocName, string docName)
    {
        currentClass.AddField(docName, keyName, typeDocName).AddSupportedProfile(UnityProfiles.Base);
    }

    public void Property(string keyName, string typeDocName, string docName, bool hasGetter, bool hasSetter)
    {
        currentClass.AddProperty(docName, keyName, typeDocName, hasGetter, hasSetter).AddSupportedProfile(UnityProfiles.Base);
    }

    public void Method(string keyName, string returnTypeDocName, string docName, string docArgumentsRepresentation)
    {
        currentClass.AddMethod(docName, keyName, returnTypeDocName, docArgumentsRepresentation).AddSupportedProfile(UnityProfiles.Base);
    }
}
