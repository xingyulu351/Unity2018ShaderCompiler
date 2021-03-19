using System;
using NiceIO;

namespace Bee.Core
{
    public class AutoAddNPathGlobsAsGraphInfluencing : IDisposable
    {
        private readonly IDisposable _globbingCallback;

        public AutoAddNPathGlobsAsGraphInfluencing()
        {
            _globbingCallback = NPath.WithGlobbingCallback(globRequest =>
            {
                Register(globRequest.Path, globRequest.Recurse);
            });
        }

        private void Register(NPath path, bool recurse)
        {
            if (!path.DirectoryExists())
                return;

            Backend.Current.RegisterGlobInfluencingGraph(path);

            if (recurse)
            {
                foreach (var dir in path.Directories())
                    Register(dir, recurse);
            }
        }

        public void Dispose()
        {
            _globbingCallback.Dispose();
        }
    }
}
