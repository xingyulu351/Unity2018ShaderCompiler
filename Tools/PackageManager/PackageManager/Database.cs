#pragma warning disable 414

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Unity.DataContract;
using UnityEditorInternal;
using System.IO;

namespace Unity.PackageManager
{
    using Ivy;

    internal class Database : IDisposable
    {
        public event Action<IvyModule[]> OnUpdateAvailable;
        public event Action<IvyModule[]> OnUpdateLocal;

        ILookup<string, IvyModule> remotePackagesByName;
        ILookup<string, IvyModule> localPackagesByName;

        IEnumerable<IvyModule> packagesNewOrUpdated = Enumerable.Empty<IvyModule>();
        public IEnumerable<IvyModule> NewOrUpdatedPackages { get { return packagesNewOrUpdated; } }
        public IEnumerable<IvyModule> NewestLocalPackages
        {
            get
            {
                if (localPackagesByName == null)
                    return Enumerable.Empty<IvyModule>();
                return localPackagesByName.Select(g => (g.Any(h => h.Selected) ? g.First(h => h.Selected) : g.First()));
            }
        }

        public IEnumerable<IvyModule> AllVersionsOfPackage(string packageName)
        {
            var packages = Enumerable.Empty<IvyModule>();
            if (localPackagesByName != null)
                packages = packages.Union(localPackagesByName[packageName]);
            if (remotePackagesByName != null)
                packages = packages.Union(remotePackagesByName[packageName]);
            return packages;
        }

        public void RefreshRemotePackages(IEnumerable<IvyModule> packages)
        {
            remotePackagesByName = packages.Where(x => x.UnityVersion.IsCompatibleWith(Settings.unityVersion)).OrderByDescending(p => p.Version).ToLookup(p => p.Name);
            RefreshNewPackages();
        }

        public void RefreshLocalPackages(IEnumerable<IvyModule> packages)
        {
            if (localPackagesByName != null)
            {
                var existing = localPackagesByName.SelectMany(x => x).ToArray();
                foreach (var old in existing)
                {
                    packages.Any(p => {
                        if (old == p)
                        {
                            p.Loaded = old.Loaded;
                            return true;
                        }
                        return false;
                    });
                }
            }
            localPackagesByName = packages.Where(x => x.UnityVersion.IsCompatibleWith(Settings.unityVersion)).OrderByDescending(p => p.Version).ToLookup(p => p.Name);

            RefreshNewPackages();

            if (OnUpdateLocal != null)
                OnUpdateLocal(NewestLocalPackages.ToArray());
        }

        void RefreshNewPackages()
        {
            if (remotePackagesByName == null)
                return;

            packagesNewOrUpdated = remotePackagesByName.Select(g => g.First()).Where(e => {
                var local = localPackagesByName[e.Name].FirstOrDefault();
                return local == null || e.Version > local.Version;
            });

            if (OnUpdateAvailable != null)
                OnUpdateAvailable(packagesNewOrUpdated.ToArray());
        }

        public void Dispose()
        {
            CleanupHandlers();
        }

        void CleanupHandlers()
        {
            OnUpdateAvailable = null;
        }

        public void UpdatePackageState(PackageInfo package)
        {
            var modules = localPackagesByName.FirstOrDefault(n => n.Key == package.packageName);
            if (modules != null)
            {
                modules.Any(m => {
                    if (m == package)
                    {
                        m.Loaded = package.loaded;
                        return true;
                    }
                    return false;
                });
            }
        }

        public void UpdatePackageState(IvyModule package)
        {
            var modules = localPackagesByName.FirstOrDefault(n => n.Key == package.Name);
            if (modules != null)
            {
                modules.Any(m => {
                    if (m == package)
                    {
                        m.Loaded = package.Loaded;
                        return true;
                    }
                    return false;
                });
            }
        }

        public void SelectPackage(PackageInfo package)
        {
            var modules = localPackagesByName.FirstOrDefault(p => p.Key == package.packageName);
            if (modules == null)
                return;

            foreach (IvyModule module in modules)
                module.Selected = (module == package);
        }

        public void SelectPackage(IvyModule module)
        {
            var modules = localPackagesByName.FirstOrDefault(p => p.Key == module.Name);
            if (modules == null)
                return;

            foreach (IvyModule m in modules)
                m.Selected = (m == module);
        }
    }
}
