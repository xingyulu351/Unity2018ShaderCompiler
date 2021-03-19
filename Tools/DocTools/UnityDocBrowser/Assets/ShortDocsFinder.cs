using UnityEngine;
using UnityEngine.Networking;
using System.Collections;
using System.Collections.Generic;
using System.Xml;
using System.Text;
using System.IO;
using System;

public class ShortDocsFinder : MonoBehaviour
{
    public string pathToApiDocs = "..\\..\\..\\..\\Documentation\\ApiDocs\\";
    public int findDocsShorterThan = int.MaxValue;
    public TextAsset analyticsCSV;
    int numEntriesWithExamplesFound = 0;

    public Dictionary<string, MemberDoc> docsData = new Dictionary<string, MemberDoc>();

    public class MemberDoc
    {
        public string member;

        // from the scriptref xml
        public bool undoc;
        public string type;
        // length in characters
        public int summaryLength;
        public int descriptionLength;
        public int examplesLength;
        public int totalLength;
        public void SetScriptRefData(bool undoc, string type, int summary, int desc, int examples)
        {
            this.undoc = undoc;
            this.type = type;
            summaryLength = summary;
            descriptionLength = desc;
            examplesLength = examples;
            totalLength = summaryLength + examplesLength + descriptionLength;
        }

        // from the analytics csv
        public int pageViews; // total page views
        public int pageViewsUnique; // unique user page views
        public float timeSpentOnPage; // how long people stay here before going to another page?
        public float bounceRate; // people who do not then go on to view another page...?
        public float exitPercent; // I do not know what this number means
        public void SetAnalyticsData(int pageViews, int pageViewsUnique, float timeSpentOnPage, float bounceRate, float exitPercent)
        {
            this.pageViews = pageViews;
            this.pageViewsUnique = pageViewsUnique;
            this.timeSpentOnPage = timeSpentOnPage;
            this.bounceRate = bounceRate;
            this.exitPercent = exitPercent;
        }

        public float score
        {
            get
            {
                return summaryLength + descriptionLength;
            }
        }
    }

    static int CompareMemberDocByScore(MemberDoc lhs, MemberDoc rhs)
    {
        float ls = lhs.score;
        float rs = rhs.score;
        if (ls == rs) return 0;
        if (ls < rs)
            return -1;
        return 1;
    }

    string SanitizeMemberName(string memberName)
    {
        memberName = memberName.ToLower();
        // remove leading / characters
        if (memberName.StartsWith("/")) memberName = memberName.Substring(1);
        // remove leading "unityengine"
        if (memberName.StartsWith("unityengine.")) memberName = memberName.Substring("unityengine.".Length);
        // remove trailing ".html"
        if (memberName.EndsWith(".html")) memberName = memberName.Substring(0, memberName.Length - 5);
        // standardise the namespace referencing character
        memberName = memberName.Replace('-', '.');
        // some pages start have members starting with _, so get rid of those
        memberName = memberName.Replace("._", ".");
        return memberName;
    }

    // Use this for initialization
    IEnumerator Start()
    {
        yield return null;

        // parse the analytics data
        char[] crSplit = { '\n' };
        char[] commaSplit = { ',' };
        string[] lines = analyticsCSV.text.Split(crSplit);
        Debug.Log(lines.Length + " lines of Analytics data to parse.");
        for (int i = 0; i < lines.Length; i++)
        {
            string line = lines[i];
            if (line.Contains(",") && !line.Contains("?"))
            {
                // probably a csv line rather than header/etc
                string[] data = line.Split(commaSplit);
                int tempResult;
                if (int.TryParse(data[1], out tempResult))
                {
                    // should be member, pageviews, uniques, time on page, bounce, exit

                    // get the standardized member name
                    string name = SanitizeMemberName(data[0]);
                    // analytics pages are named differently to scriptref
                    name = name.Replace("operator_eq", "operator_equal");
                    name = name.Replace("operator_ne", "operator_notequal");
                    name = name.Replace("operator_plus", "operator_add");
                    name = name.Replace("operator_minus", "operator_subtract");
                    name = name.Replace("operator_", "operator_subtract");

                    MemberDoc member;
                    if (!docsData.TryGetValue(name, out member))
                    {
                        member = new MemberDoc();
                        docsData.Add(name, member);
                    }
                    string report = "";
                    for (int j = 0; j < data.Length; j++)
                    {
                        data[j] = data[j].Replace("%", "");
                        report += data[j] + ", ";
                    }
                    //Debug.Log (report);
                    DateTime timeOnPage;
                    if (data.Length > 4)
                    {
                        if (DateTime.TryParse(data[3], out timeOnPage))
                        {
                            int seconds = timeOnPage.Second + timeOnPage.Minute * 60 + timeOnPage.Hour * 60 * 60;
                            member.SetAnalyticsData(int.Parse(data[1]), int.Parse(data[2]), seconds, float.Parse(data[4]), float.Parse(data[5]));
                        }
                    }
                }
            }
        }

        if (!pathToApiDocs.Contains(":"))
        {
            pathToApiDocs = Application.dataPath + "/" + pathToApiDocs;
        }
        pathToApiDocs = pathToApiDocs.Replace('\\', '/');

        Debug.Log("Searching for API docs in: " + pathToApiDocs);
        Debug.Log("Hold the space bar to exit early.");
        string[] filePaths = Directory.GetFiles(pathToApiDocs, "*.mem.xml", SearchOption.AllDirectories);


        for (int n = 0; n < filePaths.Length; ++n)
        {
            int pc = (n * 100) / filePaths.Length;

            if (n % 1000 == 0)
            {
                // progress update;
                Debug.Log("Reading " + n + "/" + filePaths.Length + " (" + pc + "%)");
                if (Input.GetKeyDown(KeyCode.Space))
                {
                    break;
                }
                yield return null;
            }

            string ns = "";
            // Create an XmlReader
            XmlDocument memxml = new XmlDocument();
            memxml.Load(filePaths[n]);
            XmlNode xmlheader = memxml.FirstChild;
            XmlNode docsheader = xmlheader.NextSibling;
            XmlNode namespaceTag = docsheader.FirstChild;
            ParseMemberNode(namespaceTag);
        }

        StringBuilder output = new StringBuilder();

        output.AppendLine("Member,Type,undoc,Summary length,Description length,Example length,Total size,Pageviews,Unique pageviews,Time spent on page,Bounce rate,Exit percent");
        foreach (KeyValuePair<string, MemberDoc> kvp in docsData)
        {
            MemberDoc m = kvp.Value;
            output.AppendLine(kvp.Key + "," + m.type + "," + (m.undoc ? "y" : "n")
                + "," + m.summaryLength + "," + m.descriptionLength + "," + m.examplesLength + "," + m.totalLength
                + "," + m.pageViews + "," + m.pageViewsUnique + "," + m.timeSpentOnPage
                + "," + m.bounceRate + "," + m.exitPercent);
        }

        File.Delete(Application.dataPath + "/ShortDocs.csv");
        File.WriteAllText(Application.dataPath + "/ShortDocs.csv", output.ToString());
        Debug.Log("Finished. ShortDocs.csv file written.");
        Debug.Log("Found " + numEntriesWithExamplesFound + " entries with examples.");
        Debug.Log("Dictionary contains " + docsData.Keys.Count + " entries.");
    }

    void ParseMemberNode(XmlNode node)
    {
        // nodes should consist of a member tag,
        //              then nested section nodes containing
        //                              signature, summary, param, description and example nodes.
        //              section nodes can be followed by further member nodes,
        //                              making this function recursive.

        // info from attributes from this tag
        string nspace = "";
        string name = "";
        string type = "";
        foreach (XmlAttribute attr in node.Attributes)
        {
            // potential tag attributes: name, namespace, type
            if (attr.Name.ToLower() == "name") name = attr.Value;
            if (attr.Name.ToLower() == "namespace") nspace = attr.Value;
            if (attr.Name.ToLower() == "type") type = attr.Value;
        }
        if (nspace != "")
        {
            // trim down the namespace to remove UnityEngine and UnityEditor
            if (nspace.StartsWith("UnityEngine")) nspace = nspace.Remove(0, "UnityEngine".Length);
            if (nspace.StartsWith("UnityEditor")) nspace = nspace.Remove(0, "UnityEditor".Length);
            if (nspace.StartsWith("."))
                nspace = nspace.Remove(0, 1);
        }
        // info from child tags
        string summary = "";
        string description = "";
        string examples = "";
        bool undoc = false;
        foreach (XmlNode child in node.ChildNodes)
        {
            switch (child.Name.ToLower())
            {
                default:
                    // unknown node type
                    break;
                case "section":
                {
                    // see if this section is undoc
                    for (int i = 0; i < child.Attributes.Count; i++)
                        if (child.Attributes[i].Name == "undoc" && child.Attributes[i].Value == "true")
                            undoc = true;


                    foreach (XmlNode grandChild in child.ChildNodes)
                    {
                        switch (grandChild.Name.ToLower())
                        {
                            case "signature":
                            case "param":
                                // don't care
                                break;
                            case "summary":
                                summary += grandChild.InnerText;
                                break;
                            case "description":
                                description += grandChild.InnerText;
                                break;
                            case "example":
                                // examples contain one or many "code" tags inside which is the actual example
                                // let's just use the first available one
                                examples += grandChild.InnerText;
                                break;
                        }
                    }
                }
                break;
                case "member":
                    ParseMemberNode(child);
                    break;
            }
        }
        name = SanitizeMemberName(name);
        // apparently scriptref shortens operator_ to op_
        if (type == "Operator")
            name = name.Replace("op_", "operator_");
        if (name.StartsWith("ai."))
            name = name.Substring(3, name.Length - 3);
        // slice off the namespace to match the analytics data. our scriptref URLs do not include the namespace
//      if (nspace.Length > 0 && name.StartsWith (nspace.ToLower ())) {
//          Debug.Log ("Trimming " + nspace + " from " + name + ".");
//          name = name.Remove (0, nspace.Length); // don't forget the dot
//          if (name.StartsWith(".")) name = name.Remove(0,1);
//      }
        MemberDoc member;
        if (!docsData.TryGetValue(name, out member))
        {
            member = new MemberDoc();
            docsData.Add(name, member);
        }

        member.SetScriptRefData(undoc, type, summary.Length, description.Length, examples.Length);
    }

//  private void ParseMember(ref string ns, XmlReader reader)
//  {
//      bool undoc = false;
//      string n = reader.GetAttribute("name");
//      string t = reader.GetAttribute("type");
//      if (reader.GetAttribute("namespace") != "")
//      {
//          ns = reader.GetAttribute("namespace");
//      }
//      reader.ReadToFollowing("section");
//      if (reader.GetAttribute("undoc") == "true") undoc = true;
//
//      //if (!undoc)
//      {
//          reader.ReadToDescendant("summary");
//          int summaryLength = reader.ReadString().Length;
//          bool found = true;
//          bool once = true;
//          int descLength = 0;
//          string description;
//          // enter a loop as we could potentially have many description/example pairs in sequence
//          while (found) {
//
//              // read the description and add its length to our count
//              found = reader.ReadToNextSibling ("description");
//              description = reader.ReadString ();
//              descLength += description.Length;
//
//              // prepend the namespace name to the member name, but only once
//              if (once && ns != null && ns.Length > 0) {
//                  // this messes with matching out analytics data
//                  //n = ns + "." + n;
//              }
//
//              // see if we have any examples. In the event there are two
//              // language examples (JS and C#) this only reads the first
//              // example, so results can be slightly inaccurate if one
//              // language example is longer than the other.
//              if (found) {
//                  found = reader.ReadToNextSibling ("example");
//                  description = reader.ReadString ();
//                  descLength += description.Length;
//                  if (once)
//                      numEntriesWithExamplesFound++;
//              }
//
//              once = false;
//          }
//          int totalLength = descLength + summaryLength;
//          MemberDoc member;
//          string name = SanitizeMemberName (n);
//          if (!docsData.TryGetValue (name, out member)) {
//              member = new MemberDoc ();
//              docsData.Add(name, member);
//          }
//          member.SetScriptRefData (undoc, t, summaryLength, descLength, 0, totalLength);
//
//
//      }
//  }
}
