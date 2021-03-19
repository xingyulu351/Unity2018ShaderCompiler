namespace Unity.CommonTools
{
    public static class LogMessageFormatExtension
    {
        public static void Info(this ILog log, string format, params object[] args)
        {
            log.Info(string.Format(format, args));
        }

        public static void Warning(this ILog log, string format, params object[] args)
        {
            log.Warning(string.Format(format, args));
        }

        public static void Error(this ILog log, string format, params object[] args)
        {
            log.Error(string.Format(format, args));
        }
    }
}
