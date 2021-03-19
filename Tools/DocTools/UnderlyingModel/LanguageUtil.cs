using System;
using System.Collections.Generic;
using System.Linq;

namespace UnderlyingModel
{
    public static class LanguageUtil
    {
        public enum ELanguage
        {
            en,
            ru,
            pt_BR,
            es,
            ko,
            zh_CN,
            ja
        }

        public static string[] FakeSuffix = { "", "ski", "eiro", "hyo", "lee", "ito", "" };
        public static string[] Postfix = { "", "ru", "pt", "ko", "cn", "jp", "" };

        public static IEnumerable<ELanguage> GetAllLanguages()
        {
            return Enum.GetValues(typeof(ELanguage)).Cast<ELanguage>();
        }

        public static IEnumerable<ELanguage> GetForeignLanguages()
        {
            return Enum.GetValues(typeof(ELanguage)).Cast<ELanguage>().Where(IsForeignLanguage);
        }

        private static bool IsForeignLanguage(ELanguage lang)
        {
            return lang != ELanguage.en;
        }

        public static bool IsEnglish(ELanguage lang)
        {
            return lang == ELanguage.en;
        }

        public static ELanguage StringToLocale(String sLanguage)
        {
            if (null == sLanguage)
                return ELanguage.en;
            try
            {
                var result = (ELanguage)Enum.Parse(typeof(ELanguage), sLanguage);
                return result;
            }
            catch (ArgumentException)
            {
                Console.Out.WriteLine("unknown locale given:{0}", sLanguage);

                return ELanguage.en;
            }
        }
    }
}
