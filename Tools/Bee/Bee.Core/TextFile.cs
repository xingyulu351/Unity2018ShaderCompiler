using NiceIO;

namespace Bee.Core
{
    public class TextFile
    {
        // Does everything that is needed to "write" a text file as part of a build
        // process.
        public static NPath Setup(NPath path, string contents, string actionName = null)
        {
            Backend.Current.AddWriteTextAction(path, contents, actionName);
            return path;
        }
    }
}
