using System;

namespace SearchReferences
{
    public class WithConsoleColors : IDisposable
    {
        private ConsoleColor previousForeground;
        private ConsoleColor previousBackground;

        public WithConsoleColors(ConsoleColor background, ConsoleColor foreground)
        {
            previousBackground = Console.BackgroundColor;
            previousForeground = Console.ForegroundColor;

            Console.BackgroundColor = background;
            Console.ForegroundColor = foreground;
        }

        public void Dispose()
        {
            Console.BackgroundColor = previousBackground;
            Console.ForegroundColor = previousForeground;
        }
    }
}
