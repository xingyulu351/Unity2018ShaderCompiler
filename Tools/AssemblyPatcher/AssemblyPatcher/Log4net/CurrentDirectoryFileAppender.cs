using System.IO;
using log4net.Appender;

namespace AssemblyPatcher.Log4net
{
    public class CurrentDirectoryFileAppender : FileAppender
    {
        public override string File
        {
            set
            {
                base.File = Path.Combine(Directory.GetCurrentDirectory(), value);
            }
        }
    }
}
