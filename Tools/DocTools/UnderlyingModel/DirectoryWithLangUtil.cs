using System;
using System.IO;

namespace UnderlyingModel
{
    public static class DirectoryWithLangUtil
    {
        public static string GetPathFromMemberNameAndDir(string memberName, string dir)
        {
            dir = LocalizedDir(dir);
            return Path.Combine(dir, String.Format("{0}.{1}", memberName, DirectoryUtil.MemExtension));
        }

        public static string LocalizedDir(string path)
        {
            var locale = DocGenSettings.Instance.Language;
            var localizedPath = LanguageUtil.IsEnglish(locale) ? path : Path.Combine(path, locale.ToString());
            return localizedPath;
        }
    }
}
