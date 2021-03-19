using System;
using System.Text;
using System.Web;

public interface ExternalDocumentationUrlBuilder
{
    string NameSpaceUrl(string ns);
    string ClassUrl(string ns, string name);
    string StructUrl(string ns, string name);
    string FieldUrl(string ns, string c, string name);
    string PropertyUrl(string ns, string c, string name);
    string ConstructorUrl(string ns, string c, string name);
    string MethodUrl(string ns, string c, string name);
}

static class MsdnSearchUrlBuilderHelpers
{
    static string UrlConvertedQuote = "%22";

    public static void AddQueryArgument(this StringBuilder sb, string argument)
    {
        sb.Append("+");
        sb.Append(UrlConvertedQuote);
        sb.Append(HttpUtility.UrlEncode(argument));
        sb.Append(UrlConvertedQuote);
    }

    public static void AddPreQualifiedQueryArgument(this StringBuilder sb, string argument, string argumentQualifier)
    {
        sb.Append("+");
        sb.Append(UrlConvertedQuote);
        sb.Append(argumentQualifier);
        sb.Append(": ");
        sb.Append(HttpUtility.UrlEncode(argument));
        sb.Append(UrlConvertedQuote);
    }

    public static void AddPostQualifiedQueryArgument(this StringBuilder sb, string argument, string argumentQualifier)
    {
        sb.Append("+");
        sb.Append(UrlConvertedQuote);
        sb.Append(HttpUtility.UrlEncode(argument));
        sb.Append(" ");
        sb.Append(argumentQualifier);
        sb.Append(UrlConvertedQuote);
    }

    public static string BuildFinalUrl(this StringBuilder sb)
    {
        return sb.ToString();
    }
}

public class MsdnSearchUrlBuilder : ExternalDocumentationUrlBuilder
{
    static string urlBase = "http://www.google.com/search?q=site:msdn.microsoft.com";

    static StringBuilder CreateStringBuilderForUrl()
    {
        return new StringBuilder(urlBase);
    }

    public string NameSpaceUrl(string ns)
    {
        StringBuilder sb = CreateStringBuilderForUrl();
        sb.AddQueryArgument(ns);
        return sb.BuildFinalUrl();
    }

    static string TypeUrl(string ns, string name, string qualifier)
    {
        StringBuilder sb = CreateStringBuilderForUrl();
        sb.AddPreQualifiedQueryArgument(ns, "Namespace");
        sb.AddPostQualifiedQueryArgument(name, qualifier);
        return sb.BuildFinalUrl();
    }

    public string ClassUrl(string ns, string name)
    {
        return (TypeUrl(ns, name, "Class"));
    }

    public string StructUrl(string ns, string name)
    {
        return (TypeUrl(ns, name, "Struct"));
    }

    static string MemberUrl(string ns, string c, string name, string qualifier)
    {
        StringBuilder sb = CreateStringBuilderForUrl();
        sb.AddPreQualifiedQueryArgument(ns, "Namespace");
        sb.AddQueryArgument(c + "." + name);
        sb.AddQueryArgument(qualifier);
        return sb.BuildFinalUrl();
    }

    public string FieldUrl(string ns, string c, string name)
    {
        return MemberUrl(ns, c, name, "Field");
    }

    public string PropertyUrl(string ns, string c, string name)
    {
        return MemberUrl(ns, c, name, "Property");
    }

    public string ConstructorUrl(string ns, string c, string name)
    {
        return MemberUrl(ns, c, name, "Constructor");
    }

    public string MethodUrl(string ns, string c, string name)
    {
        return MemberUrl(ns, c, name, "Method");
    }

    static MsdnSearchUrlBuilder instance = new MsdnSearchUrlBuilder();
    public static MsdnSearchUrlBuilder Instance
    {
        get
        {
            return instance;
        }
    }
}
