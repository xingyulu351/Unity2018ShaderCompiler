using System;
using System.IO;
using System.Collections.Generic;

public interface IDocumentationPrinter
{
    void StartDocumentation();
    void EndDocumentation();

    void StartNamespace(string name, string url);
    void EndNamespace();

    void StartClass(string name, string ns, string assembly, UnityProfiles profiles, string url);
    void EndClass();

    void StartMembers();
    void EndMembers();

    void StartMember(string name, UnityProfiles profiles, string url);
    void EndMember();
}


public static class DocumentationPrinter
{
    public static void Print(this UnityProfilesDocumentation doc, IDocumentationPrinter printer)
    {
        printer.StartDocumentation();

        foreach (DocumentedNamespace ns in doc.Namespaces)
        {
            printer.StartNamespace(ns.Name, ns.BuildExternalDocumentationUrl(MsdnSearchUrlBuilder.Instance));

            foreach (DocumentedClass c in ns.Classes)
            {
                DocumentedMember[] members = c.Members;

                if (members.Length > 0)
                {
                    printer.StartClass(c.DocName, c.ClassNameSpace.Name, c.Assembly, c.SupportedProfiles, c.BuildExternalDocumentationUrl(MsdnSearchUrlBuilder.Instance));
                    printer.StartMembers();
                    foreach (DocumentedMember member in members)
                    {
                        string memberDocRepresentation = member.DocPrefix + member.DocName + member.DocSuffix;
                        printer.StartMember(memberDocRepresentation, member.SupportedProfiles, member.BuildExternalDocumentationUrl(MsdnSearchUrlBuilder.Instance));
                        printer.EndMember();
                    }
                    printer.EndMembers();
                    printer.EndClass();
                }
            }

            printer.EndNamespace();
        }

        printer.EndDocumentation();
    }
}

public class DebugDumpDocumentation : IDocumentationPrinter
{
    public void StartDocumentation()
    {
        Console.WriteLine("Documentation START");
    }

    public void EndDocumentation()
    {
        Console.WriteLine("Documentation END");
    }

    public void StartNamespace(string name, string url)
    {
        Console.WriteLine("\tNamespace {0} START", name);
    }

    public void EndNamespace()
    {
        Console.WriteLine("\tNamespace END");
    }

    public void StartClass(string name, string ns, string assembly, UnityProfiles profiles, string url)
    {
        Console.WriteLine("\t\tClass {0} (assembly {1}, namespace {2}) (profiles {3}) START", name, assembly, ns, profiles);
    }

    public void EndClass()
    {
        Console.WriteLine("\t\tClass END");
    }

    public void StartMembers()
    {
    }

    public void EndMembers()
    {
    }

    public void StartMember(string name, UnityProfiles profiles, string url)
    {
        Console.WriteLine("\t\t\tMember {0} (profiles {1})", name, profiles);
    }

    public void EndMember()
    {
    }

    static DebugDumpDocumentation printer = new DebugDumpDocumentation();
    public static IDocumentationPrinter Printer
    {
        get
        {
            return printer;
        }
    }
}
