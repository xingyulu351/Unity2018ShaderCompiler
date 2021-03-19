using System.Collections.Generic;
using NiceIO;

namespace Unity.BuildTools
{
    public struct KnuthHash
    {
        public static KnuthHash Create()
        {
            return new KnuthHash() {Value = 3074457345618258791ul};
        }

        public ulong Value { get; private set; }

        public void Add(int value)
        {
            Value += (ulong)value;
            Value *= 3074457345618258799ul;
        }

        public void Add(ulong value)
        {
            Value += value;
            Value *= 3074457345618258799ul;
        }

        public void Add(string read)
        {
            foreach (char t in read)
            {
                Value += t;
                Value *= 3074457345618258799ul;
            }
            Add(0);
        }

        public void Add(IEnumerable<string> many)
        {
            foreach (var one in many)
                Add(one);
        }

        public void Add(NPath path)
        {
            if (path == null)
            {
                Add(0);
                return;
            }

            Add(path.ToString());
        }

        public void Add(IEnumerable<NPath> many)
        {
            foreach (var one in many)
                Add(one);
        }

        public void Add(bool value)
        {
            Add(value ? 1 : 0);
        }
    }
}
