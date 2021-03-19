#pragma warning disable 414

using System;
using System.Collections.Generic;
using System.IO;
using System.Text;
using System.Xml;
using System.Xml.Serialization;

namespace Unity.PackageManager.Ivy
{
    public static class IvyParser
    {
        public static XmlSerializerNamespaces Namespaces;
        public static bool HasErrors;
        public static Exception ErrorException;
        public static string ErrorMessage;

        static IvyParser()
        {
            Namespaces = new XmlSerializerNamespaces();
            Namespaces.Add("e", "http://ant.apache.org/ivy/extra");
        }

        public static T ParseFile<T>(string path) where T : class
        {
            HasErrors = false;
            ErrorMessage = null;
            ErrorException = null;

            if (!File.Exists(path))
            {
                HasErrors = true;
                ErrorMessage = String.Format("File does not exist: {0}", path);
                return null;
            }
            object obj = Parse<T>(File.ReadAllText(path, Encoding.UTF8));
            if (obj != null && typeof(T) == typeof(IvyModule))
            {
                ((IvyModule)obj).BasePath = Path.GetDirectoryName(path);
                ((IvyModule)obj).IvyFile = Path.GetFileName(path);
            }
            return obj as T;
        }

        public static T Parse<T>(string xml) where T : class
        {
            object obj = Deserialize<T>(xml);
            if (obj != null && typeof(T) == typeof(IvyModule))
            {
                ((IvyModule)obj).IvyFile = "ivy.xml";
            }
            return obj as T;
        }

        public static string Serialize(ModuleRepository repo)
        {
            HasErrors = false;
            ErrorMessage = null;
            ErrorException = null;

            IvyInternal.ModuleRepository obj = Cloner.CloneObject<IvyInternal.ModuleRepository>(repo);

            IvyInternal.IvyRoot root = new IvyInternal.IvyRoot();
            root.Repository = obj;

            StringBuilder sb = new StringBuilder();
            using (var sw = new UTF8StringWriter(sb))
            {
                XmlSerializer serializer = XmlSerializable.GetSerializer(root.GetType());
                serializer.Serialize(sw, root, Namespaces);
            }
            return sb.ToString().Replace("<root>", "").Replace("</root>", "");
        }

        public static string Serialize(IvyModule module)
        {
            HasErrors = false;
            ErrorMessage = null;
            ErrorException = null;

            IvyInternal.IvyModule obj = Cloner.CloneObject<IvyInternal.IvyModule>(module);

            IvyInternal.IvyRoot root = new IvyInternal.IvyRoot();
            root.Module = obj;

            StringBuilder sb = new StringBuilder();
            using (var sw = new UTF8StringWriter(sb))
            {
                XmlSerializer serializer = XmlSerializable.GetSerializer(obj.GetType());
                serializer.Serialize(sw, obj, Namespaces);
            }
            return sb.ToString().Replace("<root>", "").Replace("</root>", "");
        }

        public static T Deserialize<T>(string xml) where T : class
        {
            HasErrors = false;
            ErrorMessage = null;
            ErrorException = null;

            if (xml.Length <= 0)
            {
                HasErrors = true;
                ErrorMessage = "Cannot deserialize empty xml document.";
                return null;
            }

            Type type = typeof(T);
            int index = 0;
            int indexModule = xml.IndexOf("<ivy-module");
            int indexRepo = xml.IndexOf("<ivy-repository");
            if (typeof(T) == typeof(IvyModule))
            {
                if (indexModule < 0)
                    return null;
                else
                    index = indexModule;
                type = typeof(IvyModule);
            }
            else if (typeof(T) == typeof(ModuleRepository))
            {
                if (indexRepo < 0)
                    return null;
                else
                    index = indexRepo;
                type = typeof(ModuleRepository);
            }
            else if (typeof(T) == typeof(object))
            {
                if (indexRepo >= 0)
                {
                    index = indexRepo;
                    type = typeof(ModuleRepository);
                }
                else if (indexModule >= 0)
                {
                    index = indexModule;
                    type = typeof(IvyModule);
                }
            }

            xml = xml.Insert(index, "<root>") + "</root>";

            try
            {
                XmlTextReader xr = new XmlTextReader(xml, XmlNodeType.Document, null);
                XmlSerializer serializer = XmlSerializable.GetSerializer(typeof(IvyInternal.IvyRoot));
                IvyInternal.IvyRoot mod = serializer.Deserialize(xr) as IvyInternal.IvyRoot;
                if (type == typeof(IvyModule))
                    return Cloner.CloneObject(mod.Module, type) as T;
                return Cloner.CloneObject(mod.Repository, type) as T;
            }
            catch (Exception ex)
            {
                HasErrors = true;
                ErrorMessage = "Deserialization failed.";
                ErrorException = ex;
            }
            return null;
        }
    }

    // This seems to be the best way to set the encoding for xml serialization
    class UTF8StringWriter : StringWriter
    {
        public UTF8StringWriter(StringBuilder builder) : base(builder)
        {
        }

        public override Encoding Encoding
        {
            get { return Encoding.UTF8; }
        }
    }
}
