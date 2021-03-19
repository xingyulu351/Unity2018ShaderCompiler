using System;
using NiceIO;

namespace Bee.Core
{
    public static class Configuration
    {
        private static NPath _rootArtifactsPath;
        public const string DefaultRootArtifactsPath = "artifacts";

        public static NPath RootArtifactsPath
        {
            get => _rootArtifactsPath ?? (_rootArtifactsPath = DefaultRootArtifactsPath);
            set
            {
                if (_rootArtifactsPath != null)
                    throw new InvalidOperationException($"{nameof(RootArtifactsPath)} already set");
                _rootArtifactsPath = value;
            }
        }
    }
}
