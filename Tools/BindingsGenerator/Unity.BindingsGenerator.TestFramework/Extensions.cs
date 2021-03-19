using System;
using System.Linq;
using System.Text.RegularExpressions;
using Unity.BindingsGenerator.Core;
using Unity.BindingsGenerator.Core.Nodes;

namespace Unity.BindingsGenerator.TestFramework
{
    public static class Extensions
    {
        public static T Cast<T>(this object obj)
        {
            return (T)obj;
        }

        public static FunctionNode FindFunction(this FileNode fileNode, string methodName)
        {
            var r = new Regex(String.Format("_{0}$", methodName));
            return fileNode.DefaultNamespace.Functions.FirstOrDefault(f => r.Match(f.Name).Success);
        }

        public static FunctionNode FindMemberFunction(this FileNode fileNode, string klassName, string methodName)
        {
            var r = new Regex(String.Format("{0}_{1}$", klassName, methodName));
            return fileNode.DefaultNamespace.Functions.FirstOrDefault(f => r.Match(f.Name).Success);
        }

        public static StructDefinitionNode FindStruct(this FileNode fileNode, string structName)
        {
            return fileNode.DefaultNamespace.Structs.FirstOrDefault(s => s.Name == structName);
        }

        public static string RegisteredName(this Type type)
        {
            return Naming.BindingsTypePrefix + type.FullnameWithUnderscores();
        }

        public static string FullnameWithUnderscores(this Type type)
        {
            return TypeResolver.GetTypeReference(type).NamespaceAndNestedName().Replace('.', '_');
        }
    }
}
