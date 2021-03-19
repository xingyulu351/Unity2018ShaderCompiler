using System;

namespace Unity.CommonTools
{
    public abstract class Screenshot : IDisposable
    {
        public abstract void SaveCombinedImage(string fileName);

        public abstract void SaveImage(int desktopIdx, string fileName);

        public abstract int DesktopCount { get; }

        public static Screenshot Make()
        {
            if (Workspace.IsOSX())
            {
                return new ScreenshotMac();
            }

            if (Workspace.IsWindows())
            {
                return new ScreenshotWin();
            }

            if (Workspace.IsLinux())
                return new ScreenshotLinux();

            throw new NotSupportedException("Platform is not supported");
        }

        public abstract void Dispose();
    }
}
