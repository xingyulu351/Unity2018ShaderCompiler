using System;
using System.IO;
using System.Linq;
using NUnit.Framework;

namespace Unity.CommonTools.Test
{
    public class StreamExtensionsTest
    {
        [Test]
        public void CopyTo_WhenSourceStreamIsNull_ThenArgumentNullExceptionIsThrown()
        {
            Stream source = null;
            Stream destination = new MemoryStream();

            TestDelegate act = () => source.CopyTo(destination);

            Assert.That(act, Throws.TypeOf<ArgumentNullException>()
                .And.Property("ParamName").EqualTo("source"));
        }

        [Test]
        public void CopyTo_WhenDestinationStreamIsNull_ThenArgumentNullExceptionIsThrown()
        {
            Stream source = new MemoryStream();
            Stream destination = null;

            TestDelegate act = () => source.CopyTo(destination);

            Assert.That(act, Throws.TypeOf<ArgumentNullException>()
                .And.Property("ParamName").EqualTo("destination"));
        }

        [Test]
        public void CopyTo_WhenSourceStreamEmpty_ThenNothingIsCopiedToDestinationStream()
        {
            var source = new MemoryStream();
            var destination = new MemoryStream();

            source.CopyTo(destination);

            Assert.That(destination.ToArray(), Is.Empty);
        }

        [Test]
        public void CopyTo_WhenSourceStreamPositionIsNotAtStart_ThenOnlyBytesFromCurrentPositionIsCopied()
        {
            var source = new MemoryStream(new byte[] { 1, 2, 3, 4 });
            source.Seek(1, SeekOrigin.Begin);
            var destination = new MemoryStream();

            source.CopyTo(destination);

            Assert.That(destination.ToArray(), Is.EqualTo(new byte[] { 2, 3, 4 }));
        }

        [Test]
        public void CopyTo_WhenDestinationStreamHasData_ThenDataIsAppendedToDestinationStream()
        {
            var source = new MemoryStream(new byte[] { 1, 2, 3, 4 });
            var destination = new MemoryStream();
            destination.Write(new byte[] { 10, 20, 30, 40 }, 0, 4);

            source.CopyTo(destination);

            Assert.That(destination.ToArray(), Is.EqualTo(new byte[] { 10, 20, 30, 40, 1, 2, 3, 4 }));
        }

        [Test]
        public void CopyTo_WhenDestinationStreamHasDataButCursorIsNotAtStart_ThenDataIsFromCurserIsOverwritten()
        {
            var source = new MemoryStream(new byte[] { 1, 2, 3, 4 });
            var destination = new MemoryStream();
            destination.Write(new byte[] { 10, 20, 30, 40, 50, 60, 70, 80 }, 0, 8);
            destination.Seek(2, SeekOrigin.Begin);

            source.CopyTo(destination);

            Assert.That(destination.ToArray(), Is.EqualTo(new byte[] { 10, 20, 1, 2, 3, 4, 70, 80 }));
        }

        [Test]
        public void CopyTo_WhenSourceStreamContainsMoreDataThanBufferSize_ThenAllDataIsWrittenToDestination()
        {
            var sourceDataLength = 10 * 1024; // should be larger than the buffersize of 8*1024
            var sourceData = Enumerable.Repeat((byte)42, sourceDataLength).ToArray();
            var source = new MemoryStream(sourceData);
            var destination = new MemoryStream();

            source.CopyTo(destination);

            Assert.That(destination.ToArray().Length, Is.EqualTo(sourceDataLength));
            Assert.That(destination.ToArray().All(x => x == 42), Is.True);
        }
    }
}
