#pragma warning disable 414

using System;
using System.Collections.Generic;
using Unity.PackageManager.Ivy;
using Unity.DataContract;
using System.Linq;
using System.IO;

namespace Unity.PackageManager
{
    public class PackageManager : IDisposable
    {
        List<Action<Task>> registeredListeners = new List<Action<Task>>();
        IvyModule moduleInfo;
        bool running = false;
        object runLock = new object();

        public event Action<Task> OnTask
        {
            add
            {
                registeredListeners.Add(value);
            }
            remove
            {
                if (registeredListeners.Contains(value))
                    registeredListeners.Remove(value);
                else if (value == null)
                    registeredListeners.Clear();
            }
        }

        static PackageManager instance;
        public static PackageManager Instance
        {
            get
            {
                if (instance == null)
                    instance = new PackageManager((IvyModule)null);
                return instance;
            }
            private set { instance = value; }
        }

        Database database;
        internal static Database Database
        {
            get
            {
                if (Instance == null)
                    return null;
                return Instance.database;
            }
        }

        internal static bool Ready { get; private set; }

        internal string Version { get { return moduleInfo != null ? moduleInfo.Info.Version.ToString() : System.Reflection.Assembly.GetExecutingAssembly().GetName().Version.ToString(); } }
        internal Indexer LocalIndexer { get; private set; }
        internal Dictionary<string, Installer> Installers { get; private set; }

        PackageManager(PackageInfo packageInfo)
        {
            database = new Database();
            LocalIndexer = new LocalIndexer();

            if (packageInfo != null)
            {
                moduleInfo = IvyParser.ParseFile<IvyModule>(System.IO.Path.Combine(packageInfo.basePath, "ivy.xml"));
                if (IvyParser.HasErrors)
                    return;
            }
            Initialize();
        }

        PackageManager(IvyModule module)
        {
            database = new Database();
            LocalIndexer = new LocalIndexer();
            moduleInfo = module;

            Initialize();
        }

        void Initialize()
        {
            if (moduleInfo != null)
            {
                Console.WriteLine("Initializing {0} ({1}) v{2} for Unity v{3}", moduleInfo.Name, moduleInfo.Info.Type, moduleInfo.Version, moduleInfo.UnityVersion);
                Ready = true;
                moduleInfo.Loaded = true;
            }
            else
                Console.WriteLine("Initializing Package Manager with no module information. This should only happen during testing.");

            database.OnUpdateAvailable += new Action<IvyModule[]>(PackageUpdatesAvailable);
            database.RefreshLocalPackages(LocalIndexer.Packages);
            database.UpdatePackageState(moduleInfo);

            LocalIndexer.OnFinish += (t, s) => {
                if (s)
                    Database.RefreshLocalPackages(((Indexer)t).Packages);
            };

            Installers = new Dictionary<string, Installer>();
        }

        public static void Initialize(PackageInfo packageInfo)
        {
            if (instance == null)
                instance = new PackageManager(packageInfo);
        }

        public static void Initialize(IvyModule module)
        {
            if (instance == null)
                instance = new PackageManager(module);
        }

        public static void Start()
        {
            if (instance == null)
                instance = new PackageManager((IvyModule)null);
            instance.StartInstance();
        }

        public static void Stop(bool wait)
        {
            if (instance != null)
                instance.StopInstance(wait);
        }

        void StartInstance()
        {
            bool run = false;
            lock (runLock)
            {
                if (!running)
                {
                    run = true;
                    running = true;
                }
            }

            if (!run)
                return;

            Installers = new Dictionary<string, Installer>();
            ThreadUtils.SetMainThread();
            Settings.CacheAllSettings();
            ResumePendingTasks();
        }

        void StopInstance(bool wait)
        {
            Indexer localIndexer = null;
            Service[] installers = null;

            bool stop = false;
            lock (runLock)
            {
                if (running)
                {
                    stop = true;
                    running = false;
                    localIndexer = LocalIndexer;
                    installers = Installers.Values.ToArray();
                    LocalIndexer = null;
                    Installers = null;
                }
            }

            if (!stop)
                return;

            if (localIndexer != null)
                localIndexer.Stop(wait);
            if (installers != null)
                foreach (var installer in installers)
                    installer.Stop(wait);

            database.Dispose();
            database = null;
            instance = null;
        }

        internal void FireListeners(Task task)
        {
            /*
                        Console.WriteLine ("FireListeners {0} {1}", task.Name, task.JobId);
                        task.OnStart += (t) => Console.WriteLine ("FireListeners OnStart {0} {1}", t.Name, t.JobId);
                        task.OnProgress += (t, p) => Console.WriteLine ("FireListeners OnProgress {0} {1}", t.Name, t.JobId);
                        task.OnFinish += (t, s) => Console.WriteLine ("FireListeners OnFinish {0} {1} {2}", t.Name, t.JobId, s);
            */

            foreach (var action in registeredListeners)
                action(task);
        }

        public void Dispose()
        {
            Stop(true);
        }

        // this may be called from a non-main thread, triggered by the installation
        // of a new package
        internal void CheckForLocalUpdates()
        {
            Indexer indexer = null;

            // check if pm is being stopped
            lock (runLock)
            {
                if (!running)
                    return;
                indexer = LocalIndexer;
            }
            RunIndexer(indexer);
        }

        internal Installer SetupPackageInstall(IvyModule package)
        {
            lock (runLock)
            {
                if (!running)
                    return null;
            }

            var installer = new Installer(package);
            Installers.Add(installer.JobId.ToString(), installer);
            installer.OnFinish += (t, s) => {
                if (s || t.CancelRequested)
                {
                    if (Installers.ContainsKey(t.JobId.ToString()))
                        Installers.Remove(t.JobId.ToString());
                    try
                    {
                        if (s)
                            CheckForLocalUpdates();
                    }
                    catch (Exception ex)
                    {
                        Console.WriteLine(ex);
                    }
                }
            };
            FireListeners(installer);
            return installer;
        }

        void RunIndexer(Indexer indexer)
        {
            if (!indexer.IsRunning)
            {
                FireListeners(indexer);
                indexer.Run();
            }
        }

        // possibly called from non-main thread
        void RefreshLocalPackages()
        {
            Database db = null;
            Indexer indexer = null;

            lock (runLock)
            {
                if (!running)
                    return;
                db = database;
                indexer = LocalIndexer;
            }
            db.RefreshLocalPackages(indexer.Packages);
        }

        /*
                // called from the main thread
                void RefreshLocalPackagesFromDomain ()
                {
                    // Don't do anything if we already have packages loaded
                    if (LocalIndexer.Packages.Any ())
                        return;

                    var localModules = new List<IvyModule> (GetCachedLocalModules ());
                    if (!localModules.Any ())
                    {
                        // Fall back to domain scraping
                        foreach (var ass in AppDomain.CurrentDomain.GetAssemblies ())
                        {
                            var ivy = Path.Combine (Path.GetDirectoryName (ass.Location), "ivy.xml");
                            if (File.Exists (ivy))
                            {
                                var package = IvyParser.ParseFile<IvyModule> (ivy);
                                if (package != null)
                                    localModules.Add (package);
                            }
                        }
                    }
                    database.RefreshLocalPackages (localModules);
                }
        */
        // called from non-main thread
        void PackageUpdatesAvailable(IvyModule[] packages)
        {
            lock (runLock)
            {
                if (!running)
                    return;
            }

            // ignore everything except package manager, for now
            var newPackageManager = packages.FirstOrDefault(p => p.Info.Type == PackageType.PackageManager);
            if (newPackageManager == null)
                return;

            foreach (var i in Installers.Values)
            {
                if (i.Package.Info.FullName == newPackageManager.Info.FullName)
                    return;
            }

            var installer = SetupPackageInstall(newPackageManager);
            installer.Run();
        }

        static IEnumerable<IvyModule> GetCachedModulesForIndex(string indexFile)
        {
            try
            {
                // Load local package cache
                if (File.Exists(indexFile))
                {
                    var repo = IvyParser.Deserialize<ModuleRepository>(File.ReadAllText(indexFile));
                    return repo.Modules;
                }
            }
            catch (Exception ex)
            {
                Console.WriteLine("Error loading index cache {0}: {1}", indexFile, ex);
            }

            return new List<IvyModule>();
        }

        IEnumerable<IvyModule> GetCachedLocalModules()
        {
            return GetCachedModulesForIndex(LocalIndexer.IndexCache);
        }

        // called from main thread
        void ResumePendingTasks()
        {
            var tasks = TaskFactory.FromExisting(Settings.installLocation);
            foreach (var task in tasks)
            {
                if (task is Installer)
                {
                    Installers.Add(task.JobId.ToString(), task as Installer);
                    task.OnFinish += (t, s) => {
                        Installers.Remove(t.JobId.ToString());
                        if (s)
                            CheckForLocalUpdates();
                    };
                    FireListeners(task);
                    task.Run();
                }
            }
        }
    }

#if !STANDALONE
    [UnityEditor.InitializeOnLoad]
    class PackageManagerMainThread
    {
        static PackageManagerMainThread()
        {
            if (PackageManager.Ready)
                UnityEngine.ScriptableObject.CreateInstance<EntryPoint>();
        }
    }

    class EntryPoint : UnityEngine.ScriptableObject
    {
        void Awake()
        {
            this.hideFlags = UnityEngine.HideFlags.HideAndDontSave;
            PackageManager.Start();
        }
    }
#endif
}
