using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using Unity.BuildTools;

namespace Bee.Core
{
    public class CollectionWithConditions<TValue, TConfig> : IEnumerable<TValue>
    {
        struct Entry
        {
            public TValue Value;
            public Func<TConfig, bool> Condition;
        }

        readonly List<Func<TConfig, IEnumerable<TValue>>> _additionalValuesDependingOnConfig = new List<Func<TConfig, IEnumerable<TValue>>>();

        readonly List<Entry> _storage = new List<Entry>();

        public Action<TValue> ValueValidator { get; set; }
        public bool IsEmpty => _storage.Count == 0;

        public void Add(TValue value)
        {
            Add(null, value);
        }

        public void Add(Func<TConfig, bool> onlyWhen, TValue value)
        {
            ValueValidator?.Invoke(value);
            _storage.Add(new Entry {Value = value, Condition = onlyWhen});
        }

        public void Add(params TValue[] values)
        {
            Add(null, values);
        }

        public void Add(IEnumerable<TValue> values)
        {
            Add(null, values);
        }

        public void Add(Func<TConfig, bool> onlyWhen, params TValue[] values)
        {
            foreach (var value in values)
                Add(onlyWhen, value);
        }

        public void Add(Func<TConfig, bool> onlyWhen, IEnumerable<TValue> values)
        {
            foreach (var value in values)
                Add(onlyWhen, value);
        }

        public void Add(Func<TConfig, IEnumerable<TValue>> valuesDependingOnConfig)
        {
            _additionalValuesDependingOnConfig.Add(valuesDependingOnConfig);
        }

        public void Add(Func<TConfig, TValue> valueDependingOnConfig)
        {
            _additionalValuesDependingOnConfig.Add(c => valueDependingOnConfig(c).AsEnumerable());
        }

        public void Add(Func<TConfig, bool> condition, Func<TConfig, IEnumerable<TValue>> valuesDependingOnConfig)
        {
            _additionalValuesDependingOnConfig.Add(c => condition(c) ? valuesDependingOnConfig(c) : Enumerable.Empty<TValue>());
        }

        public void AddFront(TValue value)
        {
            AddFront(null, value);
        }

        public void AddFront(Func<TConfig, bool> onlyWhen, TValue value)
        {
            ValueValidator?.Invoke(value);
            _storage.Insert(0, new Entry {Value = value, Condition = onlyWhen});
        }

        public IEnumerable<TValue> For(TConfig config)
        {
            return _storage.Where(v => v.Condition == null || v.Condition(config)).Select(v => v.Value).Concat(_additionalValuesDependingOnConfig.SelectMany(f => f(config)));
        }

        public void CopyTo<TOtherConfig>(CollectionWithConditions<TValue, TOtherConfig> other, Func<TOtherConfig, TConfig> configConverter)
        {
            if (other.ValueValidator != null)
                _storage.ForEach(s => other.ValueValidator(s.Value));

            other._storage.AddRange(
                _storage.Select(
                    s => new CollectionWithConditions<TValue, TOtherConfig>.Entry
                    {
                        Value = s.Value,
                        Condition = (s.Condition != null) ? c => s.Condition(configConverter(c)) : (Func<TOtherConfig, bool>)null
                    }));
        }

        public IEnumerable<TValue> ForAny()
        {
            return _storage.Select(v => v.Value);
        }

        public IEnumerable<TValue> ValuesWithoutCondition => _storage.Where(v => v.Condition == null).Select(v => v.Value);
        public IEnumerator<TValue> GetEnumerator()
        {
            throw new NotSupportedException("You cannot enumerate this collection. Use For() or ForAny() instead.");
        }

        IEnumerator IEnumerable.GetEnumerator()
        {
            return GetEnumerator();
        }
    }
}
