using System;
using System.Collections.Generic;
using System.Linq;
using NUnit.Framework;

namespace Bee.Core.Tests
{
    [TestFixture]
    public class ValueWithConditionsTests
    {
        public class MyConfig
        {
            public string SomeValue { get; set; }
        }

        [Test]
        public void UnconditionalValue()
        {
            var v = new ValueWithConditions<string, MyConfig>();
            v.Add("test");
            Assert.AreEqual("test", v.For(new MyConfig() { SomeValue = "Debug" }));
        }

        [Test]
        public void ValueMatchingCondition()
        {
            var v = new ValueWithConditions<string, MyConfig>();
            v.Add(c => c.SomeValue == "release", "myvalue");
            Assert.AreEqual("myvalue", v.For(new MyConfig { SomeValue = "release"}));
        }

        [Test]
        public void ValueNotMatchingCondition()
        {
            var v = new ValueWithConditions<string, MyConfig>();
            v.Add(c => c.SomeValue == "release", "myvalue");
            Assert.AreEqual(null, v.For(new MyConfig { SomeValue = "debug"}));
        }

        [Test]
        public void ValueDependingOnConfig()
        {
            var v = new ValueWithConditions<string, MyConfig>();
            v.Add(c => c.SomeValue.ToString());
            Assert.AreEqual("Debug", v.For(new MyConfig() { SomeValue = "Debug"}));
        }

        public delegate void ConfigureAction(ValueWithConditions<string, MyConfig> v);

        private static IEnumerable<TestCaseData> InvalidUsagePatterns
        {
            // ReSharper disable once UnusedMember.Local
            get
            {
                ConfigureAction unconditional = v => v.Add("unconditional");
                ConfigureAction valueFromConfig = v => v.Add(c => c.SomeValue.ToString());
                ConfigureAction conditional = v => v.Add(c => c.SomeValue == "useConditional", "conditional");
                ConfigureAction readFor = v => v.For(new MyConfig());
                ConfigureAction readForAny = v => v.ForAny();

                yield return new TestCaseData(unconditional, unconditional) { TestName = "{m}(Unconditional, Unconditional)" };
                yield return new TestCaseData(unconditional, valueFromConfig) { TestName = "{m}(Unconditional, ValueFromConfig)" };
                yield return new TestCaseData(unconditional, conditional) { TestName = "{m}(Unconditional, Conditional)" };

                yield return new TestCaseData(valueFromConfig, unconditional) { TestName = "{m}(ValueFromConfig, Unconditional)" };
                yield return new TestCaseData(valueFromConfig, valueFromConfig) { TestName = "{m}(ValueFromConfig, ValueFromConfig)" };
                yield return new TestCaseData(valueFromConfig, conditional) { TestName = "{m}(ValueFromConfig, Conditional)" };

                yield return new TestCaseData(conditional, unconditional) { TestName = "{m}(Conditional, Unconditional)" };
                yield return new TestCaseData(conditional, valueFromConfig) { TestName = "{m}(Conditional, ValueFromConfig)" };
                // Multiple conditional values is the only permitted combination of writes

                yield return new TestCaseData(readFor, unconditional) { TestName = "{m}(For, Unconditional)" };
                yield return new TestCaseData(readFor, valueFromConfig) { TestName = "{m}(For, ValueFromConfig)" };
                yield return new TestCaseData(readFor, conditional) { TestName = "{m}(For, Conditional)" };

                yield return new TestCaseData(readForAny, unconditional) { TestName = "{m}(ForAny, Unconditional)" };
                yield return new TestCaseData(readForAny, valueFromConfig) { TestName = "{m}(ForAny, ValueFromConfig)" };
                yield return new TestCaseData(readForAny, conditional) { TestName = "{m}(ForAny, Conditional)" };
            }
        }

        [Test]
        [TestCaseSource(nameof(InvalidUsagePatterns))]
        public void InvalidUsagePatternThrows(
            ConfigureAction firstExpr,
            ConfigureAction secondExpr)
        {
            var v = new ValueWithConditions<string, MyConfig>();
            firstExpr(v);
            Assert.That(() => secondExpr(v), Throws.InvalidOperationException);
        }

        [Test]
        public void ForAnyReturnsAllConditionalValues()
        {
            var v = new ValueWithConditions<string, MyConfig>();
            v.Add(c => c.SomeValue == "debug", "value");
            v.Add(c => c.SomeValue == "release", "othervalue");
            Assert.That(v.ForAny(), Is.EquivalentTo(new[] {"value", "othervalue"}));
        }

        [Test]
        public void ForAnyReturnsUnconditionalLiteralValue()
        {
            var v = new ValueWithConditions<string, MyConfig>();
            v.Add("value");
            Assert.That(v.ForAny().Single(), Is.EqualTo("value"));
        }

        [Test]
        public void ForAnyReturnsUnconditionalValueDependingOnConfig()
        {
            var v = new ValueWithConditions<string, MyConfig>();
            v.Add(c => c?.SomeValue.ToString());
            Assert.That(v.ForAny().Single(), Is.Null);
        }
    }
}
