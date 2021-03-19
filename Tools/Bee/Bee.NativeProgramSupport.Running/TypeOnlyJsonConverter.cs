using System;
using System.CodeDom;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using System.Reflection;
using Newtonsoft.Json;
using Unity.BuildTools;

namespace Bee.NativeProgramSupport.Running
{
    // Json converter that writes only the (unqualified, not namespaced) type out as string.
    //
    // Useful for objects that are fully defined by their type alone (like Architecture or Platform)
    public class TypeOnlyJsonConverter : JsonConverter
    {
        private static PropertyInfo GetStaticAllProperty(Type type)
        {
            return type.GetProperties().FirstOrDefault(x =>
                x.Name == "All" &&
                x.CanRead &&
                x.PropertyType.IsGenericType &&
                x.PropertyType.GetGenericTypeDefinition() == typeof(IEnumerable<>) &&
                x.PropertyType.GenericTypeArguments[0] == type);
        }

        public override void WriteJson(JsonWriter writer, object value, JsonSerializer serializer)
        {
            if (value == null)
                writer.WriteNull();
            else if (value.GetType().IsArray)
            {
                writer.WriteStartArray();
                foreach (var obj in (Array)value)
                    writer.WriteValue(obj.GetType().Name);
                writer.WriteEndArray();
            }
            else
                writer.WriteValue(value.GetType().Name);
        }

        private static object InstantiateObjectFromType(string typeString, Type objectType)
        {
            string typeNameFromJson = typeString;
            var allValues = (IEnumerable<object>)GetStaticAllProperty(objectType).GetValue(null, null);
            return allValues.FirstOrDefault(x => x.GetType().Name == typeNameFromJson);
        }

        public override object ReadJson(JsonReader reader, Type objectType, object existingValue, JsonSerializer serializer)
        {
            if (reader.TokenType == JsonToken.Null)
                return null;
            if (reader.TokenType == JsonToken.StartArray)
            {
                Type elementType = objectType.GetElementType();

                var elements = (IList)Activator.CreateInstance((typeof(List<>).MakeGenericType(elementType)));
                while (reader.Read() && reader.TokenType != JsonToken.EndArray)
                    elements.Add(InstantiateObjectFromType((string)reader.Value, elementType));

                // Copy over into strongly typed array.
                Array array = Array.CreateInstance(elementType, elements.Count);
                elements.CopyTo(array, 0);
                return array;
            }

            return InstantiateObjectFromType((string)reader.Value, objectType);
        }

        public override bool CanConvert(Type objectType)
        {
            if (objectType.IsArray)
                return GetStaticAllProperty(objectType) != null;
            else
                return GetStaticAllProperty(objectType.GetElementType()) != null;
        }
    }
}
