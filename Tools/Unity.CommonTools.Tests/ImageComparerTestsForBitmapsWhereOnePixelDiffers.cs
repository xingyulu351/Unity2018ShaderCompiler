using System.Drawing;
using NUnit.Framework;

namespace Unity.CommonTools.Tests.ImageComparerScenarios
{
    [TestFixture]
    public class ImageComparerTestsForBitmapsWhereOnePixelDiffers : ImageComparerTestsBase
    {
        [Test]
        public void PixelOverThresholdReturnsCorrectValue()
        {
            Assert.AreEqual(2.5499999f, CompareWithThreshold(0));
        }

        [Test]
        public void PixelUnderThresholdReturnsZero()
        {
            Assert.AreEqual(0, CompareWithThreshold(255));
        }

        [Test]
        public void PixelDifferenceIsWrittenToDiff()
        {
            Bitmap diff;
            Compare(BlankBitmap(), BitmapWithOneWhitePixel(), 0, out diff);
            AssertBitmapsAreEqual(BitmapWithOneWhitePixel(), diff);
        }

        float CompareWithThreshold(int threshold)
        {
            return Compare(BlankBitmap(), BitmapWithOneWhitePixel(), threshold);
        }

        void AssertBitmapsAreEqual(Bitmap expected, Bitmap actual)
        {
            Assert.AreEqual(expected.Width, actual.Width);
            Assert.AreEqual(expected.Height, actual.Height);

            for (int x = 0; x < expected.Width; x++)
                for (int y = 0; y < expected.Height; y++)
                    Assert.AreEqual(expected.GetPixel(x, y), actual.GetPixel(x, y));
        }
    }
}
