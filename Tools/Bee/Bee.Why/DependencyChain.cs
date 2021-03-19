using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Bee.Why
{
    internal class DependencyChain
    {
        public enum Kind
        {
            None,
            Uses,
            Depends
        }

        public DependencyChain(IEnumerable<Tuple<BuiltNodeInfo, Kind>> nodes)
        {
            Nodes = nodes.ToList();
        }

        public IEnumerable<Tuple<BuiltNodeInfo, Kind>> Nodes { get; }
    }
}
