using System;
using System.Drawing;
using NUnit.Framework;

namespace Unity.CommonTools.Tests.ImageComparerScenarios
{
    [TestFixture]
    public class ImageComparerTestsForIdenticalBitmaps : ImageComparerTestsBase
    {
        [Test]
        public void IdenticalBitmapsReturnZero()
        {
            Assert.AreEqual(0, Compare(BlankBitmap(), BlankBitmap(), 0));
        }

        [Test]
        public void ComparingBitmapToItselfThrows()
        {
            var bitmap = BlankBitmap();
            Bitmap diff;
            Assert.Throws<ArgumentException>(() => Compare(bitmap, bitmap, 0, out diff));
        }

        [Test]
        public void BitmapsAreUnlocked()
        {
            var bitmap1 = BlankBitmap();
            var bitmap2 = BlankBitmap();
            Bitmap diff;
            Compare(bitmap1, bitmap2, 0, out diff);
            bitmap1.GetPixel(0, 0);
            bitmap2.GetPixel(0, 0);
            diff.GetPixel(0, 0);
        }
    }
}
