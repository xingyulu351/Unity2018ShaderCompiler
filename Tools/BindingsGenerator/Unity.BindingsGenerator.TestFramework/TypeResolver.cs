using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Reflection;
using Mono.Cecil;
using Unity.CommonTools;

namespace Unity.BindingsGenerator.TestFramework
{
    public class TypeResolver
    {
        private static readonly Dictionary<string, AssemblyDefinition> Assemblies = new Dictionary<string, AssemblyDefinition>();

        private static DefaultAssemblyResolver _assemblyResolver = null;

        private static string BuildDir
        {
            get
            {
#if DEBUG
                return Paths.Combine(Workspace.BasePath, "Tools", "BindingsGenerator", "build", "bin", "Debug");
#else
                return Paths.Combine(Workspace.BasePath, "Tools", "BindingsGenerator", "build", "bin", "Release");
#endif
            }
        }

        public static string GetAssemblyPathNextToExecutingAssembly(string assemblyName)
        {
            var assemblyDirectory = Path.GetDirectoryName(new Uri(Assembly.GetExecutingAssembly().CodeBase).LocalPath);
            return Path.Combine(assemblyDirectory, assemblyName);
        }

        public static AssemblyDefinition LoadAssembly(string assemblyName)
        {
            if (_assemblyResolver == null)
            {
                _assemblyResolver = new DefaultAssemblyResolver();
                _assemblyResolver.AddSearchDirectory(BuildDir);
            }

            var assemblyPath = assemblyName;

            if (!Path.IsPathRooted(assemblyPath))
                assemblyPath = GetAssemblyPathNextToExecutingAssembly(assemblyPath);

            return AssemblyDefinition.ReadAssembly(assemblyPath, new ReaderParameters { AssemblyResolver = _assemblyResolver });
        }

        public static TypeReference GetTypeReference<T>()
        {
            return GetTypeReference(typeof(T));
        }

        public static TypeReference GetTypeReference(Type type)
        {
            if (type.IsArray)
            {
                TypeReference elementType = GetTypeReference(type.GetElementType());
                return new ArrayType(elementType);
            }

            if (type.IsNested)
            {
                var declaringType = GetTypeReference(type.DeclaringType);
                return declaringType != null ? declaringType.Resolve().NestedTypes.FirstOrDefault(t => t.Name == type.Name) : null;
            }

            EnsureAssemblyForTypeIsLoaded(type);

            return Assemblies.Values.Select(assembly => assembly.MainModule.GetType(type.FullName)).FirstOrDefault(typeDefinition => typeDefinition != null);
        }

        public static TypeDefinition TypeDefinitionFor<T>()
        {
            return TypeDefinitionFor(typeof(T));
        }

        public static TypeDefinition TypeDefinitionFor(Type type)
        {
            return GetTypeReference(type).Resolve();
        }

        private static void EnsureAssemblyForTypeIsLoaded(Type type)
        {
            var location = type.Assembly.Location;

            if (!Assemblies.ContainsKey(location))
            {
                var assembly = LoadAssembly(location);
                Assemblies.Add(location, assembly);
            }
        }
    }
}
