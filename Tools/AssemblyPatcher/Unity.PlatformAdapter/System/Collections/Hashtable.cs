using System.Collections.Generic;
using System.Diagnostics;

namespace System.Collections
{
    [DebuggerDisplay("Count={Count}")]
    public class Hashtable : IDictionary
    {
        public Hashtable(IEqualityComparer comparer)// : base(new EqualityComparerAdapter(comparer))
        {
        }

        public bool Contains(object key)
        {
            throw new NotImplementedException();
        }

        public void Add(object key, object value)
        {
            throw new NotImplementedException();
        }

        public void Clear()
        {
            throw new NotImplementedException();
        }

        public IDictionaryEnumerator GetEnumerator()
        {
            throw new NotImplementedException();
        }

        public void Remove(object key)
        {
            throw new NotImplementedException();
        }

        public object this[object key]
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        public ICollection Keys { get; private set; }
        public ICollection Values { get; private set; }
        public bool IsReadOnly { get; private set; }
        public bool IsFixedSize { get; private set; }

        IEnumerator IEnumerable.GetEnumerator()
        {
            return GetEnumerator();
        }

        public void CopyTo(Array array, int index)
        {
            throw new NotImplementedException();
        }

        public int Count { get; private set; }
        public object SyncRoot { get; private set; }
        public bool IsSynchronized { get; private set; }


        public bool ContainsKey(object key)
        {
            throw new NotImplementedException();
        }

        protected virtual int GetHash(Object key)
        {
            throw new NotImplementedException();
        }

        private Dictionary<object, object> container;
    }
}
