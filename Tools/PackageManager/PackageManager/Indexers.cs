#pragma warning disable 414

using System;
using System.Collections.Generic;
using System.Threading;
using Unity.DataContract;
using Unity.PackageManager.Ivy;
using System.IO;
using System.Linq;

namespace Unity.PackageManager
{
    internal class Indexer : Service
    {
        protected List<IvyModule> packages = new List<IvyModule>();
        static UpdateMode updateMode = Settings.updateMode;
        protected string cachePath = Utils.BuildPath(Locator.installLocation, Settings.unityVersionPath);
        protected string cacheName = "index-cache.xml";


        public string IndexCache
        {
            get { return Path.Combine(cachePath, cacheName); }
        }

        public List<IvyModule> Packages
        {
            get
            {
                return packages;
            }
            set
            {
                packages = value;
                Result = value;
            }
        }

        public virtual void CacheResult()
        {
            var repo = new ModuleRepository();
            if (packages != null)
                repo.Modules.AddRange(packages);
            try
            {
                var temp = Utils.GetTempDirectory();
                string path = repo.WriteIvyFile(temp.FullName, null);  // Write then copy for better atomicity
                File.Copy(path, IndexCache, true);
                temp.Delete(true);
            }
            catch
            {
                // Don't care; we'll just try again next time
            }
        }

        public void FlushCache()
        {
            try
            {
                if (File.Exists(IndexCache))
                    File.Delete(IndexCache);
            }
            catch
            {
                // Try again when we index
            }
        }

        public virtual void LoadFromCache()
        {
            try
            {
                // Load local package cache
                if (File.Exists(IndexCache))
                {
                    var repo = IvyParser.Deserialize<ModuleRepository>(File.ReadAllText(IndexCache));
                    Packages = repo.Modules;
                }
            }
            catch (Exception ex)
            {
                Console.WriteLine("Error loading index cache {0}: {1}\n{2}", IndexCache, ex, IvyParser.ErrorException);
            }
        }
    }

    internal class LocalIndexer : Indexer
    {
        public LocalIndexer() : base()
        {
            Name = "Local Indexer";
            cachePath = Locator.installLocation;
            cacheName = "index-local.xml";

            if (!Settings.teamcity)
                LoadFromCache();

            // make sure we kill old caches if we know the locator has fresh results for us
            if (Locator.Completed)
                FlushCache();

            IEnumerable<object> localPackages = Locator.QueryAllModules();
            if (localPackages.Any())
            {
                var localModules = localPackages.Cast<IvyModule>();
                var found = Packages.Intersect(localModules);  // Discard cached packages not found by locator

                // Take the newest base path for found, cached packages
                foreach (IvyModule module in found)
                {
                    foreach (var local in localModules)
                    {
                        if (module == local)
                        {
                            module.BasePath = local.BasePath;
                            break;
                        }
                    }
                }
                found = found.Union(localModules.Except(found));   // Add new packages
                Packages = found.ToList();
                CacheResult();
            }
            else
            {
                if (!Packages.Any())
                {
                    List<IvyModule> modules = new List<IvyModule>();
                    // Fall back to domain scraping
                    foreach (var ass in AppDomain.CurrentDomain.GetAssemblies().Where(a => GetAssemblyLocationSafe(a) != null))
                    {
                        var ivy = Path.Combine(Path.GetDirectoryName(ass.Location), "ivy.xml");
                        if (File.Exists(ivy))
                        {
                            var package = IvyParser.ParseFile<IvyModule>(ivy);
                            if (package != null)
                                modules.Add(package);
                        }
                    }
                    Packages = modules;
                    CacheResult();
                }
            }
        }

        static string GetAssemblyLocationSafe(System.Reflection.Assembly a)
        {
            try
            {
                return a.Location;
            }
            catch
            {
                // Don't care
            }
            return null;
        }

        protected override bool TaskStarting()
        {
            if (!base.TaskStarting())
                return false;

            Task task1 = new Task(
                null,
                _ => {
                    ManualResetEvent scanDone = new ManualResetEvent(false);
                    Locator.Scan(Path.Combine(Settings.editorInstallPath, "PackageManager"), Settings.unityVersionPath,
                        () => !CancelRequested,
                        () => scanDone.Set());
                    while (!scanDone.WaitOne(10))
                        UpdateProgress(0);
                    return true;
                },
                null
            );
            task1.Name = "Locator Task";

            Task task2 = new Task(null, t => {
                IEnumerable<object> localPackages = Locator.QueryAllModules();
                List<IvyModule> modules = new List<IvyModule>();
                foreach (object package in localPackages)
                    modules.Add(package as IvyModule);
                Packages = modules;
                CacheResult();
                return task1.IsSuccessful;
            }, null);
            task2.Name = "Package List Task";

            HookupChildTask(task1);
            HookupChildTask(task2);
            return true;
        }
    }
}
