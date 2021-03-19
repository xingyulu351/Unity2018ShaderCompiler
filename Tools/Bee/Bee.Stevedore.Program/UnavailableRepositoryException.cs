using System;

namespace Bee.Stevedore.Program
{
    public class UnavailableRepositoryException : HumaneException
    {
        public UnavailableRepositoryException(string message, Exception inner = null) : base(message, inner) {}

        /// <summary>
        /// Utility wrapping the exception with info on *which* artifact failed to download.
        /// </summary>
        public UnavailableRepositoryException CannotDownload(ArtifactName artifact)
            => new UnavailableRepositoryException($"Cannot download '{artifact}'. {Message}", this);
    }
}
