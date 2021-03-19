using System;
using System.Collections.Generic;
using System.IO;

namespace UnityBindingsParser
{
    public class PushBackReader
    {
        private readonly TextReader _reader;
        private readonly Queue<string> _head = new Queue<string>();
        private TextReader _docReader;

        public PushBackReader(TextReader reader)
        {
            _reader = reader;
            _docReader = null;
        }

        ~PushBackReader()
        {
            if (_reader != null)
                _reader.Close();

            if (_docReader != null)
                _docReader.Close();
        }

        public void PushBack(string line)
        {
            _head.Enqueue(line);
        }

        public string PopHead()
        {
            return _head.Dequeue();
        }

        public string ReadLine()
        {
            if (_head.Count > 0)
                return PopHead();

            if (_docReader == null)
                return _reader.ReadLine();

            var line = _docReader.ReadLine();

            if (line != null)
                return line;

            // read failed, so assume at end of file
            _docReader.Close();
            _docReader = null;

            //follow through to get input from parent file
            return _reader.ReadLine();
        }

        public void OpenDocFile(string name)
        {
            Console.Out.WriteLine("Got {0}", name);
            _docReader = File.OpenText(name);
        }
    }
}
