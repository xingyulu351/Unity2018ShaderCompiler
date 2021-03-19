using System;
using System.Collections.Generic;
using System.Linq;

namespace Bee.ProjectGeneration.XCode
{
    class XCodeNode
    {
        public string Id { get; }
        public readonly List<KeyValuePair<string, object>> Members = new List<KeyValuePair<string, object>>();
        public List<XCodeNode> Children => (List<XCodeNode>)Members.First(kvp => kvp.Key == "children").Value;

        internal XCodeNode(string id)
        {
            Id = id;
        }

        public void Add(string key, object value)
        {
            if (!IsValidType(value))
                throw new ArgumentException();
            Members.Add(new KeyValuePair<string, object>(key, value));
        }

        static bool IsValidType(object value)
        {
            if (value is string)
                return true;
            if (value is IEnumerable<string>)
                return true;
            if (value is XCodeNode)
                return true;
            if (value is IEnumerable<XCodeNode>)
                return true;
            return false;
        }

        public string GetStringMember(string key)
        {
            return Members.FirstOrDefault(kvp => kvp.Key == key).Value?.ToString() ?? "";
        }
    }
}
