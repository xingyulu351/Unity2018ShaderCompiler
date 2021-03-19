using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using System.Text.RegularExpressions;

namespace UnityBindingsParser
{
    public class PatternBasedDispatcher : IEnumerable
    {
        struct PatternAction
        {
            public readonly Regex Pattern;
            public readonly Action<string> Action;
            public readonly MarkerType Label;

            public PatternAction(Regex regex, Action<string> action, MarkerType label)
            {
                Pattern = regex;
                Action = action;
                Label = label;
            }
        }

        private readonly Action _preDispatchAction = delegate {};
        private readonly List<PatternAction> _patterns = new List<PatternAction>();

        public PatternBasedDispatcher()
        {
        }

        private PatternBasedDispatcher(IEnumerable<PatternAction> patterns, Action preDispatchAction) : this(preDispatchAction)
        {
            _patterns.AddRange(patterns);
        }

        public PatternBasedDispatcher(Action preDispatchAction)
        {
            _preDispatchAction = preDispatchAction;
        }

        public void Add(string lineMarker, Action<string> action)
        {
            Add(ParserUtil.CreateMarker(lineMarker), action, MarkerType.Unknown);
        }

        public void Add(string lineMarker, Action<string> action, MarkerType label)
        {
            Add(ParserUtil.CreateMarker(lineMarker), action, label);
        }

        public void Add(Regex pattern, Action<string> action, MarkerType label)
        {
            _patterns.Add(new PatternAction(pattern, action, label));
        }

        public bool Dispatch(string trimmedLine, string originalLine)
        {
            foreach (var p in _patterns)
            {
                var match = p.Pattern.Match(trimmedLine);
                if (!match.Success)
                    continue;
                _preDispatchAction();
                switch (p.Label)
                {
                    case MarkerType.SimpleComment:
                    case MarkerType.StartPoundIf:
                    case MarkerType.EndPoundIf:
                        p.Action(originalLine);
                        break;
                    case MarkerType.Attribute:
                        p.Action(trimmedLine);
                        break;
                    default:
                        p.Action(ParserUtil.TextAfterMarkerMatch(match));
                        break;
                }
                return true;
            }
            return false;
        }

        IEnumerator IEnumerable.GetEnumerator()
        {
            throw new NotSupportedException();
        }

        public bool MatchesAny(string line)
        {
            return _patterns.Any(p => p.Pattern.IsMatch(line));
        }

        public bool MatchesAnyAndComments(string line)
        {
            return _patterns.Any(p => p.Pattern.IsMatch(line)) || ParserUtil.SimpleCommentMarker.IsMatch(line);
        }

        public PatternBasedDispatcher CombinedWith(PatternBasedDispatcher additionalPatterns)
        {
            return new PatternBasedDispatcher(_patterns.Concat(additionalPatterns._patterns), (Action)Delegate.Combine(_preDispatchAction, additionalPatterns._preDispatchAction));
        }
    }
}
