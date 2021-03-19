using System;
using System.Runtime.CompilerServices;

namespace Unity.BuildTools
{
    public static class Errors
    {
        public static void Exit(
            string message,
            [CallerFilePath] string file = "",
            [CallerLineNumber] int line = 0)
        {
            PrintErrorWithLocation(message, file, line);
            throw new Exception(message);
        }

        public static void PrintWarning(
            string message,
            [CallerFilePath] string file = "",
            [CallerLineNumber] int line = 0)
        {
            PrintWarningWithLocation(message, file, line);
        }

        public static void PrintWarningWithLocation(string message, string file, int line)
        {
            Console.ForegroundColor = ConsoleColor.Yellow;
            Print("warning", message, file, line);
            Console.ResetColor();
        }

        public static void PrintError(
            string message,
            [CallerFilePath] string file = "",
            [CallerLineNumber] int line = 0)
        {
            PrintErrorWithLocation(message, file, line);
        }

        public static void PrintErrorWithLocation(string message, string file, int line)
        {
            Console.ForegroundColor = ConsoleColor.Red;
            Print("error", message, file, line);
            Console.ResetColor();
        }

        private static void Print(string type, string message, string file, int line)
        {
            var location = HostPlatform.IsWindows
                ? $"{file}({line})" // MSVC error format
                : $"{file}:{line}"; // clang error format
            Console.WriteLine($"{location}: {type}: {message}");
        }
    }
}
