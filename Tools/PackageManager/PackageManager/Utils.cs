using System;
using System.IO;
using System.Diagnostics;
using System.Net;

namespace Unity.PackageManager
{
    public class Utils
    {
        public static bool Copy(Stream source, Stream destination, int chunkSize)
        {
            return Copy(source, destination, chunkSize, 0, null, 1000);
        }

        public static bool Copy(Stream source, Stream destination,
            int chunkSize, long totalSize, Func<long, long, bool> progress , int progressUpdateRate)
        {
            byte[] buffer = new byte[chunkSize];
            int bytesRead = 0;
            long totalRead = 0;
            float averageSpeed = -1f;
            float lastSpeed = 0f;
            float smoothing = 0.005f;
            long readLastSecond = 0;
            long timeToFinish = 0;
            Stopwatch watch = null;
            bool success = true;

            bool trackProgress = totalSize > 0 && progress != null;
            if (trackProgress)
                watch = new Stopwatch();

            do
            {
                if (trackProgress)
                    watch.Start();

                bytesRead = source.Read(buffer, 0, chunkSize);

                if (trackProgress)
                    watch.Stop();

                totalRead += bytesRead;

                if (bytesRead > 0)
                {
                    destination.Write(buffer, 0, bytesRead);
                    if (trackProgress)
                    {
                        readLastSecond += bytesRead;
                        if (watch.ElapsedMilliseconds >= progressUpdateRate || totalRead == totalSize)
                        {
                            watch.Reset();
                            lastSpeed = readLastSecond;
                            readLastSecond = 0;
                            averageSpeed = averageSpeed < 0f ? lastSpeed : smoothing * lastSpeed + (1f - smoothing) * averageSpeed;
                            timeToFinish =  Math.Max(1L, (long)((totalSize - totalRead) / (averageSpeed / progressUpdateRate)));

                            if (!progress(totalRead, timeToFinish))
                                break;
                        }
                    }
                }
            }
            while (bytesRead > 0);

            if (totalRead > 0)
                destination.Flush();

            return success;
        }

        internal static DirectoryInfo GetTempDirectory()
        {
            while (true)
            {
                string temp = Path.GetTempFileName();
                try { File.Delete(temp); } catch {}
                temp = BuildPath("unity", Path.GetDirectoryName(temp), Path.GetFileNameWithoutExtension(temp));
                if (Directory.Exists(temp))
                    continue;
                return Directory.CreateDirectory(temp);
            }
        }

        internal static string BuildPath(params string[] parts)
        {
            string path = "";
            for (int i = parts.Length - 1; i >= 0; i--)
            {
                if (!String.IsNullOrEmpty(parts[i]))
                    path = Path.Combine(parts[i], path);
            }
            return path;
        }
    }

    public static class WebRequestExtensions
    {
        public static WebResponse GetResponseWithoutException(this WebRequest request)
        {
            try
            {
                return request.GetResponse();
            }
            catch (WebException e)
            {
                return e.Response;
            }
        }
    }
}
