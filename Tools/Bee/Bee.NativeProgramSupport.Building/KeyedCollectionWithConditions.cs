using System;
using System.Collections.Generic;
using System.Linq;

namespace Unity.BuildSystem.NativeProgramSupport
{
    public class KeyedCollectionWithConditions<TKey, TValue, TConfig>
    {
        struct Entry
        {
            public List<TValue> Values;
            public Func<TConfig, bool> Condition;
        }

        readonly Dictionary<TKey, List<Entry>> _storage = new Dictionary<TKey, List<Entry>>();

        public Action<TKey> KeyValidator { get; set; }
        public IEnumerable<TKey> Keys => _storage.Keys;

        public void Add(IEnumerable<TValue> values, params TKey[] keys)
        {
            Add(null, values, keys);
        }

        public void Add(IEnumerable<TValue> values, IEnumerable<TKey> keys)
        {
            Add(null, values, keys);
        }

        public void Add(Func<TConfig, bool> onlyWhen, IEnumerable<TValue> values, params TKey[] keys)
        {
            Add(onlyWhen, values, (IEnumerable<TKey>)keys);
        }

        public void Add(Func<TConfig, bool> onlyWhen, IEnumerable<TValue> values, IEnumerable<TKey> keys)
        {
            foreach (var key in keys)
            {
                KeyValidator?.Invoke(key);

                List<Entry> list;
                if (!_storage.TryGetValue(key, out list))
                    list = _storage[key] = new List<Entry>();

                list.Add(new Entry {Condition = onlyWhen, Values = values.ToList()});
            }
        }

        public IDictionary<TKey, IEnumerable<TValue>> For(TConfig config)
        {
            var result = new Dictionary<TKey, IEnumerable<TValue>>();
            foreach (var kv in _storage)
            {
                var values = kv.Value.Where(value => value.Condition == null || value.Condition(config))
                    .SelectMany(value => value.Values)
                    .Distinct()
                    .ToArray();

                if (values.Length > 0)
                    result.Add(kv.Key, values);
            }
            return result;
        }

        public IEnumerable<TValue> For(TKey key, TConfig config)
        {
            List<Entry> values;

            if (_storage.TryGetValue(key, out values))
            {
                return values.Where(value => value.Condition == null || value.Condition(config))
                    .SelectMany(value => value.Values)
                    .Distinct();
            }
            return Array.Empty<TValue>();
        }

        public IEnumerable<TValue> ForAny(TKey key)
        {
            List<Entry> values;

            if (_storage.TryGetValue(key, out values))
            {
                return values
                    .SelectMany(value => value.Values)
                    .Distinct();
            }
            return Array.Empty<TValue>();
        }
    }
}
