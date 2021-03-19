using System;
using System.Collections.Generic;
using System.Linq;
using NiceIO;

namespace Bee.Core
{
    public static class FilteredGlob
    {
        public static IEnumerable<NPath> RecursiveGlob(NPath directory, string pattern, Func<NPath, bool> filter)
        {
            foreach (var file in directory.Files(pattern, false).Where(filter))
                yield return file;
            foreach (var subdir in directory.Directories())
            {
                var filesInSubDir = RecursiveGlob(subdir, pattern, filter);
                foreach (var file in filesInSubDir)
                    yield return file;
            }
        }
    }
}
