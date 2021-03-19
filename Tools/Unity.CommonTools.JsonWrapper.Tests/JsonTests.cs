using System;
using System.Collections;
using System.Collections.Generic;
using System.Text;
using NUnit.Framework;

namespace Unity.CommonTools.JsonWrapper.Tests
{
    [TestFixture]
    public class JsonTests
    {
        [Test]
        public void Can_Parse_Empty_Object()
        {
            var result = Parse("{}");
            Assert.That(result, Is.Empty);
        }

        [Test]
        public void Can_Parse_String()
        {
            var result = Parse("\"name\"");
            Assert.That(result, Is.EqualTo("name"));
        }

        [Test]
        public void Can_Parse_Integer()
        {
            var result = Parse("23342");
            Assert.That(result, Is.EqualTo(23342));
        }

        [Test]
        public void Can_Parse_Negative_Integer()
        {
            var result = Parse("-23342");
            Assert.That(result, Is.EqualTo(-23342));
        }

        [Test]
        public void Can_Parse_Double()
        {
            var result = Parse("23342.123");
            Assert.That(result, Is.EqualTo(23342.123));
        }

        [Test]
        public void Can_Exponential_Double()
        {
            Assert.That(Parse("1.7E+3"), Is.EqualTo(1.7E+3));
        }

        [Test]
        public void Can_Parse_Negative_Double()
        {
            var result = Parse("-23342.123");
            Assert.That(result, Is.EqualTo(-23342.123));
        }

        [Test]
        public void Can_Parse_Boolean_True()
        {
            var result = Parse("true");
            Assert.That(result, Is.True);
        }

        [Test]
        public void Can_Parse_Boolean_False()
        {
            var result = Parse("false");
            Assert.That(result, Is.False);
        }

        [Test]
        public void Can_Parse_Json_Null()
        {
            var result = Parse("null");
            Assert.That(result, Is.Null);
        }

        [Test]
        public void Can_Parse_Empty_Array()
        {
            var result = Parse("[]");
            Assert.That(result, Is.InstanceOf<IList>().And.Empty);
        }

        [Test]
        public void Can_Parse_Array_With_Single_Number()
        {
            var result = Parse("[42]");
            Assert.That(ToArray(result), Is.EqualTo(new object[] { 42 }));
        }

        [Test]
        public void Can_Parse_Array_With_2_Numbers()
        {
            var result = Parse("[42,43]");
            Assert.That(ToArray(result), Is.EqualTo(new object[] { 42, 43 }));
        }

        [Test]
        public void Can_Parse_Array_With_String_And_Number()
        {
            var result = Parse("[\"42\",43]");
            Assert.That(ToArray(result), Is.EqualTo(new object[] { "42", 43 }));
        }

        [Test]
        public void Can_Parse_Object_With_Single_Number_Field()
        {
            var result = Parse("{\"state\":3}");
            var state = ((IDictionary)result)["state"];
            Assert.That(state, Is.EqualTo(3));
        }

        [Test]
        public void Can_Parse_Json_With_Json_String_Inside()
        {
            var result = (IDictionary)Parse("{\"key\":\"{\\\"innerkey\\\":\\\"value\\\"}\"}");
            var expected = new Dictionary<string, object>();
            expected.Add("key", "{\"innerkey\":\"value\"}");
            Assert.That(result.Keys, Is.EqualTo(expected.Keys));
            Assert.That(result["key"], Is.EqualTo(expected["key"]));
        }

        [Test]
        [TestCase("json value with \\\" char", "json value with \" char")]
        [TestCase("json value with \\/ char", "json value with / char")]
        [TestCase("json value with \\n char", "json value with \n char")]
        [TestCase("json value with \\t char", "json value with \t char")]
        [TestCase("json value with \\r char", "json value with \r char")]
        [TestCase("json value with \\b char", "json value with \b char")]
        [TestCase("json value with \\u0058 char", "json value with X char")]
        public void Can_Parse_StringWithOtherSpecialChars(string text, string expected)
        {
            var sb = new StringBuilder();
            sb.Append('{');
            sb.Append('"');
            sb.Append("message");
            sb.Append('"');
            sb.Append(':');
            sb.Append('"');
            sb.Append(text);
            sb.Append('"');
            sb.Append('}');

            var result = (IDictionary)Parse(sb.ToString());

            Assert.That(result["message"].ToString(), Is.EqualTo(expected));
        }

        private static object Parse(string json)
        {
            return Json.Deserialize(json);
        }

        private static Array ToArray(object list)
        {
            if (list == null)
            {
                return null;
            }
            return (new ArrayList((IList)list)).ToArray();
        }
    }
}
