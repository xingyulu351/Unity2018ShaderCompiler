using System;
using System.Text;
using System.Collections.Generic;
using System.IO;
using System.Web;

public class UnityProfileError
{
    string description;
    public string Description
    {
        get { return description; }
    }

    public UnityProfileError(string description)
    {
        this.description = description;
    }
}

public class UnityProfilesDocumentation
{
    Dictionary<string, DocumentedNamespace> namespaces;

    public DocumentedNamespace[] Namespaces
    {
        get
        {
            DocumentedNamespace[] result = new DocumentedNamespace[namespaces.Count];
            namespaces.Values.CopyTo(result, 0);
            return result;
        }
    }

    public DocumentedNamespace FindNamespace(string name)
    {
        if (namespaces.ContainsKey(name))
        {
            return namespaces[name];
        }
        else
        {
            return null;
        }
    }

    public DocumentedNamespace AddNamespace(string name)
    {
        if (namespaces.ContainsKey(name))
        {
            return namespaces[name];
        }
        else
        {
            DocumentedNamespace result = new DocumentedNamespace(name, this);
            namespaces[name] = result;
            return result;
        }
    }

    List<UnityProfileError> errors;
    public bool HasErrors
    {
        get { return errors.Count != 0; }
    }

    public UnityProfileError[] Errors
    {
        get
        {
            UnityProfileError[] result = new UnityProfileError[errors.Count];
            errors.CopyTo(result, 0);
            return result;
        }
    }

    public void AddError(string description)
    {
        errors.Add(new UnityProfileError(description));
    }

    public UnityProfilesDocumentation()
    {
        namespaces = new Dictionary<string, DocumentedNamespace>();
        errors = new List<UnityProfileError>();
    }
}
