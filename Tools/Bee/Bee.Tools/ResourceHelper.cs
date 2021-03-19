using System;
using System.IO;
using System.Reflection;
using NiceIO;

namespace Unity.BuildTools
{
    public static class ResourceHelper
    {
        public static string ReadTextFromResource(Assembly assembly, string resourceID)
        {
            using (Stream stream = assembly.GetManifestResourceStream(resourceID))
            {
                if (stream == null)
                    throw new ArgumentException($"Unable to find resource {resourceID} in {assembly.FullName}");
                using (StreamReader reader = new StreamReader(stream))
                {
                    return reader.ReadToEnd();
                }
            }
        }

        public static void WriteResourceToDisk(Assembly assembly, string resourceID, NPath path)
        {
            var resource = assembly.GetManifestResourceStream(resourceID);
            if (resource == null)
                throw new ArgumentException($"Unable to find {resourceID} in {assembly.FullName}");
            path.MakeAbsolute().EnsureParentDirectoryExists();
            using (var file = new FileStream(path.ToString(SlashMode.Native), FileMode.Create, FileAccess.Write))
            {
                resource.CopyTo(file);
            }
        }
    }
}
