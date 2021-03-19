using System;
using System.Collections.Generic;
using System.Linq;
using Unity.DataContract;

namespace Unity.PackageManager
{
    public class EditorModule : IPackageManagerModule
    {
        #region IEditorModule implementation

        public PackageInfo moduleInfo { get; set; }

        public void Initialize()
        {
            PackageManager.Initialize(moduleInfo);
        }

        public void Shutdown(bool wait)
        {
            PackageManager.Stop(wait);
        }

        public void CheckForUpdates()
        {
        }

        #endregion

        #region IPackageManagerModule implementation

        public string editorInstallPath
        {
            get { return Settings.editorInstallPath; }
            set { Settings.editorInstallPath = value; }
        }

        public string unityVersion
        {
            get { return Settings.unityVersion.ToString(); }
            set { Settings.unityVersion = new PackageVersion(value); }
        }

        public UpdateMode updateMode
        {
            get { return Settings.updateMode; }
            set { Settings.updateMode = value; }
        }

        public IEnumerable<PackageInfo> playbackEngines
        {
            get
            {
                return GetNewestLocalPackagesOfType(PackageType.PlaybackEngine);
            }
        }

        public IEnumerable<PackageInfo> unityExtensions
        {
            get
            {
                return GetNewestLocalPackagesOfType(PackageType.UnityExtension);
            }
        }

        public void LoadPackage(PackageInfo package)
        {
            package.loaded = true;
            PackageManager.Database.UpdatePackageState(package);
        }

        public void SelectPackage(PackageInfo package)
        {
            PackageManager.Database.SelectPackage(package);
            PackageManager.Instance.LocalIndexer.CacheResult();
        }

        #endregion

        #region IDisposable implementation

        public void Dispose()
        {
            PackageManager.Stop(false);
        }

        #endregion

        static IEnumerable<PackageInfo> GetNewestLocalPackagesOfType(PackageType type)
        {
            return PackageManager.Database.NewestLocalPackages
                .Where(m => m.Info.Type == type)
                .Select(p => p.ToPackageInfo())
                .OrderBy(pi => pi.name);
        }
    }
}
