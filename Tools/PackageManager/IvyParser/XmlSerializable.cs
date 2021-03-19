using System;
using System.Collections.Generic;
using System.Reflection;
using System.Xml;
using System.Xml.Schema;
using System.Xml.Serialization;
using System.Collections;
using System.ComponentModel;

namespace Unity.PackageManager.Ivy
{
    public class XmlSerializable : IXmlSerializable
    {
        static readonly Unity.DataContract.PackageVersion compatibilityVersion = new Unity.DataContract.PackageVersion("4.5.0b2");

        static Dictionary<string, KeyValuePair<Func<object, object>, Action<object, object>>> attributeMappings =
            new Dictionary<string, KeyValuePair<Func<object, object>, Action<object, object>>>();

        static List<string> processed = new List<string>();

        static Dictionary<string, Type> typeMappings = new Dictionary<string, Type>();

        public virtual XmlSchema GetSchema()
        {
            return null;
        }

        public void ReadXml(XmlReader reader)
        {
            ProcessTypes();
            ProcessType();

            bool isEmpty = reader.IsEmptyElement;

            reader.MoveToContent();

            if (reader.HasAttributes)
            {
                string typeName = GetType().FullName + ".";

                for (int i = 0; i < reader.AttributeCount; i++)
                {
                    reader.MoveToAttribute(i);
                    string name = reader.LocalName;
                    string val = reader.Value;
#if !DISABLE_COMPAT
                    if (name == "packageType")
                    {
                        if (val == "PlaybackEngines") val = "PlaybackEngine";
                        if (val == "EditorExtension") val = "UnityExtension";
                    }
#endif
                    KeyValuePair<Func<object, object>, Action<object, object>> invoker;
                    if (attributeMappings.TryGetValue(typeName + name, out invoker))
                    {
                        invoker.Value(this, val);
                    }
                }
            }

            if (reader.IsStartElement() && !reader.IsEmptyElement)
                reader.ReadStartElement();

            while (reader.IsStartElement() && !isEmpty)
            {
                string localName = reader.LocalName;
                string typeName = localName;

                Type t;
                if (!typeMappings.TryGetValue(typeName, out t))
                {
                    typeName = GetType().Name + "." + typeName;
                    if (!typeMappings.TryGetValue(typeName, out t))
                    {
                        reader.Skip();
                        continue;
                    }
                }

                XmlSerializer serializer = GetSerializer(t);
                object o = serializer.Deserialize(reader);
                typeName = GetType().FullName + "." + localName;
                KeyValuePair<Func<object, object>, Action<object, object>> invoker;
                if (attributeMappings.TryGetValue(typeName, out invoker))
                    invoker.Value(this, o);
            }

            if (!isEmpty)
                reader.ReadEndElement();
            else
                reader.ReadStartElement();
        }

        public void WriteXml(XmlWriter writer)
        {
            ProcessTypes();
            ProcessType();

            Type type = GetType();

            string typeName = type.FullName + ".";
            MemberInfo[] members = type.GetMembers(BindingFlags.Public | BindingFlags.NonPublic | BindingFlags.Instance);

            var elementNames = new List<string>();
            var attributes = new Dictionary<string, XmlAttributeAttribute>();
            var elements = new Dictionary<string, string>();
            var defaultValues = new Dictionary<string, object>();
            foreach (var memberInfo in members)
            {
                if (memberInfo.MemberType != MemberTypes.Field && memberInfo.MemberType != MemberTypes.Property)
                    continue;

                string memberName = null;
                object[] atts = memberInfo.GetCustomAttributes(false);
                foreach (object o in atts)
                {
                    if (o is XmlIgnoreAttribute)
                        continue;

                    if (o is XmlElementAttribute)
                    {
                        XmlElementAttribute attr = ((XmlElementAttribute)o);
                        memberName = attr.ElementName;
                        if (attr.Order > 0)
                        {
                            if (elementNames.Count < attr.Order)
                            {
                                elementNames.AddRange(new string[attr.Order - elementNames.Count]);
                            }
                            elementNames[attr.Order - 1] = typeName + memberName;
                        }
                        else
                            elementNames.Add(typeName + memberName);

                        elements.Add(typeName + memberName, memberName);
                        break;
                    }
                    else if (o is XmlAttributeAttribute)
                    {
                        XmlAttributeAttribute attr = (XmlAttributeAttribute)o;
                        attributes.Add(typeName + attr.AttributeName, attr);
                        var defs = memberInfo.GetCustomAttributes(typeof(DefaultValueAttribute), false);
                        if (defs.Length > 0)
                            defaultValues.Add(typeName + attr.AttributeName, ((DefaultValueAttribute)defs[0]).Value);
                        break;
                    }
                }
            }

            foreach (var attribute in attributes)
            {
                if (!attributeMappings.ContainsKey(attribute.Key))
                    continue;
                object o = attributeMappings[attribute.Key].Key(this);
                if (o == null)
                    continue;
                string value = o.ToString();

                // this will skip fields that have the same value as their default value
                object defobj = null;
                if (defaultValues.TryGetValue(attribute.Key, out defobj))
                {
                    if (o == defobj)
                        continue;
                }

                if (o is bool)
                {
                    if (!(bool)o)
                        continue;
                    value = value.ToLower();
                }

#if !DISABLE_COMPAT
                if (o is IvyInternal.IvyPackageType && this is IvyInternal.IvyInfo)
                {
                    var p = new DataContract.PackageVersion(((IvyInternal.IvyInfo) this).UnityVersion);
                    if (p <= compatibilityVersion)
                    {
                        switch ((IvyInternal.IvyPackageType)o)
                        {
                            case Unity.PackageManager.IvyInternal.IvyPackageType.PlaybackEngine:
                                value = "PlaybackEngines";
                                break;
                            case Unity.PackageManager.IvyInternal.IvyPackageType.UnityExtension:
                                value = "EditorExtension";
                                break;
                        }
                    }
                }
#endif
                // I have NO F...... CLUE why just adding a namespace is suddenly not enough and I need to add the prefix too now
                writer.WriteAttributeString(attribute.Value.Namespace != null ? "e" : "", attribute.Value.AttributeName, attribute.Value.Namespace, value);
            }

            foreach (var el in elementNames)
            {
                if (!attributeMappings.ContainsKey(el))
                    continue;
                object o = attributeMappings[el].Key(this);
                if (o == null)
                    continue;

                // hack to support having repository elements directly in info
                if (o is List<IvyInternal.IvyRepository>)
                {
                    IList originList = o as IList;
                    foreach (object item in originList)
                    {
                        XmlSerializer serializer = GetSerializer(item.GetType());
                        serializer.Serialize(writer, item, IvyParser.Namespaces);
                    }
                }
                else
                {
                    XmlSerializer serializer = GetSerializer(o.GetType());
                    serializer.Serialize(writer, o, IvyParser.Namespaces);
                }
            }
        }

        private void ProcessTypes()
        {
            lock (typeMappings)
            {
                if (typeMappings.Count > 0)
                    return;

                Type[] types = Assembly.GetAssembly(GetType()).GetTypes();
                foreach (var type in types)
                {
                    if (type.Namespace != "Unity.PackageManager.IvyInternal")
                        continue;
                    object[] atts = type.GetCustomAttributes(false);
                    foreach (var att in atts)
                    {
                        string typename = null;
                        string namespce = null;
                        if (att is XmlTypeAttribute)
                        {
                            typename = ((XmlTypeAttribute)att).TypeName;
                            namespce = ((XmlTypeAttribute)att).Namespace;
                        }
                        else if (att is XmlRootAttribute)
                        {
                            typename = ((XmlRootAttribute)att).ElementName;
                            namespce = ((XmlRootAttribute)att).Namespace;
                        }
                        else
                            continue;

                        if (!String.IsNullOrEmpty(namespce))
                            typename = namespce + "." + typename;
                        typeMappings.Add(typename, type);
                    }
                }
            }
        }

        void ProcessType()
        {
            Type type = GetType();

            lock (processed)
            {
                if (processed.Contains(type.FullName))
                    return;

                string typeName = type.FullName + ".";
                MemberInfo[] members = type.GetMembers(BindingFlags.Public | BindingFlags.NonPublic | BindingFlags.Instance);
                foreach (var memberInfo in members)
                {
                    if (memberInfo.MemberType != MemberTypes.Field && memberInfo.MemberType != MemberTypes.Property)
                        continue;
                    string memberName = null;
                    object[] atts = memberInfo.GetCustomAttributes(false);
                    foreach (object o in atts)
                    {
                        if (o is XmlIgnoreAttribute)
                            continue;

                        if (o is XmlElementAttribute)
                        {
                            memberName = ((XmlElementAttribute)o).ElementName;
                            break;
                        }
                        else if (o is XmlAttributeAttribute)
                        {
                            memberName = ((XmlAttributeAttribute)o).AttributeName;
                            break;
                        }
                    }

                    if (memberName == null)
                        continue;

                    memberName = typeName + memberName;
                    if (attributeMappings.ContainsKey(memberName))
                        continue;

                    if (memberInfo.MemberType == MemberTypes.Field)
                    {
                        FieldInfo field = type.GetField(memberInfo.Name, BindingFlags.Public | BindingFlags.NonPublic | BindingFlags.Instance);
                        attributeMappings.Add(memberName, new KeyValuePair<Func<object, object>, Action<object, object>>(
                            (who) => field.GetValue(who),
                            (who, what) =>
                            {
                                var converter = System.ComponentModel.TypeDescriptor.GetConverter(field.FieldType);
                                var value = what;
                                if (value != null && converter.CanConvertFrom(value.GetType()))
                                    value = converter.ConvertFrom(what);
                                field.SetValue(who, value);
                            }));
                    }
                    else if (memberInfo.MemberType == MemberTypes.Property)
                    {
                        PropertyInfo prop = type.GetProperty(memberInfo.Name, BindingFlags.Public | BindingFlags.NonPublic | BindingFlags.Instance);
                        attributeMappings.Add(memberName, new KeyValuePair<Func<object, object>, Action<object, object>>(
                            (who) => prop.GetGetMethod(true).Invoke(who, null),
                            (who, what) =>
                            {
                                // handle individual elements with no container
                                if (!(what is System.Collections.IList) &&
                                    typeof(System.Collections.IList).IsAssignableFrom(prop.PropertyType))
                                {
                                    if (what == null)
                                        return;
                                    var target = (System.Collections.IList)prop.GetGetMethod(true).Invoke(who, null);
                                    if (target == null && memberInfo.Name.StartsWith("xml"))
                                    {
                                        // find the corresponding getter that returns a created list instead of null
                                        var listprop = type.GetProperty(memberInfo.Name.Substring(3), BindingFlags.Public | BindingFlags.NonPublic | BindingFlags.Instance);
                                        if (listprop != null)
                                            target = (System.Collections.IList)listprop.GetGetMethod(true).Invoke(who, null);
                                    }
                                    if (target != null)
                                        target.Add(what);
                                }
                                else
                                    prop.GetSetMethod(true).Invoke(who, new object[] {what});
                            }));
                    }
                }
                processed.Add(type.FullName);
            }
        }

        static object cacheLock = new object();
        static Dictionary<Type, XmlSerializer> serializerCache = new Dictionary<Type, XmlSerializer>();
        internal static XmlSerializer GetSerializer(Type type)
        {
            lock (cacheLock)
            {
                // the `FromTypes` form, rather than the simpler ctor form, avoids an annoying exception, though
                // we have to do the caching ourselves (which is fine); see https://stackoverflow.com/a/9943900/14582
                if (!serializerCache.ContainsKey(type))
                    serializerCache.Add(type, XmlSerializer.FromTypes(new[] { type })[0]);

                return serializerCache[type];
            }
        }
    }
}
