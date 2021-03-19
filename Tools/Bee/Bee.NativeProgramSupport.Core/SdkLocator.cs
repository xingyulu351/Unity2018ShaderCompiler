using System;
using System.Collections.Generic;
using System.Linq;
using Bee.Core;
using Unity.BuildSystem.NativeProgramSupport;
using NiceIO;
using Unity.BuildTools;

namespace Bee.NativeProgramSupport
{
    public abstract class SdkLocator<T> where T : Sdk
    {
        protected abstract Platform Platform { get; }

        protected virtual IEnumerable<T> ScanForDownloadableSdks()
        {
            var downloadableSdks = _downloadableSdkProviders.Value.SelectMany(p => p.Provide()).OfType<T>();
            var incorrect = downloadableSdks.FirstOrDefault(d => !d.IsDownloadable);
            if (incorrect != null)
                throw new ArgumentException($"DownloadableSdkProvider provided sdk {incorrect} that is not downloadable");
            return downloadableSdks;
        }

        protected abstract IEnumerable<T> ScanForSystemInstalledSdks();

        protected abstract T NewSdkInstance(NPath path, Version version, bool isDownloadable);

        public abstract T FromPath(NPath path);

        public abstract T UserDefault { get; } // default selected sdk (in most cases env var configured)

        // this is what is used when no SDK is available
        public virtual T Dummy => NewSdkInstance($"/__unable_to_locate_{Platform.Name.ToLower()}_sdk__", new Version(), false);

        public T UserDefaultOrDummy => UserDefault ?? Dummy;
        public T UserDefaultOrLatest => UserDefault ?? Latest;

        public T Latest => All.FirstOrDefault();

        private readonly Lazy<T[]> _downloadableSdks;
        public IEnumerable<T> DownloadableSdks => _downloadableSdks.Value
        .Where(sdk => sdk != null)
        .OrderByDescending(sdk => sdk)
        .Distinct();

        private readonly Lazy<T[]> _systemInstalledSdks;
        private readonly Lazy<DownloadableSdkProvider[]> _downloadableSdkProviders;

        public IEnumerable<T> SystemInstalledSdks => _systemInstalledSdks.Value
        .Where(sdk => sdk != null)
        .OrderByDescending(sdk => sdk)
        .Distinct();

        public IEnumerable<T> All => Array.Empty<T>()
        .Concat(DownloadableSdks)
        .Concat(SystemInstalledSdks)
        .Append(UserDefaultOrDummy)
        .Append(Dummy)
        .OrderByDescending(sdk => sdk)
        .Distinct();

        protected T FromPath(string path) => FromPath(path != null ? new NPath(path) : null);

        protected SdkLocator(DownloadableSdkProvider[] downloadableSdkProviders = null)
        {
            _downloadableSdks = new Lazy<T[]>(() => ScanForDownloadableSdks().ToArray());
            _systemInstalledSdks = new Lazy<T[]>(() => ScanForSystemInstalledSdks().ToArray());
            _downloadableSdkProviders = new Lazy<DownloadableSdkProvider[]>(() => downloadableSdkProviders ?? DynamicLoader.FindAndCreateInstanceOfEachInAllAssemblies<DownloadableSdkProvider>("Bee.*.dll").ToArray());
        }

        protected static IEnumerable<NPath> ScanCommonLocationsFor(NPath relativePathOrDirectoryName)
        {
            return from path in new[]
                {
                    "C:/", "D:/", "E:/", "F:/",
                    Environment.GetFolderPath(Environment.SpecialFolder.UserProfile),
                    new NPath(Environment.GetFolderPath(Environment.SpecialFolder.UserProfile)).Combine("Library"),
                    Environment.GetFolderPath(Environment.SpecialFolder.LocalApplicationData),
                    Environment.GetFolderPath(Environment.SpecialFolder.ProgramFiles),
                    Environment.GetFolderPath(Environment.SpecialFolder.ProgramFilesX86),
                } where path != null && path.Combine(relativePathOrDirectoryName).Exists()
                select path.Combine(relativePathOrDirectoryName);
        }

        public T FindSdkInSystemOrDownloads(Version version)
        {
            return SystemInstalledSdks.FirstOrDefault(sdk => sdk.Version == version && sdk.SupportedOnHostPlatform)
                ?? DownloadableSdks.FirstOrDefault(sdk => sdk.Version == version && sdk.SupportedOnHostPlatform)
                ?? Dummy;
        }
    }

    public abstract class DownloadableSdkProvider
    {
        public abstract IEnumerable<Sdk> Provide();
    }
}
