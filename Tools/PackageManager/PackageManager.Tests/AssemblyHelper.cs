using System;
using System.IO;
using System.Linq;
using System.Reflection;
using System.Collections;
using System.Collections.Generic;

namespace Unity.PackageManager
{
    class AssemblyHelper
    {
        internal static Type[] GetTypesFromAssembly(Assembly assembly)
        {
            try
            {
                return assembly.GetTypes();
            }
            catch (ReflectionTypeLoadException)
            {
                return new Type[] {};
            }
        }

        internal static IEnumerable<T> FindImplementors<T>(Assembly assembly) where T : class
        {
            Type interfaze = typeof(T);
            foreach (Type type in AssemblyHelper.GetTypesFromAssembly(assembly))
            {
                if (/*type.IsNotPublic - future! ||*/ type.IsInterface || type.IsAbstract || !interfaze.IsAssignableFrom(type))
                    continue;
                T module = Activator.CreateInstance(type) as T;
                if (module != null)
                    yield return module;
            }
        }
    }
}
