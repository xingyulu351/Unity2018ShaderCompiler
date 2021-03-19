using System.Collections.Generic;
using System.Linq;
using NiceIO;

namespace JamSharp.Runtime
{
    public static class HandwrittenRuleReplacements
    {
        public static JamList FilterExtensions(JamList files, JamList extensions)
        {
            var extensionsHash = new HashSet<string>(extensions.Elements);
            return new JamList(files.Elements.Where(file => extensionsHash.Contains(ExtensionOf(file))));
        }

        static string ExtensionOf(string file)
        {
            string newResult = new NPath(file.Contains("<") ? file.Replace("<", "_").Replace(">", "_") : file).Extension;
            if (newResult != string.Empty)
                newResult = "." + newResult;
            return newResult;
        }
    }
}
