using System.Drawing;
using System.Drawing.Imaging;

namespace Unity.CommonTools.Tests.ImageComparerScenarios
{
    public class ImageComparerTestsBase
    {
        protected static Bitmap BitmapWithOneWhitePixel()
        {
            var bitmap = BlankBitmap();
            bitmap.SetPixel(0, 0, Color.White);
            return bitmap;
        }

        protected static Bitmap BlankBitmap()
        {
            return new Bitmap(10, 10, PixelFormat.Format24bppRgb);
        }

        protected static float Compare(Bitmap bitmap1, Bitmap bitmap2, int threshold)
        {
            return new ImageComparer().Compare(bitmap1, bitmap2, threshold);
        }

        protected static float Compare(Bitmap bitmap1, Bitmap bitmap2, int threshold, out Bitmap diff)
        {
            return new ImageComparer().Compare(bitmap1, bitmap2, threshold, out diff);
        }
    }
}
