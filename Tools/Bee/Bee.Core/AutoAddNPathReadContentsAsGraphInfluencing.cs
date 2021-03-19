using System;
using NiceIO;

namespace Bee.Core
{
    public class AutoAddNPathReadContentsAsGraphInfluencing : IDisposable
    {
        private readonly IDisposable _readContentCallback;
        private readonly IDisposable _statCallback;

        private static bool enableStatCallback = false;

        public AutoAddNPathReadContentsAsGraphInfluencing()
        {
            _readContentCallback = NPath.WithReadContentsCallback(path =>
            {
                Backend.Current.RegisterFileInfluencingGraph(path);
            });

            if (enableStatCallback)
            {
                _statCallback = NPath.WithStatCallback(
                    path =>
                    {
                        //tundra has no facility yet to only store the "stat result", when it determines if a dag can be recycled.  For now we'll just record stat's as if "the whole file was read".
                        Backend.Current.RegisterFileInfluencingGraph(path);
                    });
            }
        }

        public void Dispose()
        {
            _readContentCallback.Dispose();
            _statCallback?.Dispose();
        }
    }
}
