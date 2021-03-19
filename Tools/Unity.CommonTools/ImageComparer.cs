using System;
using System.Drawing;
using System.Drawing.Imaging;

namespace Unity.CommonTools
{
    public class ImageComparer
    {
        /// <summary>
        /// Compares two bitmaps, writes the difference to diff and
        /// returns a an accumulated error of how pixels that exceed
        /// the input threshold.
        /// </summary>
        public float Compare(Bitmap bitmap1, Bitmap bitmap2, int channelThreshold)
        {
            Bitmap bitmap;
            return Compare(bitmap1, bitmap2, channelThreshold, out bitmap);
        }

        /// <summary>
        /// Compares two bitmaps, writes the difference to diff and
        /// returns a an accumulated error of how pixels that exceed
        /// the input threshold.
        /// </summary>
        public float Compare(Bitmap bitmap1, Bitmap bitmap2, int channelThreshold, out Bitmap diff)
        {
            return Compare(bitmap1, bitmap2, channelThreshold, out diff, 8);
        }

        /// <summary>
        /// Compares two bitmaps, writes the difference to diff and
        /// returns a an accumulated error of how pixels that exceed
        /// the input threshold. The diff is amplified by the given factor.
        /// </summary>
        unsafe public float Compare(Bitmap bitmap1, Bitmap bitmap2, int channelThreshold,
            out Bitmap diff, int diffAmplification)
        {
            if (bitmap1 == bitmap2)
                throw new ArgumentException("Can not compare bitmap to itself.");

            var bitmapData1 = BitmapDataOf(bitmap1, ImageLockMode.ReadOnly);
            var bitmapData2 = BitmapDataOf(bitmap2, ImageLockMode.ReadOnly);
            var width = Math.Max(bitmapData1.Width, bitmapData2.Width);
            var height = Math.Max(bitmapData1.Height, bitmapData2.Height);
            diff = new Bitmap(width, height, PixelFormat.Format24bppRgb);
            var diffBitmapData = BitmapDataOf(diff, ImageLockMode.ReadWrite);

            var ptr1 = (byte*)bitmapData1.Scan0;
            var ptr2 = (byte*)bitmapData2.Scan0;
            var diffPtr = (byte*)diffBitmapData.Scan0;
            float totalOverThreshold = 0;
            for (var h = 0; h < height; h++)
            {
                for (var w = 0; w < width; w++)
                {
                    for (var c = 0; c < 3; c++)
                    {
                        var channel1 = 0;
                        var channel2 = 0;
                        if (w < bitmapData1.Width && h < bitmapData1.Height) channel1 = *ptr1++;
                        if (w < bitmapData2.Width && h < bitmapData2.Height) channel2 = *ptr2++;
                        var channelDiff = Math.Abs(channel1 - channel2);
                        *diffPtr++ = (byte)Math.Min(channelDiff * diffAmplification, 255);

                        var amountOverThreshold = Math.Max(0, channelDiff - channelThreshold);
                        totalOverThreshold += amountOverThreshold;
                    }
                }

                ptr1 = MoveToNextRow(ptr1, bitmapData1);
                ptr2 = MoveToNextRow(ptr2, bitmapData2);
                diffPtr = MoveToNextRow(diffPtr, diffBitmapData);
            }

            bitmap1.UnlockBits(bitmapData1);
            bitmap2.UnlockBits(bitmapData2);
            diff.UnlockBits(diffBitmapData);

            return totalOverThreshold / (width * height * 3);
        }

        BitmapData BitmapDataOf(Bitmap bitmap, ImageLockMode imageLockMode)
        {
            return bitmap.LockBits(new Rectangle(0, 0, bitmap.Width, bitmap.Height), imageLockMode, PixelFormat.Format24bppRgb);
        }

        unsafe byte* MoveToNextRow(byte* pointer, BitmapData bitmapData)
        {
            return pointer + bitmapData.Stride - bitmapData.Width * 3;
        }
    }
}
