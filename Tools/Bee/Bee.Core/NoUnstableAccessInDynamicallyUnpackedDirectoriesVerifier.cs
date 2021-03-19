using System;
using System.Collections.Generic;
using System.Linq;
using NiceIO;

namespace Bee.Core
{
    public class NoUnstableAccessInDynamicallyUnpackedDirectoriesVerifier : IDisposable
    {
        private readonly IDisposable _globbingCallback;
        private readonly IDisposable _statCallback;
        private readonly IDisposable _readFileCallback;
        private readonly List<GlobRequest> _capturedGlobs = new List<GlobRequest>();
        private readonly HashSet<NPath> _capturedStatsAndReads = new HashSet<NPath>();

        public static HashSet<NPath> AdditionalOffLimitsDirectories { get; } = new HashSet<NPath>();

        public NoUnstableAccessInDynamicallyUnpackedDirectoriesVerifier()
        {
            _globbingCallback = NPath.WithGlobbingCallback(CheckGlobbingOperationIsPermitted);
            _statCallback = NPath.WithStatCallback(CheckStatOrReadContentsOperationIsPermitted);
            _readFileCallback = NPath.WithReadContentsCallback(CheckStatOrReadContentsOperationIsPermitted);
        }

        private void CheckStatOrReadContentsOperationIsPermitted(NPath obj)
        {
            VerifyStatOrRead(obj, Backend.Current.AllDirectoriesProducingImplicitInputs.Concat(AdditionalOffLimitsDirectories));

            _capturedStatsAndReads.Add(obj);
        }

        void CheckGlobbingOperationIsPermitted(GlobRequest globRequest)
        {
            VerifyGlob(globRequest, Backend.Current.AllDirectoriesProducingImplicitInputs.Concat(AdditionalOffLimitsDirectories));

            _capturedGlobs.Add(globRequest);
        }

        private static void VerifyGlob(GlobRequest globRequest, IEnumerable<NPath> offLimitsDirectories)
        {
            foreach (var path in offLimitsDirectories)
            {
                if (globRequest.Path.IsSameAsOrChildOf(path))
                    throw new InvalidOperationException(
                        $"You attempted to glob at {globRequest.Path}, but all globbing inside {path} is not permitted.");

                if (globRequest.Recurse && path.IsChildOf(globRequest.Path))
                    throw new InvalidOperationException($"You attempted to do a recursive glob at {globRequest.Path}, but no globs are allowed to operate inside {path}");
            }
        }

        private static void VerifyStatOrRead(NPath path, IEnumerable<NPath> offLimitsDirectories)
        {
            foreach (var dir in offLimitsDirectories)
            {
                if (path.IsSameAsOrChildOf(dir))
                    throw new InvalidOperationException($"You attempted to access {path}, but all file access to the contents of {dir} is forbidden until the build has actually begun.");
            }
        }

        public void Dispose()
        {
            var invalidDirs = Backend.Current.AllDirectoriesProducingImplicitInputs.Concat(AdditionalOffLimitsDirectories).ToList();

            try
            {
                foreach (var globRequest in _capturedGlobs)
                    VerifyGlob(globRequest, invalidDirs);

                foreach (var statRequest in _capturedStatsAndReads)
                    VerifyStatOrRead(statRequest, invalidDirs);
            }
            finally
            {
                _globbingCallback.Dispose();
                _statCallback.Dispose();
                _readFileCallback.Dispose();
            }
        }
    }
}
