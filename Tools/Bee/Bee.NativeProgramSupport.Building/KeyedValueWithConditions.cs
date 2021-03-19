using System;
using System.Collections.Generic;
using System.Linq;

namespace Unity.BuildSystem.NativeProgramSupport
{
    public class KeyedValueWithConditions<TKey, TValue, TConfig>
    {
        class Entry
        {
            public TValue Value;
            public Func<TConfig, bool> Condition;
        }

        readonly Dictionary<TKey, List<Entry>> _storage = new Dictionary<TKey, List<Entry>>();

        public Action<TKey> KeyValidator { get; set; }
        public IEnumerable<TKey> Keys => _storage.Keys;

        public void Add(TValue value, params TKey[] keys)
        {
            Add(null, value, (IEnumerable<TKey>)keys);
        }

        public void Add(TValue value, IEnumerable<TKey> keys)
        {
            Add(null, value, keys);
        }

        public void Add(Func<TConfig, bool> condition, TValue value, params TKey[] keys)
        {
            Add(condition, value, (IEnumerable<TKey>)keys);
        }

        public void Add(Func<TConfig, bool> condition, TValue value, IEnumerable<TKey> keys)
        {
            foreach (var key in keys)
            {
                KeyValidator?.Invoke(key);

                List<Entry> list;
                if (!_storage.TryGetValue(key, out list))
                    list = _storage[key] = new List<Entry>();

                list.Add(new Entry {Condition = condition, Value = value});
            }
        }

        public TValue For(TKey key)
        {
            if (_storage[key] == null) return default(TValue);

            var ret = _storage[key].LastOrDefault(entry => entry.Condition == null).Value;
            return ret;
        }

        public bool Has(TKey key, TConfig config)
        {
            if (!_storage.TryGetValue(key, out var values))
                return false;
            return values.Any(v => v.Condition == null ||  v.Condition(config));
        }

        public TValue For(TKey key, TConfig config)
        {
            if (_storage[key] == null) return default(TValue);

            var ret = _storage[key].LastOrDefault(entry => entry.Condition == null || entry.Condition(config)).Value;
            return ret;
        }

        public Dictionary<TKey, IEnumerable<TValue>> For(TConfig config)
        {
            var ret = new Dictionary<TKey, IEnumerable<TValue>>();
            foreach (var key in _storage.Keys)
            {
                var values = _storage[key].Where(e => e.Condition == null || e.Condition(config)).Select(e => e.Value)
                    .ToArray();
                if (values.Length > 0)
                    ret[key] = values;
            }

            return ret;
        }
    }
}
