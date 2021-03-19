using System;

namespace Unity.CommonTools
{
    public class ConsoleLog : ILog
    {
        public string LogPrefix { get; set; }

        public ConsoleLog() {}

        public ConsoleLog(string logPrefix)
        {
            LogPrefix = logPrefix;
        }

        public void Info(string message)
        {
            Console.WriteLine(GetDecoratedMessage(message));
        }

        public void Warning(string message)
        {
            Console.WriteLine(GetDecoratedMessage(message));
        }

        public void Error(string message)
        {
            Console.Error.WriteLine(GetDecoratedMessage(message));
        }

        private string GetDecoratedMessage(string message)
        {
            return string.Format(LogPrefix + "{0}", message);
        }
    }
}
