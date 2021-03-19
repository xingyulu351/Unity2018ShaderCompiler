using System;
using System.Collections.Generic;
using System.Linq;
using NUnit.Framework;

namespace BindingsToCsAndCpp
{
    [TestFixture]
    class TypeMapBuilderTests
    {
        [Test]
        public void TestTypeMap()
        {
            var m = MapBuilder.BuildFrom<Dictionary<string, string>>(new string[] { "one=>two", "three=>four" }, MapBuilder.MapType.Type);
            Assert.AreEqual("two", m["one"]);
            Assert.AreEqual("four", m["three"]);
        }
    }

    internal static class MapBuilder
    {
        public enum MapType
        {
            Type,
            Struct
        }

        public static T BuildFrom<T>(string[] lines, MapType type) where T : class
        {
            switch (type)
            {
                case MapType.Type:
                {
                    var result = new Dictionary<string, string>();
                    foreach (var line in lines.Where(l => l.Contains("=>")))
                    {
                        var k = line.Substring(0, line.IndexOf("=>"));
                        var v = line.Substring(line.IndexOf("=>") + 2);
                        result[k] = v;
                    }
                    return result as T;
                }
                case MapType.Struct:
                {
                    return new List<string>(lines).ToArray() as T;
                }
            }
            return null;
        }
    }
}
