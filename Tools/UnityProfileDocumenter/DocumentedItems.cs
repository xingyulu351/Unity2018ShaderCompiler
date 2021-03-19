using System;
using System.Collections.Generic;
using System.IO;
using System.Text;
using System.Text.RegularExpressions;
using System.Web;


public abstract class DocumentedItem
{
    string docName;
    public string DocName
    {
        get { return docName; }
    }

    string keyName;
    public string KeyName
    {
        get { return keyName; }
    }

    public abstract string BuildExternalDocumentationUrl(ExternalDocumentationUrlBuilder builder);

    UnityProfiles supportedProfiles;
    public UnityProfiles SupportedProfiles
    {
        get { return supportedProfiles; }
    }
    public void AddSupportedProfile(UnityProfiles profile)
    {
        profile.AddTo(ref supportedProfiles);
    }

    public DocumentedItem(string docName, string keyName)
    {
        this.docName = docName;
        this.keyName = keyName;
    }
}

public abstract class DocumentedMember : DocumentedItem
{
    DocumentedClass parent;
    public DocumentedClass Parent
    {
        get { return parent; }
    }

    public abstract string DocPrefix {get; }
    public abstract string DocSuffix {get; }

    public DocumentedMember(string docName, string keyName, DocumentedClass parent) : base(docName, keyName)
    {
        this.parent = parent;
    }
}

public class DocumentedField : DocumentedMember
{
    string typeName;
    public string TypeName
    {
        get { return typeName; }
    }

    public override string DocPrefix
    {
        get
        {
            if (!String.IsNullOrEmpty(typeName))
                return typeName + " ";
            else
                return String.Empty;
        }
    }

    public override string DocSuffix
    {
        get
        {
            return String.Empty;
        }
    }

    public override string BuildExternalDocumentationUrl(ExternalDocumentationUrlBuilder builder)
    {
        return builder.FieldUrl(Parent.ClassNameSpace.Name, Parent.DocName, DocName);
    }

    public DocumentedField(string docName, string keyName, string typeName, DocumentedClass parent) : base(docName, keyName, parent)
    {
        this.typeName = typeName;
    }
}

public class DocumentedMethod : DocumentedMember
{
    string returnTypeName;
    public string ReturnTypeName
    {
        get { return returnTypeName; }
    }

    string argumentsRepresentation;
    public string ArgumentsRepresentation
    {
        get { return argumentsRepresentation; }
    }

    public override string DocPrefix
    {
        get
        {
            if (!String.IsNullOrEmpty(returnTypeName))
                return returnTypeName + " ";
            else
                return String.Empty;
        }
    }

    public override string DocSuffix
    {
        get
        {
            if (!String.IsNullOrEmpty(argumentsRepresentation))
                return " " + argumentsRepresentation;
            else
                return String.Empty;
        }
    }

    public override string BuildExternalDocumentationUrl(ExternalDocumentationUrlBuilder builder)
    {
        if (String.IsNullOrEmpty(returnTypeName))
            return builder.ConstructorUrl(Parent.ClassNameSpace.Name, Parent.DocName, DocName);
        else
            return builder.MethodUrl(Parent.ClassNameSpace.Name, Parent.DocName, DocName);
    }

    public DocumentedMethod(string docName, string keyName, string returnTypeName, string argumentsRepresentation, DocumentedClass parent) : base(docName, keyName, parent)
    {
        this.returnTypeName = returnTypeName;
        this.argumentsRepresentation = argumentsRepresentation;
    }
}

public class DocumentedProperty : DocumentedMember
{
    string typeName;
    public string TypeName
    {
        get { return typeName; }
    }

    bool hasGetter;
    public bool HasGetter
    {
        get
        {
            return hasGetter;
        }
    }

    bool hasSetter;
    public bool HasSetter
    {
        get
        {
            return hasSetter;
        }
    }

    public override string DocPrefix
    {
        get
        {
            if (!String.IsNullOrEmpty(typeName))
                return typeName + " ";
            else
                return String.Empty;
        }
    }

    public override string DocSuffix
    {
        get
        {
            StringBuilder sb = new StringBuilder();
            sb.Append(" {");
            if (hasGetter)
                sb.Append(" get;");
            if (hasSetter)
                sb.Append(" set;");
            sb.Append(" }");
            return sb.ToString();
        }
    }

    public override string BuildExternalDocumentationUrl(ExternalDocumentationUrlBuilder builder)
    {
        return builder.PropertyUrl(Parent.ClassNameSpace.Name, Parent.DocName, DocName);
    }

    public DocumentedProperty(string docName, string keyName, string typeName, DocumentedClass parent, bool hasGetter, bool hasSetter) : base(docName, keyName, parent)
    {
        this.typeName = typeName;
        this.hasGetter = hasGetter;
        this.hasSetter = hasSetter;
    }
}

public class DocumentedClass : DocumentedItem
{
    Dictionary<string, DocumentedMember> members;
    public DocumentedMember[] Members
    {
        get
        {
            DocumentedMember[] result = new DocumentedMember[members.Count];
            members.Values.CopyTo(result, 0);
            return result;
        }
    }

    bool isValueType;
    public bool IsValueType
    {
        get
        {
            return isValueType;
        }
    }

    public override string BuildExternalDocumentationUrl(ExternalDocumentationUrlBuilder builder)
    {
        if (isValueType)
            return builder.StructUrl(classNameSpace.Name, DocName);
        else
            return builder.ClassUrl(classNameSpace.Name, DocName);
    }

    public DocumentedMember FindMember(string keyName)
    {
        if (members.ContainsKey(keyName))
        {
            return members[keyName];
        }
        else
        {
            return null;
        }
    }

    public DocumentedField AddField(string docName, string keyName, string typeName)
    {
        if (members.ContainsKey(keyName))
        {
            return (DocumentedField)members[keyName];
        }
        else
        {
            DocumentedField result = new DocumentedField(docName, keyName, typeName, this);
            members[keyName] = result;
            return result;
        }
    }

    public DocumentedMethod AddMethod(string docName, string keyName, string returnTypeName, string argumentsRepresentation)
    {
        if (members.ContainsKey(keyName))
        {
            return (DocumentedMethod)members[keyName];
        }
        else
        {
            DocumentedMethod result = new DocumentedMethod(docName, keyName, returnTypeName, argumentsRepresentation, this);
            members[keyName] = result;
            return result;
        }
    }

    public DocumentedProperty AddProperty(string docName, string keyName, string typeName, bool hasGetter, bool hasSetter)
    {
        if (members.ContainsKey(keyName))
        {
            return (DocumentedProperty)members[keyName];
        }
        else
        {
            DocumentedProperty result = new DocumentedProperty(docName, keyName, typeName, this, hasGetter, hasSetter);
            members[keyName] = result;
            return result;
        }
    }

    DocumentedNamespace classNameSpace;
    public DocumentedNamespace ClassNameSpace
    {
        get { return classNameSpace; }
    }

    string assembly;
    public string Assembly
    {
        get { return assembly; }
    }

    public DocumentedClass(string docName, string keyName, DocumentedNamespace classNameSpace, string assembly, bool isValueType) : base(docName, keyName)
    {
        this.classNameSpace = classNameSpace;
        members = new Dictionary<string, DocumentedMember>();
        this.assembly = assembly;
        this.isValueType = isValueType;
    }
}

public class DocumentedNamespace
{
    string name;
    public string Name
    {
        get { return name; }
    }

    Dictionary<string, DocumentedClass> classes;
    public DocumentedClass[] Classes
    {
        get
        {
            DocumentedClass[] result = new DocumentedClass[classes.Count];
            classes.Values.CopyTo(result, 0);
            return result;
        }
    }

    public DocumentedClass FindClass(string name)
    {
        if (classes.ContainsKey(name))
        {
            return classes[name];
        }
        else
        {
            return null;
        }
    }

    public DocumentedClass AddClass(string name, string assembly, bool isValueType)
    {
        if (classes.ContainsKey(name))
        {
            return classes[name];
        }
        else
        {
            DocumentedClass result = new DocumentedClass(name, name, this, assembly, isValueType);
            classes[name] = result;
            return result;
        }
    }

    public string BuildExternalDocumentationUrl(ExternalDocumentationUrlBuilder builder)
    {
        return builder.NameSpaceUrl(name);
    }

    UnityProfilesDocumentation documentation;
    public UnityProfilesDocumentation Documentation
    {
        get { return documentation; }
    }

    public DocumentedNamespace(string name, UnityProfilesDocumentation documentation)
    {
        this.name = name;
        this.documentation = documentation;
        classes = new Dictionary<string, DocumentedClass>();
    }
}
