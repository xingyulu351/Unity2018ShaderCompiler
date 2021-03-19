using System;
using System.Linq;
using NiceIO;
using Unity.BuildTools;

namespace Bee.Core.Tests
{
    public struct SerializedFunction
    {
        public string Assembly;
        public string Type;
        public string Method;

        public static SerializedFunction From(Delegate d)
        {
            var fieldInfos = d.Method.DeclaringType.GetFields().Where(f => !f.IsStatic);
            if (fieldInfos.Any() && !d.Method.IsStatic)
                throw new ArgumentException(
                    $"The lambda you are trying to convert is capturing variables {fieldInfos.Select(f => f.Name).SeparateWithComma()}, which is not allowed. ");

            return new SerializedFunction()
            {
                Assembly = new NPath(d.Method.DeclaringType.Assembly.Location).ToString(),
                Method = d.Method.Name,
                Type = d.Method.DeclaringType.FullName
            };
        }

        public string[] AsStrings => new[] {Assembly, Type, Method};
    }
}
