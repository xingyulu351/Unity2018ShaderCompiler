using System;
using System.Runtime.InteropServices;

namespace Unity.CommonTools
{
    internal class ScreenshotMac : Screenshot
    {
        internal ScreenshotMac()
        {
            CaptureScreenImages();
        }

        public override void SaveCombinedImage(string fileName)
        {
            CombineDesktops().Save(fileName);
        }

        public override void SaveImage(int desktopIdx, string fileName)
        {
            m_desktops[desktopIdx].Save(fileName);
        }

        public override int DesktopCount
        {
            get
            {
                return m_desktops.Length;
            }
        }

        public override void Dispose()
        {
            if (m_desktops != null)
            {
                foreach (var d in m_desktops)
                {
                    d.Dispose();
                }
            }
            m_desktops = null;
        }

        void CaptureScreenImages()
        {
            var desktopIds = GetActiveDisplayList();
            m_desktops = new CGImage[desktopIds.Length];
            for (var i = 0; i != desktopIds.Length; ++i)
            {
                m_desktops[i] = CGImage.FromDisplayId(desktopIds[i]);
            }
        }

        CGImage CombineDesktops()
        {
            uint combined_width = 0;
            uint combined_height = 0;
            foreach (var desktop in m_desktops)
            {
                combined_width = Math.Max(combined_width, desktop.Width);
                combined_height += desktop.Height;
            }
            uint vertical_offset = 0;
            var drawing_context = CGBitmapContext.CreateCompatibleToImage(
                m_desktops[0], combined_width, combined_height, null);
            foreach (var desktop in m_desktops)
            {
                var rect = new CGRect(0, vertical_offset, desktop.Width, desktop.Height);
                drawing_context.DrawImage(rect, desktop);
                vertical_offset += desktop.Height;
            }
            return drawing_context.ToImage();
        }

        public class Exception : System.Exception
        {
            public Exception(string description) : base(description) {}
        }

        CGImage[] m_desktops;

        static int[] GetActiveDisplayList()
        {
            var displays = new int[32];
            var count = 0;
            CheckCGError(CGGetActiveDisplayList(32, displays, ref count));
            Array.Resize(ref displays, count);
            return displays;
        }

        static void CheckCGError(int error_code)
        {
            if (error_code != 0)
                throw new Exception("CGError: " + error_code);
        }

        struct CGRect
        {
            public float x;
            public float y;
            public float width;
            public float height;
            public CGRect(float x, float y, float width, float height)
            {
                this.x = x;
                this.y = y;
                this.width = width;
                this.height = height;
            }
        }

        struct CGRect64
        {
            public double x;
            public double y;
            public double width;
            public double height;
            public CGRect64(CGRect rect32)
            {
                x = rect32.x;
                y = rect32.y;
                width = rect32.width;
                height = rect32.height;
            }
        }

        class CFString : IDisposable
        {
            public CFString(string value)
            {
                Handle = CFStringCreateWithCharacters(IntPtr.Zero, value, value.Length);
                if (Handle == IntPtr.Zero) throw new Exception("Failed to allocate CFString " + value);
            }

            public IntPtr Handle { get; private set; }

            ~CFString()
            {
                Dispose();
            }

            public void Dispose()
            {
                if (Handle != IntPtr.Zero)
                {
                    CFRelease(Handle);
                }
                Handle = IntPtr.Zero;
            }

            [DllImport(CoreFoundationLibrary, CharSet = CharSet.Unicode)]
            extern static IntPtr CFStringCreateWithCharacters(IntPtr allocator, string str, int count);
        }

        class CFURL : IDisposable
        {
            public IntPtr Handle { get; private set; }

            CFURL(IntPtr urlref)
            {
                if (urlref == IntPtr.Zero) throw new ArgumentNullException("urlref");
                Handle = urlref;
            }

            public static CFURL FromFileSystemPath(string path, bool isDirectory)
            {
                var cfpath = new CFString(path);
                var urlref = CFURLCreateWithFileSystemPath(IntPtr.Zero, cfpath.Handle, kCFURLPOSIXPathStyle, isDirectory);
                if (urlref == IntPtr.Zero) throw new Exception("Failed to create CFURL for path: " + path);
                return new CFURL(urlref);
            }

            ~CFURL()
            {
                Dispose();
            }

            public void Dispose()
            {
                if (Handle != IntPtr.Zero)
                {
                    CFRelease(Handle);
                }
                Handle = IntPtr.Zero;
            }

            const int kCFURLPOSIXPathStyle = 0;

            [DllImport(CoreFoundationLibrary)]
            extern static IntPtr CFURLCreateWithFileSystemPath(IntPtr allocator, IntPtr filePath, int pathStyle, bool isDirectory);
        }

        class CGImage : IDisposable
        {
            public IntPtr Handle { get; private set; }

            public CGImage(IntPtr imageref)
            {
                if (imageref == IntPtr.Zero) throw new ArgumentNullException("imageref");
                Handle = imageref;
            }

            public static CGImage FromDisplayId(int displayId)
            {
                var imageref = CGDisplayCreateImage(displayId);
                if (imageref == IntPtr.Zero) throw new Exception("Failed to take screenshot from display " + displayId);
                return new CGImage(imageref);
            }

            public void Save(string filename)
            {
                using (var saveurl = CFURL.FromFileSystemPath(filename, false))
                {
                    using (var destination = CGImageDestination.FromUrl(saveurl, kUTTypePNG, 1))
                    {
                        destination.AddImage(this);
                        destination.Finish();
                    }
                }
            }

            public uint Width { get { return CGImageGetWidth(Handle); } }

            public uint Height { get { return CGImageGetHeight(Handle); } }

            public uint BitsPerComponent { get { return CGImageGetBitsPerComponent(Handle); } }

            public uint BytesPerRow { get { return CGImageGetBytesPerRow(Handle); } }

            public IntPtr ColorSpace { get { return CGImageGetColorSpace(Handle); } }

            public uint BitmapInfo { get { return CGImageGetBitmapInfo(Handle); } }

            ~CGImage()
            {
                Dispose();
            }

            public void Dispose()
            {
                CGImageRelease(Handle);
                Handle = IntPtr.Zero;
            }

            const string kUTTypePNG = "public.png";

            [DllImport(CoreGraphicsLibrary)]
            extern static void CGImageRelease(IntPtr handle);

            [DllImport(CoreGraphicsLibrary)]
            extern static IntPtr CGDisplayCreateImage(int displayId);

            [DllImport(CoreGraphicsLibrary)]
            extern static uint CGImageGetWidth(IntPtr image);

            [DllImport(CoreGraphicsLibrary)]
            extern static uint CGImageGetHeight(IntPtr image);

            [DllImport(CoreGraphicsLibrary)]
            extern static uint CGImageGetBitsPerComponent(IntPtr image);

            [DllImport(CoreGraphicsLibrary)]
            extern static uint CGImageGetBytesPerRow(IntPtr image);

            [DllImport(CoreGraphicsLibrary)]
            extern static IntPtr CGImageGetColorSpace(IntPtr image);

            [DllImport(CoreGraphicsLibrary)]
            extern static uint CGImageGetBitmapInfo(IntPtr image);
        }

        class CGImageDestination : IDisposable
        {
            public IntPtr Handle { get; private set; }

            CGImageDestination(IntPtr destinationref)
            {
                if (destinationref == IntPtr.Zero) throw new ArgumentNullException("destinationref");
                Handle = destinationref;
            }

            public static CGImageDestination FromUrl(CFURL url, string type, long count)
            {
                var cftype = new CFString(type);
                var destinationref = CGImageDestinationCreateWithURL(url.Handle, cftype.Handle, count, IntPtr.Zero);
                if (destinationref == IntPtr.Zero) throw new Exception("Failed to create destination of type " + type);
                return new CGImageDestination(destinationref);
            }

            public void AddImage(CGImage image)
            {
                CGImageDestinationAddImage(Handle, image.Handle, IntPtr.Zero);
            }

            public void Finish()
            {
                var finalized = CGImageDestinationFinalize(Handle);
                if (!finalized) throw new Exception("Failed to finalize CGImageDestination");
            }

            ~CGImageDestination()
            {
                Dispose();
            }

            public void Dispose()
            {
                if (Handle != IntPtr.Zero)
                {
                    CFRelease(Handle);
                }
                Handle = IntPtr.Zero;
            }

            [DllImport(ImageIOLibrary)]
            static extern IntPtr CGImageDestinationCreateWithURL(IntPtr url, IntPtr type, long count, IntPtr options);

            [DllImport(ImageIOLibrary)]
            static extern bool CGImageDestinationFinalize(IntPtr idst);

            [DllImport(ImageIOLibrary)]
            static extern void CGImageDestinationAddImage(IntPtr idst, IntPtr image, IntPtr properties);
        }

        class CGBitmapContext : IDisposable
        {
            CGBitmapContext(IntPtr contextref)
            {
                if (contextref == IntPtr.Zero) throw new ArgumentNullException("contextref");
                Handle = contextref;
            }

            public static CGBitmapContext CreateCompatibleToImage(CGImage image, uint width, uint height, byte[] data)
            {
                var contextref = CGBitmapContextCreate(data, width, height,
                    image.BitsPerComponent, image.BytesPerRow, image.ColorSpace, image.BitmapInfo);
                if (contextref == IntPtr.Zero) throw new Exception("Failed to create CGBitmapContext");
                return new CGBitmapContext(contextref);
            }

            public void DrawImage(CGRect rect, CGImage image)
            {
                CGContextDrawImage(Handle, rect, image.Handle);
            }

            public CGImage ToImage()
            {
                var imageref = CGBitmapContextCreateImage(Handle);
                if (imageref == IntPtr.Zero) throw new Exception("Failed to obtain image from BitmapContext");
                return new CGImage(imageref);
            }

            public IntPtr Handle { get; private set; }

            ~CGBitmapContext()
            {
                Dispose();
            }

            public void Dispose()
            {
                if (Handle != IntPtr.Zero)
                {
                    CGContextRelease(Handle);
                }
                Handle = IntPtr.Zero;
            }

            [DllImport(CoreGraphicsLibrary)]
            extern static void CGContextRelease(IntPtr handle);

            [DllImport(CoreGraphicsLibrary)]
            extern static IntPtr CGBitmapContextCreate(byte[] data, uint width, uint height, uint bitsPerComponent,
                uint bytesPerRow, IntPtr colorSpace, uint bitmapInfo);

            [DllImport(CoreGraphicsLibrary)]
            extern static IntPtr CGBitmapContextCreateImage(IntPtr context);

            [DllImport(CoreGraphicsLibrary, EntryPoint = "CGContextDrawImage")]
            extern static void CGContextDrawImage32(IntPtr context, CGRect rect, IntPtr image);

            [DllImport(CoreGraphicsLibrary, EntryPoint = "CGContextDrawImage")]
            extern static void CGContextDrawImage64(IntPtr context, CGRect64 rect, IntPtr image);

            static void CGContextDrawImage(IntPtr context, CGRect rect, IntPtr image)
            {
                if (Is64Bit)
                {
                    var rect64 = new CGRect64(rect);
                    CGContextDrawImage64(context, rect64, image);
                }
                else
                {
                    CGContextDrawImage32(context, rect, image);
                }
            }

            static bool Is64Bit = IntPtr.Size == 8;
        }

        [DllImport(CoreGraphicsLibrary)]
        extern static int CGGetActiveDisplayList(int maxDisplays, int[] activeDisplays, ref int displayCount);

        [DllImport(CoreFoundationLibrary)]
        extern static void CFRelease(IntPtr obj);

        const string ImageIOLibrary = "/System/Library/Frameworks/ApplicationServices.framework/Frameworks/ImageIO.framework/ImageIO";
        const string CoreFoundationLibrary = "/System/Library/Frameworks/ApplicationServices.framework/Versions/A/Frameworks/CoreFoundation.framework/CoreFoundation";
        const string CoreGraphicsLibrary = "/System/Library/Frameworks/ApplicationServices.framework/Versions/A/Frameworks/CoreGraphics.framework/CoreGraphics";
    }
}
