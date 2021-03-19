using System;
using System.IO;
using System.Collections.Generic;
using System.Xml;
using System.Text;

namespace ScriptRefXMLSorter
{
    class MainClass
    {
        // compare nodes by the contents of the name property of this tag
        static int CompareByName(XmlNode n1, XmlNode n2)
        {
            string n1Name = "";
            string n2Name = "";
            foreach (XmlNode attr in n1.Attributes)
            {
                if (attr.Name == "name")
                    n1Name = attr.Value;
            }
            foreach (XmlNode attr in n2.Attributes)
            {
                if (attr.Name == "name")
                    n2Name = attr.Value;
            }
            return n1Name.CompareTo(n2Name);
        }

        // ScriptRefXMLSorter by Alex May @alexmay
        // this program finds the api xml docs, opens each file, and sorts the member tags within by alphabetical order
        // in order to produce a more reliable file format that is easier to merge between source code branches
        public static void Main(string[] args)
        {
            Console.WriteLine("ScriptRefXMLSorter by Alex May.\n\nThis program sorts the member tags of the API docs XML files by alphabetical order.\n\nSend relative path to Documentation directory as a parameter or run locally to the repository to find the Documentation directory automatically.");

            // first we need to find the content

            string argType = "-d"; // default to directory
            string individualFile = "";

            if (args.Length > 0)
            {
                foreach (string s in args)
                {
                    Console.WriteLine("arg:" + s);
                    if (s.StartsWith("-"))
                    {
                        // it's an arg type
                        if (s == "-f")
                        {
                            // individual file
                            argType = "-f";
                        }
                    }
                    else
                    {
                        // it's an arg value

                        if (argType == "-f")
                        {
                            // process individual file
                            individualFile = Path.GetFileName(s);
                            string dir = Path.GetDirectoryName(s);
                            ProcessFile(individualFile, dir);
                            return;
                        }

                        if (argType == "-d")
                        {
                            // process directory
                            try
                            {
                                Directory.SetCurrentDirectory(Path.Combine(Directory.GetCurrentDirectory(), s));
                            }
                            catch
                            {
                                Console.WriteLine("Unable to recognise input parameter as a directory. Ignoring parameter and trying to find Documentation directory relative to current location.");
                            }
                        }
                    }
                }
            }

            while (!Directory.Exists("Documentation") && Directory.GetCurrentDirectory().Contains(Path.DirectorySeparatorChar.ToString()))
            {
                // try moving up, we may be in DocTools
                Directory.SetCurrentDirectory(Directory.GetParent(Directory.GetCurrentDirectory()).FullName);
            }

            if (!Directory.Exists(Path.Combine(Directory.GetCurrentDirectory(), "Documentation")))
            {
                // we need to exit
                Console.WriteLine("Fatal error: Could not locate the Documentation/ApiDocs directory. Exiting.");
                return;
            }

            // navigate to the ApiDocs
            Directory.SetCurrentDirectory(Path.Combine(Directory.GetCurrentDirectory(), "Documentation"));
            Directory.SetCurrentDirectory(Path.Combine(Directory.GetCurrentDirectory(), "ApiDocs"));

            // begin our sort
            SortDir(Directory.GetCurrentDirectory());
        }

        static void SortDir(string dir)
        {
            // enumerate all the .xml files in this dir
            List<string> xmlFiles = new List<string>();
            xmlFiles.AddRange(Directory.EnumerateFiles(dir, "*.xml"));

            foreach (string xmlFilename in xmlFiles)
            {
                ProcessFile(xmlFilename, dir);
            }

            // also run the sort in all subdirs
            List<string> subDirs = new List<string>();
            subDirs.AddRange(Directory.EnumerateDirectories(dir));
            foreach (string subDir in subDirs)
            {
                SortDir(Path.Combine(dir, subDir));
            }
        }

        static void ProcessFile(string xmlFilename, string dir)
        {
            Console.WriteLine("Processing file " + xmlFilename + "...");

            // open the file and copy it to a string
            string xml = File.ReadAllText(Path.Combine(dir, xmlFilename));

            XmlDocument doc = new XmlDocument();

            // load in the xml
            doc.LoadXml(xml);

            // get to the correct node from which to start searching
            XmlNode rootNode = doc.FirstChild;
            // this one should be "xml"
            if (rootNode.Name == "xml")
            {
                // we want the next node, called doc
                rootNode = rootNode.NextSibling;
                // ok, within this should be at least one "member" tag
                Sort(rootNode, 0);
            }

            // overwrite the file
            XmlWriterSettings xmlWriterSettings = new XmlWriterSettings();
            // always use Unix line endings.
            xmlWriterSettings.NewLineChars = "\n";
            xmlWriterSettings.Indent = true;
            xmlWriterSettings.IndentChars = "  ";
            XmlWriter writer = XmlWriter.Create(Path.Combine(dir, xmlFilename), xmlWriterSettings);

            // save the new doc
            doc.WriteContentTo(writer);

            // don't forget to close it!
            writer.Close();
        }

        static void Sort(XmlNode parentNode, int depth)
        {
            XmlNode node = parentNode.FirstChild;
            List<XmlNode> nodesToSort = new List<XmlNode>();
            XmlNode lastAnchorNode = null;

            // for all children:
            while (node != null)
            {
                // add members to a list until we hit a section
                while (node != null && node.Name == "member")
                {
                    Sort(node, depth + 1);
                    nodesToSort.Add(node);
                    XmlNode nextNode = node.NextSibling;
                    // remove the old node
                    parentNode.RemoveChild(node);
                    node = nextNode;
                }

                if (nodesToSort.Count > 0)
                {
                    nodesToSort.Sort(CompareByName);
                    // reverse the list as we want to insert them end-first
                    nodesToSort.Reverse();

                    // re-insert the newly-sorted ones
                    if (lastAnchorNode != null)
                    {
                        foreach (XmlNode sortee in nodesToSort)
                        {
                            parentNode.InsertAfter(sortee, lastAnchorNode);
                        }
                    }
                    else
                    {
                        // must have been members all the way from the start
                        foreach (XmlNode sortee in nodesToSort)
                        {
                            parentNode.PrependChild(sortee);
                        }
                    }
                }

                // continue searching until we hit the next member
                while (node != null && node.Name != "member")
                {
                    // make a note of this node so we can add subsequently-occurring sorted member nodes after it
                    lastAnchorNode = node;
                    node = node.NextSibling;
                }
            }
        }
    }
}
