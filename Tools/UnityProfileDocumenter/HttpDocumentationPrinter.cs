using System;
using System.Text;
using System.IO;
using System.Web;

public class HttpDocumentationPrinter : IDocumentationPrinter
{
    const string squareOpenBracket = "&#91;";
    const string squareCloseBracket = "&#93;";
    const string red = "A80000";
    const string green = "00CC66";

    void PrintMsdnSearchUrl(string url)
    {
        if (!String.IsNullOrEmpty(url))
        {
            tw.Write("<a href=\"");
            tw.Write(url);
            tw.Write("\">Search MSDN</a>");
        }
    }

    void PrintUnityProfiles(UnityProfiles enumP)
    {
        foreach (UnityProfiles profile in UnityProfilesUtils.ListProfiles())
        {
            tw.Write("<td style=\"color:#{0}; padding-left:10px; padding-right:10px; \" class=\"monoCompatibility-cell\"> {1} </td> ", (0 == (enumP & profile)) ? red : green, profile.DocumentationNameFromProfile());
        }
    }

    string MemberRowStyleClass()
    {
        return isRowEven ? "monoCompatibility-roweven" : "monoCompatibility-rowodd";
    }

    string fileName;
    TextWriter tw;

    bool isRowEven = true;

    public HttpDocumentationPrinter(string fileName)
    {
        this.fileName = fileName;
        this.tw = null;
    }

    public void StartDocumentation()
    {
        tw = new StreamWriter(fileName);

        // Makes the code compatible with our current build system
        tw.Write("DOCUMENT \"Mono Compatibility\" MonoCompatibility.html\n");
        tw.Write("You can use .NET class libraries in the scripts you write for Unity.  Depending on your .NET Compatibility level selected in your project settings,  Unity supports more, or less of the full class libraries.  This page contains a list of which classes and methods are available for each .NET compatibility level setting.\n");
    }

    public void EndDocumentation()
    {
        tw.Write("END\n");

        tw.Close();
        tw = null;
    }

    public void StartNamespace(string name, string url)
    {
        tw.Write("<li><h4>Namespace {0}</h4></li><ul>\n", name);
    }

    public void EndNamespace()
    {
        tw.Write("</ul>\n");
    }

    public void StartClass(string name, string ns, string assembly, UnityProfiles profiles, string url)
    {
        tw.Write("<table border=\"0\"><tr class=\"monoCompatibility-roweven\">");
        tw.Write("<td><b>Class: </b> {0} (namespace {1}, assembly {2})</td>\n", HttpUtility.HtmlEncode(name), ns, assembly);
        tw.Write("<td></td>\n");
        tw.Write("<td style=\"padding-left:5px; padding-right:5px;\" class=\"monoCompatibility-cell\">");
        PrintMsdnSearchUrl(url);
        tw.Write("</td>\n");
        PrintUnityProfiles(profiles);
        tw.Write("</tr>");

        tw.Write("<tr> <td colspan=\"2\" height=\"5\">&nbsp;</td> </tr>\n");
        //Empty Line in the table.
    }

    public void EndClass()
    {
        //end the class table
        tw.Write("</table><br /><br />\n");
    }

    public void StartMembers()
    {
        isRowEven = false;
        tw.Write("<tr>");
        tw.Write("<td><b>Members: </b></td>\n");
        tw.Write("</tr>");
    }

    public void EndMembers()
    {
    }

    public void StartMember(string name, UnityProfiles profiles, string url)
    {
        tw.Write("<tr class=\"{0}\">", HttpUtility.HtmlEncode(MemberRowStyleClass()));

        tw.Write("<td style=\"width:700px; \" class=\"monoCompatibility-cell\"> {0} </td>", name);
        tw.Write("<td>  </td>");
        tw.Write("<td style=\"padding-left:5px; padding-right:5px;\" class=\"monoCompatibility-cell\">");
        PrintMsdnSearchUrl(url);
        tw.Write("</td>");
        PrintUnityProfiles(profiles);
        tw.Write("</tr>");
    }

    public void EndMember()
    {
        isRowEven = !isRowEven;
    }
}
