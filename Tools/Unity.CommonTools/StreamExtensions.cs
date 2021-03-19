using System.IO;

namespace Unity.CommonTools
{
    public static class StreamExtensions
    {
        /// <summary>
        /// Reads the bytes from the source stream and writes them to destination stream.
        /// </summary>
        /// <param name="source">Source stream, reads from the streams current position. May not be null.</param>
        /// <param name="destination">The destination stream where the content is written to. May not be null.</param>
        /// <remarks>This will become obsolite with c# 4 and above since it is a standard extension</remarks>
        public static void CopyTo(this Stream source, Stream destination)
        {
            Ensure.IsNotNull(source, "source");
            Ensure.IsNotNull(destination, "destination");

            int len;
            byte[] buffer = new byte[8 * 1024];
            while ((len = source.Read(buffer, 0, buffer.Length)) > 0)
            {
                destination.Write(buffer, 0, len);
            }
        }
    }
}
