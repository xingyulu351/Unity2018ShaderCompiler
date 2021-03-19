using System;
using System.Collections;
using System.IO;
using System.Linq;
using NUnit.Framework.Constraints;

namespace Bee.Core.Tests
{
    public interface ISpecLine
    {
        bool Match(StringReader sr);
    }

    public class MultilineStringMatchesSpecConstraint : Constraint
    {
        private readonly object[] _spec;
        public MultilineStringMatchesSpecConstraint(params object[] spec)
        {
            var list = spec.ToList();
            for (int i = 0; i < list.Count; ++i)
            {
                // Split multi-line strings
                if (list[i] is string s && s.Contains('\n'))
                    list[i] = s.Split('\n');

                // Flatten arrays
                if (!(list[i] is string) && list[i] is IEnumerable e)
                {
                    var sublist = e.OfType<Object>().ToArray();
                    list.RemoveAt(i);
                    list.InsertRange(i, sublist);
                    --i;
                }
            }

            _spec = list.ToArray();
        }

        public override string Description
        {
            get { return "\"" + string.Join("\n", _spec.Select(s => s.ToString())) + "\""; }
        }

        public override ConstraintResult ApplyTo(object actualObj)
        {
            var actual = actualObj as string;
            if (actual == null)
            {
                if (actualObj is IEnumerable)
                    actual = string.Join("\n", ((IEnumerable)actualObj).Cast<object>().ToArray());
                else
                    actual = actualObj.ToString();
            }

            return ApplyTo(actual);
        }

        public ConstraintResult ApplyTo(string actual)
        {
            using (var stringReader = new StringReader(actual))
            {
                foreach (var specLine in _spec)
                {
                    if (specLine is ISpecLine complexSpecLine)
                    {
                        if (!complexSpecLine.Match(stringReader))
                            return new ConstraintResult(this, actual, false);
                        continue;
                    }

                    if (specLine is string simpleSpecLine)
                    {
                        var line = stringReader.ReadLine();
                        if (line != simpleSpecLine)
                            return new ConstraintResult(this, actual, false);
                        continue;
                    }

                    throw new NotSupportedException($"Spec contained an entry of type {specLine.GetType()} which is not supported.");
                }
            }

            return new ConstraintResult(this, actual, true);
        }
    }

    public class AnyNumberOfLinesFollowedByABlankLine : ISpecLine
    {
        public bool Match(StringReader sr)
        {
            while (true)
            {
                var line = sr.ReadLine();
                if (string.IsNullOrEmpty(line))
                    return true;
            }
        }

        public override string ToString()
        {
            return "<any number of lines followed by a blank line>";
        }
    }
}
