using System;
using System.IO;
using System.Linq;
using System.Text;
using NiceIO;

namespace Bee.NativeProgramSupport.Running
{
    public static class NPathWithoutInvalidCharactersExtension
    {
        private static bool IsSlash(char c) => c == '/' || c == '\\';

        public static NPath WithoutInvalidCharacters(this NPath path, char replacementChar)
        {
            var invalidFileNameChars = Path.GetInvalidFileNameChars();
            if (invalidFileNameChars.Contains(replacementChar))
                throw new ArgumentException($"Replacement character '{replacementChar}' is itself an invalid file name character", nameof(replacementChar));

            string oldPathString = path.ToString();
            var newPath = new StringBuilder(oldPathString.Length);
            for (int i = 0; i < oldPathString.Length; ++i)
            {
                if (!IsSlash(oldPathString[i]) && (i != 1 || oldPathString[i] != ':') && // Explicitely allow ':' in second place for windows drive letters.
                    invalidFileNameChars.Contains(oldPathString[i]))
                {
                    newPath.Append(replacementChar);
                }
                else
                    newPath.Append(oldPathString[i]);
            }

            return new NPath(newPath.ToString());
        }
    }
}
