using System;
using System.Diagnostics;

namespace Unity.CommonTools
{
    internal class ScreenshotLinux : Screenshot
    {
        public override void SaveCombinedImage(string fileName)
        {
            var info = new ProcessStartInfo("import", string.Format("-window root '{0}'", fileName));
            info.UseShellExecute = true;

            try
            {
                using (Process p = Process.Start(info))
                    p.WaitForExit(60000); // one minute
            }
            catch (Exception)
            {
            }
        }

        public override void SaveImage(int desktopIdx, string fileName)
        {
            SaveCombinedImage(fileName);
        }

        public override int DesktopCount
        {
            get
            {
                return 1; // combined desktops for screenshot purposes
            }
        }

        public override void Dispose()
        {
        }
    }
}
