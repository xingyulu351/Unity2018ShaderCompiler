using System;
using System.Collections.Generic;
using System.Linq;
using Newtonsoft.Json.Linq;
using NiceIO;

namespace Bee.Why
{
    internal class BuiltNodeInfo
    {
        public BuiltNodeInfo(string annotation, int originalDagIndex, IEnumerable<RebuildReason> rebuildReasons)
        {
            if (string.IsNullOrEmpty(annotation))
                throw new ArgumentNullException(nameof(annotation));

            Annotation = annotation;
            OriginalDagIndex = originalDagIndex;
            RebuildReasons = rebuildReasons?.ToList() ?? new List<RebuildReason>();
        }

        public IList<RebuildReason> RebuildReasons { get; }

        public string Annotation { get; }
        public int OriginalDagIndex { get; }
    }
}
