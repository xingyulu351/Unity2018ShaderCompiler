using System.Drawing;
using System.Drawing.Drawing2D;

namespace Unity.CommonTools
{
    public class ImageTools
    {
        public static Bitmap ScaleTo(Bitmap bmp, int width, int height)
        {
            var smallBmp = new Bitmap(width, height);

            var g = Graphics.FromImage(smallBmp);
            g.CompositingQuality = CompositingQuality.HighQuality;
            g.InterpolationMode = InterpolationMode.HighQualityBilinear;
            g.SmoothingMode = SmoothingMode.HighQuality;
            g.DrawImage(bmp, 0, 0, smallBmp.Width, smallBmp.Height);
            g.Dispose();

            return smallBmp;
        }
    }
}
