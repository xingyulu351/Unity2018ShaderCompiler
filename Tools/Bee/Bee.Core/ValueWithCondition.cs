using System;
using System.Collections;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Text;

namespace Bee.Core
{
    public class ValueWithConditions<TValue, TConfig> : IEnumerable
    {
        class Entry
        {
            public TValue Value;
            public Func<TConfig, bool> Condition;
            public string SetupTrace;
        }

        readonly List<Entry> _valuesWithConditions = new List<Entry>();
        private Func<TConfig, TValue> _unconditionalValue;
        private string _setUnconditionalStackTrace;
        private string _firstReadStackTrace;

        private void CheckNotAlreadyRead()
        {
            if (!string.IsNullOrEmpty(_firstReadStackTrace))
                throw new InvalidOperationException(
                    $"This ValueWithConditions has already been read from, and so can no longer be modified. It was read with this trace:\n{_firstReadStackTrace}\n");
        }

        public void Set(Func<TConfig, bool> condition, TValue value)
        {
            CheckNotAlreadyRead();
            if (_unconditionalValue != null)
                throw new InvalidOperationException($"This ValueWithConditions has already been configured with an unconditional value, stack trace:\n{_setUnconditionalStackTrace}\n");

            _valuesWithConditions.Add(new Entry() {Condition = condition, Value = value, SetupTrace = Environment.StackTrace});
        }

        public void Set(Func<TConfig, TValue> valueBasedOnConfigFunc)
        {
            CheckNotAlreadyRead();

            if (_unconditionalValue != null || _valuesWithConditions.Any())
                throw new InvalidOperationException($"This ValueWithConditions has already been configured with an unconditional value, stack trace:\n{_setUnconditionalStackTrace}\n");

            if (_valuesWithConditions.Any())
                throw new InvalidOperationException($"This ValueWithConditions has already been configured with conditional values, so an unconditional value can not now be set up. One condition value's stack trace:\n{_valuesWithConditions.First().SetupTrace}\n");

            _unconditionalValue = valueBasedOnConfigFunc;
            _setUnconditionalStackTrace = Environment.StackTrace;
        }

        public void Set(TValue value)
        {
            Set(_ => value);
        }

        //Add aliases for Set, so we can use collection initializer syntax
        public void Add(TValue value) => Set(value);
        public void Add(Func<TConfig, TValue> valueBasedOnConfigFunc) => Set(valueBasedOnConfigFunc);
        public void Add(Func<TConfig, bool> condition, TValue value) => Set(condition, value);

        public TValue For(TConfig config)
        {
            if (string.IsNullOrEmpty(_firstReadStackTrace))
                _firstReadStackTrace = Environment.StackTrace;

            if (_unconditionalValue != null)
                return _unconditionalValue(config);

            var valueWithCondition = _valuesWithConditions.LastOrDefault(c => c.Condition(config));
            return valueWithCondition == null ? default(TValue) : valueWithCondition.Value;
        }

        public IEnumerable<TValue> ForAny()
        {
            if (string.IsNullOrEmpty(_firstReadStackTrace))
                _firstReadStackTrace = Environment.StackTrace;

            if (_unconditionalValue != null)
                return new[] {_unconditionalValue(default(TConfig))};

            return _valuesWithConditions.Select(e => e.Value);
        }

        public bool IsSet => _unconditionalValue != null || _valuesWithConditions.Any();

        public IEnumerator GetEnumerator()
        {
            throw new NotImplementedException();
        }
    }
}
