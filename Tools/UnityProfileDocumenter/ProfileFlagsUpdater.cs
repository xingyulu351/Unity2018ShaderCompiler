using System;
using System.Collections.Generic;
using Mono.Cecil;

public class ProfileFlagsUpdater : IAssemblyApiVisitor
{
    UnityProfilesDocumentation documentation;
    UnityProfiles profile;

    string currentAssembly;
    DocumentedNamespace currentNamespace;
    DocumentedClass currentClass;
    List<DocumentedClass> currentNestedClasses;

    public ProfileFlagsUpdater(UnityProfilesDocumentation documentation, UnityProfiles profile)
    {
        this.documentation = documentation;
        this.profile = profile;
        currentAssembly = null;
        currentNamespace = null;
        currentClass = null;
        currentNestedClasses = new List<DocumentedClass>();
    }

    public void ModuleStart(string moduleName)
    {
        currentAssembly = moduleName;
    }

    public void ModuleEnd()
    {
        currentAssembly = null;
    }

    public void TypeStart(bool isValueType, string nameSpace, string name)
    {
        currentNamespace = documentation.FindNamespace(nameSpace);
        if (currentNamespace != null)
        {
            currentClass = currentNamespace.FindClass(name);
            if (currentClass != null)
            {
                currentClass.AddSupportedProfile(profile);
            }
            else
            {
                documentation.AddError(String.Format("Extra class {0}.{1} in profile {2} (module {3})", nameSpace, name, profile, currentAssembly));
            }
        }
        else
        {
            documentation.AddError(String.Format("Extra namespace {0} in profile {1}", nameSpace, profile));
        }
    }

    public void TypeEnd()
    {
        currentNamespace = null;
        currentClass = null;
    }

    public void NestedTypeStart(bool isValueType, string name)
    {
        string nestedClassName = currentClass.KeyName + "." + name;
        if (currentClass != null)
        {
            currentClass = currentNamespace.AddClass(nestedClassName, currentAssembly, isValueType);
            if (currentClass == null)
            {
                currentClass.AddSupportedProfile(profile);
            }
            else
            {
                documentation.AddError(String.Format("Extra nested class {0} in class {1} {2}", nestedClassName, profile));
            }
        }
        currentNestedClasses.Add(currentClass);
        currentClass.AddSupportedProfile(UnityProfiles.Base);
    }

    public void NestedTypeEnd()
    {
        currentClass = currentNestedClasses[currentNestedClasses.Count - 1];
        currentNestedClasses.Remove(currentClass);
    }

    public void Field(string keyName, string typeDocName, string docName)
    {
        if (currentClass != null)
        {
            DocumentedMember member = currentClass.FindMember(keyName);
            if (member != null)
            {
                if (member is DocumentedField)
                {
                    member.AddSupportedProfile(profile);
                }
                else
                {
                    documentation.AddError(String.Format("Member \"{0}\" in class {1}.{2} in profile {3} is not a field", keyName, currentClass.ClassNameSpace.Name, currentClass.KeyName, profile));
                }
            }
            else
            {
                documentation.AddError(String.Format("Extra field \"{0}\" in class {1}.{2} in profile {3}", keyName, currentClass.ClassNameSpace.Name, currentClass.KeyName, profile));
            }
        }
    }

    public void Property(string keyName, string typeDocName, string docName, bool hasGetter, bool hasSetter)
    {
        if (currentClass != null)
        {
            DocumentedMember member = currentClass.FindMember(keyName);
            if (member != null)
            {
                if (member is DocumentedProperty)
                {
                    DocumentedProperty property = (DocumentedProperty)member;
                    if (property.HasGetter != hasGetter)
                    {
                        documentation.AddError(String.Format("Property \"{0}\" in class {1}.{2} in profile {3} has getter {4} instead of {5}", keyName, currentClass.ClassNameSpace.Name, currentClass.KeyName, profile, property.HasGetter, hasGetter));
                    }
                    if (property.HasSetter != hasSetter)
                    {
                        documentation.AddError(String.Format("Property \"{0}\" in class {1}.{2} in profile {3} has setter {4} instead of {5}", keyName, currentClass.ClassNameSpace.Name, currentClass.KeyName, profile, property.HasGetter, hasGetter));
                    }

                    member.AddSupportedProfile(profile);
                }
                else
                {
                    documentation.AddError(String.Format("Member \"{0}\" in class {1}.{2} in profile {3} is not a property", keyName, currentClass.ClassNameSpace.Name, currentClass.KeyName, profile));
                }
            }
            else
            {
                documentation.AddError(String.Format("Extra property \"{0}\" in class {1}.{2} in profile {3}", keyName, currentClass.ClassNameSpace.Name, currentClass.KeyName, profile));
            }
        }
    }

    public void Method(string keyName, string returnTypeDocName, string docName, string docArgumentsRepresentation)
    {
        if (currentClass != null)
        {
            DocumentedMember member = currentClass.FindMember(keyName);
            if (member != null)
            {
                if (member is DocumentedMethod)
                {
                    member.AddSupportedProfile(profile);
                }
                else
                {
                    documentation.AddError(String.Format("Member \"{0}\" in class {1}.{2} in profile {3} is not a method", keyName, currentClass.ClassNameSpace.Name, currentClass.KeyName, profile));
                }
            }
            else
            {
                documentation.AddError(String.Format("Extra method \"{0}\" in class {1}.{2} in profile {3}", keyName, currentClass.ClassNameSpace.Name, currentClass.KeyName, profile));
            }
        }
    }
}
