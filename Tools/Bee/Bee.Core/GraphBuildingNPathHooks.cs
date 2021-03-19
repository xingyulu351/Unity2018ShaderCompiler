using System;
using System.Collections.Generic;

namespace Bee.Core
{
    public class GraphBuildingNPathHooks : IDisposable
    {
        readonly List<IDisposable> _hooks = new List<IDisposable>();

        public GraphBuildingNPathHooks()
        {
            _hooks.Add(new NoUnstableAccessInDynamicallyUnpackedDirectoriesVerifier());
            _hooks.Add(new AutoAddNPathReadContentsAsGraphInfluencing());
            _hooks.Add(new AutoAddNPathGlobsAsGraphInfluencing());
        }

        public void Dispose()
        {
            foreach (var hook in _hooks)
                hook.Dispose();
        }
    }
}
