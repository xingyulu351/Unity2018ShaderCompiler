using System;
using System.Collections.Generic;
using System.Drawing;
using System.Runtime.InteropServices;

namespace Unity.CommonTools
{
    internal class ScreenshotWin : Screenshot
    {
        private List<Image> Desktops { get; set; }
        private Image VirtualDesktop { get { return Desktops.Count == 1 ? Desktops[0] : CombineImages(); } }

        internal ScreenshotWin()
        {
            Desktops = new List<Image>();
            _virtualDesktopSize = new Size(
                User32.GetSystemMetrics(User32.SM_CXVIRTUALSCREEN),
                User32.GetSystemMetrics(User32.SM_CYVIRTUALSCREEN));
            _positionsOfDesktops = new List<Point>();
            CaptureImagesOfScreens();
        }

        public override void SaveCombinedImage(string fileName)
        {
            VirtualDesktop.Save(fileName);
        }

        public override void SaveImage(int desktopIdx, string fileName)
        {
            Desktops[desktopIdx].Save(fileName);
        }

        public override int DesktopCount
        {
            get
            {
                return Desktops.Count;
            }
        }

        public override void Dispose()
        {
            foreach (var desktop in Desktops)
            {
                desktop.Dispose();
            }
            GC.SuppressFinalize(this);
        }

        Size _virtualDesktopSize;
        List<Point> _positionsOfDesktops;

        Image CombineImages()
        {
            var virtualDesktop = new Bitmap(_virtualDesktopSize.Width, _virtualDesktopSize.Height);
            using (var virtualDesktopGraphics = Graphics.FromImage(virtualDesktop))
            {
                for (var i = 0; i != Desktops.Count; ++i)
                {
                    virtualDesktopGraphics.DrawImageUnscaled(Desktops[i], _positionsOfDesktops[i]);
                }
                return virtualDesktop;
            }
        }

        void CaptureImagesOfScreens()
        {
            var originOffset = new Point(
                -User32.GetSystemMetrics(User32.SM_XVIRTUALSCREEN),
                -User32.GetSystemMetrics(User32.SM_YVIRTUALSCREEN));
            using (var desktop = Graphics.FromHwnd(IntPtr.Zero))
            {
                User32.EnumDisplayMonitors(desktop.GetHdc(), IntPtr.Zero,
                    delegate(IntPtr hMonitor, IntPtr hdcMonitor, ref User32.Rect lprcMonitor, IntPtr dwData)
                    {
                        Desktops.Add(CaptureDcImage(hdcMonitor, lprcMonitor));
                        _positionsOfDesktops.Add(new Point(
                            lprcMonitor.left + originOffset.X,
                            lprcMonitor.top + originOffset.Y));
                        return true;
                    }, IntPtr.Zero);
                desktop.ReleaseHdc();
            }
        }

        static Image CaptureDcImage(IntPtr dc, User32.Rect rect)
        {
            using (var graphics = Graphics.FromHdc(dc))
            {
                var bitmap = new Bitmap(rect.Width, rect.Height, graphics);
                using (var graphicsCopy = Graphics.FromImage(bitmap))
                {
                    Gdi32.BitBlt(graphicsCopy.GetHdc(), 0, 0, rect.Width, rect.Height, dc, rect.left, rect.top,
                        Gdi32.SRCCOPY);
                    graphicsCopy.ReleaseHdc();
                    return bitmap;
                }
            }
        }

        static class Gdi32
        {
            public const int SRCCOPY = 0x00CC0020;
            [DllImport("gdi32.dll")]
            public static extern bool BitBlt(IntPtr hObject, int nXDest, int nYDest,
                int nWidth, int nHeight, IntPtr hObjectSource,
                int nXSrc, int nYSrc, int dwRop);
        }

        static class User32
        {
            public const int SM_XVIRTUALSCREEN = 76;
            public const int SM_YVIRTUALSCREEN = 77;
            public const int SM_CXVIRTUALSCREEN = 78;
            public const int SM_CYVIRTUALSCREEN = 79;
            [StructLayout(LayoutKind.Sequential)]
            public struct Rect
            {
                public int left;
                public int top;
                public int right;
                public int bottom;
                public int Width { get { return right - left; } }
                public int Height { get { return bottom - top; } }
            }
            public delegate bool MonitorEnumDelegate(IntPtr hMonitor, IntPtr hdcMonitor, ref Rect lprcMonitor, IntPtr dwData);
            [DllImport("user32.dll")]
            public static extern int GetSystemMetrics(int nIndex);
            [DllImport("user32.dll")]
            public static extern bool EnumDisplayMonitors(IntPtr hdc, IntPtr lprcClip, MonitorEnumDelegate lpfnEnum, IntPtr dwData);
        }
    }
}
