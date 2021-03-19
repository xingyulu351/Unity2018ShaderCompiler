using System;
using System.Collections.Generic;
using System.Linq;
using System.Reflection;
using NiceIO;
using Unity.BuildTools;

namespace Bee.Core
{
    public class DynamicLoader
    {
        public static IEnumerable<Type> GetAllTypesInAssemblies(string searchPattern, bool addCallingAssembly = false)
        {
            return GetAllTypesInAssemblies(searchPattern, addCallingAssembly ? Assembly.GetCallingAssembly() : null);
        }

        private static IEnumerable<Type> GetAllTypesInAssemblies(string searchPattern, Assembly callingAssembly)
        {
            var assemblies = new NPath(typeof(DynamicLoader).Assembly.Location)
                .Parent
                .Files(searchPattern)
                .Select(p => Assembly.LoadFrom(p.ToString(SlashMode.Native)))
                .Append(typeof(DynamicLoader).Assembly);
            if (callingAssembly != null)
                assemblies = assemblies.Append(callingAssembly);
            assemblies = assemblies.Distinct();

            try
            {
                return assemblies.SelectMany(t => t.GetTypes());
            }
            catch (ReflectionTypeLoadException rtle)
            {
                if (rtle.LoaderExceptions.Length == 0)
                    throw;
                throw rtle.LoaderExceptions[0];
            }
        }

        public static IEnumerable<T> FindAndCreateInstanceOfEachInAllAssemblies<T>(string searchPattern, bool addCallingAssembly = false)
        {
            var allTypes = GetAllTypesInAssemblies(searchPattern, addCallingAssembly ? Assembly.GetCallingAssembly() : null).ToList();
            var types = allTypes
                .Where(t => typeof(T).IsAssignableFrom(t) && !t.IsAbstract)
                .Where(HasDefaultConstructor)
                .ToList();

            List<T> regularInstances = new List<T>();
            Type currentlyProcessing = null;
            try
            {
                foreach (var type in types)
                {
                    currentlyProcessing = type;
                    regularInstances.Add((T)Activator.CreateInstance(type));
                }
            }
            catch (Exception e)
            {
                throw new Exception($"Caught exception when invoking CreateInstance on {currentlyProcessing}", e);
            }

            var instancesThroughDynamicLoader = allTypes
                .Where(t => typeof(DynamicLoaderFor<T>).IsAssignableFrom(t) && !t.IsAbstract)
                .Select(t => (DynamicLoaderFor<T>)Activator.CreateInstance(t))
                .SelectMany(loader => loader.Provide())
                .ToList();

            return regularInstances.Concat(instancesThroughDynamicLoader);
        }

        private static bool HasDefaultConstructor(Type t)
        {
            return t.GetConstructor(Array.Empty<Type>()) != null;
        }
    }

    public abstract class DynamicLoaderFor<T>
    {
        public abstract IEnumerable<T> Provide();
    }
}
