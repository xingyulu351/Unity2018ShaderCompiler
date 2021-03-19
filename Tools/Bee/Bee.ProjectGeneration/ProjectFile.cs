using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Bee.Tools;
using NiceIO;

namespace Unity.BuildSystem.NativeProgramSupport
{
    public class ProjectFile
    {
        // We want to keep project/solution GUIDs stable when rebuilding the projects; less work for IDE to reload
        // and allows diffing generated projects when doing generation code changes.
        // Note, to be 100% robust against anything imaginable, the algorithm would have to be something like
        // outlined in https://code.logos.com/blog/2011/04/generating_a_deterministic_guid.html
        // For our use cases though, just hashing the name and stuffing bytes into the GUID is fine.
        public static string GuidForName(string name, string guidFormat = "D")
        {
            byte[] bytes = Encoding.UTF8.GetBytes(name);
            byte[] hash = new System.Security.Cryptography.SHA1CryptoServiceProvider().ComputeHash(bytes);
            Array.Resize(ref hash, 16);
            return new Guid(hash).ToString(guidFormat).ToUpper();
        }

        public static Dictionary<NPath, NPath[]> _cache = new Dictionary<NPath, NPath[]>();

        public static IEnumerable<NPath> SourcePathToFiles(NPath path)
        {
            NPath[] result;
            if (_cache.TryGetValue(path, out result))
                return result;
            // Early out on common file extensions - faster and stops us from statting paths that we're not allowed to stat
            if (path.HasExtension("c", "cc", "cpp", "h", "hpp", "m", "mm"))
                result = new[] {path};
            else if (!path.DirectoryExists())
                result = new[] {path};
            else if (path.Extension == "nib")
            {
                // Some folders should be treated as "single file" for IDEs; mostly
                // on Mac/Xcode; e.g. interface builder nib's.
                result = new[] {path};
            }
            else
                result = path.Files(true).Where(p => !p.HasDirectory("obj") && !p.HasDirectory("bin"))
                    .ToArray();
            _cache.Add(path, result);
            return result;
        }
    }
}
