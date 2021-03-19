using System;
using System.IO;

namespace Unity.CommonTools
{
    public static class WavFileCreator
    {
        static int freq = 44100;
        static int bitspersample = 16;

        public static void Create(string path, TimeSpan duration)
        {
            using (var stream = new FileStream(path, FileMode.Create))
            {
                using (var writer = new BinaryWriter(stream))
                {
                    var numsamples = (int)(duration.TotalMilliseconds * freq / 1000);
                    var size = numsamples * bitspersample / 8;
                    WriteHeader(writer, size);
                    var r = new Random();
                    for (var i = 0; i != numsamples; i++)
                        writer.Write((ushort)r.Next(ushort.MinValue, ushort.MaxValue));

                    writer.Close();
                }
            }
        }

        static void WriteHeader(BinaryWriter writer, int datalength)
        {
            WriteRawString(writer, "RIFF");
            writer.Write(44 - 8 + datalength);
            WriteRawString(writer, "WAVE");
            WriteRawString(writer, "fmt ");
            writer.Write(16);
            writer.Write((short)1);
            writer.Write((short)1);
            writer.Write(freq);
            writer.Write((freq * bitspersample) / 8);
            writer.Write((short)(bitspersample / 8));
            writer.Write((short)bitspersample);

            WriteRawString(writer, "data");
            writer.Write(datalength);
        }

        static void WriteRawString(BinaryWriter writer, string s)
        {
            for (var i = 0; i != s.Length; i++)
                writer.Write(s[i]);
        }
    }
}
