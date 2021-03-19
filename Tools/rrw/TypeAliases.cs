using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Unity.ReferenceRewriter
{
    public static class TypeAliases
    {
        private static readonly SortedSet<Tuple<string, string>> _aliases;

        static TypeAliases()
        {
            _aliases = new SortedSet<Tuple<string, string>>
            {
                new Tuple<string, string>(
                    "System.Collections.Specialized.INotifyCollectionChanged",                  "Windows.UI.Xaml.Interop.INotifyCollectionChanged"),
                new Tuple<string, string>(
                    "System.Collections.Specialized.NotifyCollectionChangedAction",             "Windows.UI.Xaml.Interop.NotifyCollectionChangedAction"),
                new Tuple<string, string>(
                    "System.Collections.Specialized.NotifyCollectionChangedEventArgs",          "Windows.UI.Xaml.Interop.NotifyCollectionChangedEventArgs"),
                new Tuple<string, string>(
                    "System.Collections.Specialized.NotifyCollectionChangedEventHandler",       "Windows.UI.Xaml.Interop.NotifyCollectionChangedEventHandler"),
                new Tuple<string, string>(
                    "System.ComponentModel.INotifyPropertyChanged",                             "Windows.UI.Xaml.Data.INotifyPropertyChanged"),
                new Tuple<string, string>(
                    "System.ComponentModel.PropertyChangedEventArgs",                           "Windows.UI.Xaml.Data.PropertyChangedEventArgs"),
                new Tuple<string, string>(
                    "System.ComponentModel.PropertyChangedEventHandler",                        "Windows.UI.Xaml.Data.PropertyChangedEventHandler"),
                new Tuple<string, string>(
                    "System.Windows.Input.ICommand",                                            "Windows.UI.Xaml.Input.ICommand"),
                new Tuple<string, string>(
                    "System.AttributeTargets",                                                  "Windows.Foundation.Metadata.AttributeTargets"),
                new Tuple<string, string>(
                    "System.AttributeUsageAttribute",                                           "Windows.Foundation.Metadata.AttributeUsageAttribute"),
                new Tuple<string, string>(
                    "System.DateTimeOffset",                                                    "Windows.Foundation.DateTime"),
                new Tuple<string, string>(
                    "System.EventHandler",                                                      "Windows.Foundation.EventHandler"),
                new Tuple<string, string>(
                    "System.Exception",                                                         "Windows.Foundation.HResult"),
                new Tuple<string, string>(
                    "System.IDisposable",                                                       "Windows.Foundation.IClosable"),
                new Tuple<string, string>(
                    "System.Nullable",                                                          "Windows.Foundation.IReference"),
                new Tuple<string, string>(
                    "System.TimeSpan",                                                          "Windows.Foundation.TimeSpan"),
                new Tuple<string, string>(
                    "System.Type",                                                              "Windows.UI.Xaml.Interop.TypeName"),
                new Tuple<string, string>(
                    "System.Uri",                                                               "Windows.Foundation.Uri"),
                new Tuple<string, string>(
                    "System.Collections.IEnumerable",                                           "Windows.UI.Xaml.Interop.IBindableIterable"),
                new Tuple<string, string>(
                    "System.Collections.IList",                                                 "Windows.UI.Xaml.Interop.IBindableVector"),
                new Tuple<string, string>(
                    "System.Collections.Generic.IDictionary",                                   "Windows.Foundation.Collections.IMap"),
                new Tuple<string, string>(
                    "System.Collections.Generic.IEnumerable",                                   "Windows.Foundation.Collections.IIterable"),
                new Tuple<string, string>(
                    "System.Collections.Generic.IList",                                         "Windows.Foundation.Collections.IVector"),
                new Tuple<string, string>(
                    "System.Collections.Generic.IReadOnlyDictionary",                           "Windows.Foundation.Collections.IMapView"),
                new Tuple<string, string>(
                    "System.Collections.Generic.IReadOnlyList",                                 "Windows.Foundation.Collections.IVectorView"),
                new Tuple<string, string>(
                    "System.Collections.Generic.KeyValuePair",                                  "Windows.Foundation.Collections.IKeyValuePair"),
                new Tuple<string, string>(
                    "System.Runtime.InteropServices.WindowsRuntime.EventRegistrationToken",     "Windows.Foundation.EventRegistrationToken"),
                new Tuple<string, string>(
                    "Windows.Foundation.Point",                                                 "Windows.Foundation.Point"),
                new Tuple<string, string>(
                    "Windows.Foundation.Rect",                                                  "Windows.Foundation.Rect"),
                new Tuple<string, string>(
                    "Windows.Foundation.Size",                                                  "Windows.Foundation.Size"),
                new Tuple<string, string>(
                    "Windows.UI.Color",                                                         "Windows.UI.Color"),
                new Tuple<string, string>(
                    "Windows.UI.Xaml.CornerRadius",                                             "Windows.UI.Xaml.CornerRadius"),
                new Tuple<string, string>(
                    "Windows.UI.Xaml.Duration",                                                 "Windows.UI.Xaml.Duration"),
                new Tuple<string, string>(
                    "Windows.UI.Xaml.DurationType",                                             "Windows.UI.Xaml.DurationType"),
                new Tuple<string, string>(
                    "Windows.UI.Xaml.GridLength",                                               "Windows.UI.Xaml.GridLength"),
                new Tuple<string, string>(
                    "Windows.UI.Xaml.GridUnitType",                                             "Windows.UI.Xaml.GridUnitType"),
                new Tuple<string, string>(
                    "Windows.UI.Xaml.Thickness",                                                "Windows.UI.Xaml.Thickness"),
                new Tuple<string, string>(
                    "Windows.UI.Xaml.Controls.Primitives.GeneratorPosition",                    "Windows.UI.Xaml.Controls.Primitives.GeneratorPosition"),
                new Tuple<string, string>(
                    "Windows.UI.Xaml.Media.Matrix",                                             "Windows.UI.Xaml.Media.Matrix"),
                new Tuple<string, string>(
                    "Windows.UI.Xaml.Media.Animation.RepeatBehavior",                           "Windows.UI.Xaml.Media.Animation.RepeatBehavior"),
                new Tuple<string, string>(
                    "Windows.UI.Xaml.Media.Animation.RepeatBehaviorType",                       "Windows.UI.Xaml.Media.Animation.RepeatBehaviorType"),
                new Tuple<string, string>(
                    "Windows.UI.Xaml.Media.Animation.KeyTime",                                  "Windows.UI.Xaml.Media.Animation.KeyTime"),
                new Tuple<string, string>(
                    "Windows.UI.Xaml.Media.Media3D.Matrix3D",                                   "Windows.UI.Xaml.Media.Media3D.Matrix3D"),
                new Tuple<string, string>(
                    "System.Numerics.Matrix3x2",                                                "Windows.Foundation.Numerics.Matrix3x2"),
                new Tuple<string, string>(
                    "System.Numerics.Matrix4x4",                                                "Windows.Foundation.Numerics.Matrix4x4"),
                new Tuple<string, string>(
                    "System.Numerics.Plane",                                                    "Windows.Foundation.Numerics.Plane"),
                new Tuple<string, string>(
                    "System.Numerics.Quaternion",                                               "Windows.Foundation.Numerics.Quaternion"),
                new Tuple<string, string>(
                    "System.Numerics.Vector2",                                                  "Windows.Foundation.Numerics.Vector2"),
                new Tuple<string, string>(
                    "System.Numerics.Vector3",                                                  "Windows.Foundation.Numerics.Vector3"),
                new Tuple<string, string>(
                    "System.Numerics.Vector4",                                                  "Windows.Foundation.Numerics.Vector4"),
            };
        }

        public static bool AreAliases(string typeA, string typeB)
        {
            string[] templateA = GetTemplateArguments(typeA),
            templateB = GetTemplateArguments(typeB);

            var namesMatch = typeA == typeB;
            bool templatesMatch = templateA.Length == templateB.Length;

            if (templatesMatch)
            {
                for (int i = 0; i < templateA.Length; i++)
                {
                    templatesMatch &= templateA[i] == templateB[i] ||
                        (templateA[i] != string.Empty && templateB[i] != string.Empty && AreAliases(templateA[i], templateB[i]));
                }
            }

            if (templateA.Length != 0)
            {
                // Character '`' denotes start of template arguments
                typeA = typeA.Substring(0, typeA.IndexOf('`'));
            }

            if (templateB.Length != 0)
            {
                typeB = typeB.Substring(0, typeB.IndexOf('`'));
            }

            var areAliases = typeA == typeB ||
                _aliases.Contains(new Tuple<string, string>(typeA, typeB)) ||
                _aliases.Contains(new Tuple<string, string>(typeB, typeA));

            return namesMatch || (templatesMatch && areAliases);
        }

        public static string[] GetTemplateArguments(string type)
        {
            string template;

            int startIndex = type.IndexOf('<') + 1;
            int length = type.LastIndexOf('>') - startIndex;

            if (startIndex > 1 && length > 0)
            {
                template = type.Substring(startIndex, length);
            }
            else if (startIndex == 0 && length == -1)
            {
                template = string.Empty;
            }
            else
            {
                throw new ArgumentException("Invalid type name!", type);
            }

            var arguments = new List<string>();
            while (template.Length > 0)
            {
                int start = 0;
                uint depth = 0;
                string argument = null;
                do
                {
                    int idx = template.IndexOfAny(new char[] {',', '<', '>'}, start);
                    if (idx < 0)
                    {
                        argument = template;
                        template = "";
                    }
                    else
                    {
                        switch (template[idx])
                        {
                            case ',':
                                if (depth > 0)
                                    start = idx + 1;
                                else
                                {
                                    argument = template.Substring(0, idx);
                                    template = template.Remove(0, idx + 1);
                                }
                                break;
                            case '<':
                                ++depth;
                                start = idx + 1;
                                break;
                            case '>':
                                --depth;
                                start = idx + 1;
                                break;
                        }
                    }

                    if (!string.IsNullOrEmpty(argument))
                        arguments.Add(argument.Trim());
                }
                while (argument == null);
            }

            return arguments.ToArray();
        }
    }
}
