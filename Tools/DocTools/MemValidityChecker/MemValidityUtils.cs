using System.IO;
using UnderlyingModel.MemDoc;

namespace MemValidityChecker
{
    public static class MemValidityUtils
    {
        internal static string ValidateMemFile(string memFileName)
        {
            string memContent = File.ReadAllText(memFileName);
            return ValidateMemContent(memContent);
        }

        internal static string ValidateMemContent(string memContent)
        {
            var model = new MemDocModel();
            model.ParseFromString(memContent);
            return model.ErrorMessage;
        }
    }
}
