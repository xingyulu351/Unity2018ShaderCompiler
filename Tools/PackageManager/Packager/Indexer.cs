#pragma warning disable 414

using System;
using Unity.PackageManager.Ivy;
using System.Collections.Generic;
using System.IO;

namespace Unity.Packager
{
    public class Indexer
    {
        public Uri SourceFolder;
        public Uri IvyPath;
        public Uri BaseUrl;
        public Uri OutputPath;
        public string PackageSubDir = "packages";
        public string OutputFilename;

        public bool KeepIvyFileInfo;

        public bool HasErrors;
        public string ErrorMessage;

        bool OnlyPublishedPackages;

        Action<string> logCallback;
        Action<string, Exception> logErrorCallback;
        public Indexer(Action<string> log, Action<string, Exception> logError)
        {
            logCallback = log;
            logErrorCallback = logError;
        }

        /// <summary>
        /// Generates the full index of all packages for all versions of unity.
        /// </summary>
        /// <returns><c>true</c>, if index was generated, <c>false</c> otherwise.</returns>
        public bool GenerateFullInternalIndex()
        {
            if (!Validate())
                return false;

            try
            {
                var repo = GenerateIndex();
                if (repo == null)
                    return false;
                repo.WriteIvyFile(OutputPath.LocalPath);
            }
            catch (Exception ex)
            {
                LogError("Error generating index", ex);
                return false;
            }

            return true;
        }

        /// <summary>
        /// Generates indexes of all packages, split by unity version version.
        /// </summary>
        /// <returns><c>true</c>, if index was generated, <c>false</c> otherwise.</returns>
        public bool GenerateSplitInternalIndex()
        {
            if (!Validate())
                return false;

            ModuleRepository repo = null;
            try
            {
                repo = GenerateIndex();
                if (repo == null)
                    return false;
            }
            catch (Exception ex)
            {
                LogError("Error generating index", ex);
                return false;
            }

            try
            {
                var repos = SplitIndexByVersion(repo);
                if (repos == null)
                {
                    LogError("Something went wrong generating the split indexes");
                    return false;
                }

                if (!Directory.Exists(OutputPath.LocalPath))
                    Directory.CreateDirectory(OutputPath.LocalPath);
                else
                {
                    foreach (var dir in Directory.GetDirectories(OutputPath.LocalPath))
                        if (File.Exists(Path.Combine(dir, "ivy.xml")))
                            File.Delete(Path.Combine(dir, "ivy.xml"));
                }

                foreach (var r in repos)
                {
                    var path = Path.Combine(OutputPath.LocalPath, r.Key);
                    if (!Directory.Exists(path))
                        Directory.CreateDirectory(path);
                    r.Value.WriteIvyFile(path);
                }
            }
            catch (Exception ex)
            {
                LogError("Error generating split indexes", ex);
                return false;
            }

            return true;
        }

        /// <summary>
        /// Generates the public indexes. They are always split by version
        /// </summary>
        /// <returns><c>true</c>, if public indexes was generated, <c>false</c> otherwise.</returns>
        public bool GeneratePublicIndexes()
        {
            if (SourceFolder == null || !Directory.Exists(SourceFolder.LocalPath))
            {
                LogError(string.Format("SourceFolder {0} is invalid", SourceFolder));
                return false;
            }

            if (IvyPath == null || !File.Exists(IvyPath.LocalPath))
            {
                LogError(string.Format("IvyPath {0} is invalid", IvyPath));
                return false;
            }

            if (OutputPath == null)
            {
                LogError("OutputPath is invalid");
                return false;
            }

            OnlyPublishedPackages = true;

            if (Directory.Exists(OutputPath.LocalPath))
            {
                foreach (var dir in Directory.GetDirectories(OutputPath.LocalPath))
                    Directory.Delete(dir, true);
            }

            ModuleRepository repo = ModuleRepository.FromIvyFile(IvyPath.LocalPath);

            foreach (IvyModule module in repo.Modules)
            {
                if (!module.Public)
                    continue;

                string outDir = Utils.BuildPath(OutputPath.LocalPath, module.Info.UnityVersion.major + "." + module.Info.UnityVersion.minor);
                string packagesPath = Utils.BuildPath(outDir, PackageSubDir);
                if (!Directory.Exists(packagesPath))
                    Directory.CreateDirectory(packagesPath);

                foreach (IvyArtifact artifact in module.Artifacts)
                {
                    File.Copy(Path.Combine(SourceFolder.LocalPath, artifact.Filename),
                        Path.Combine(packagesPath, artifact.Filename),
                        true);
                    if (artifact.Type == ArtifactType.Package)
                        File.Copy(Path.Combine(SourceFolder.LocalPath, artifact.MD5Filename),
                            Path.Combine(packagesPath, artifact.MD5Filename),
                            true);
                }
            }


            foreach (var dir in Directory.GetDirectories(OutputPath.LocalPath))
            {
                SourceFolder = Utils.ValidateDir(Path.Combine(dir, PackageSubDir));
                var index = GenerateIndex();
                index.WriteIvyFile(dir);
            }

            return true;
        }

        ModuleRepository GenerateIndex()
        {
            ModuleRepository repo = null;

            DirectoryInfo from = new DirectoryInfo(SourceFolder.LocalPath);
            foreach (FileInfo file in from.GetFiles("*.xml"))
            {
                if (repo == null)
                    repo = new ModuleRepository();

                IvyModule module = null;

                try
                {
                    module = IvyModule.FromIvyFile(file.FullName);
                }
                catch (Exception ex)
                {
                    LogError("Error loading ivy for GenerateIndex", ex);
                }
                if (module == null)
                {
                    if (IvyParser.HasErrors)
                        LogError(IvyParser.ErrorMessage, IvyParser.ErrorException);
                    continue;
                }

                if (OnlyPublishedPackages && !module.Info.Published)
                    continue;

                if (repo.Modules.Contains(module))
                {
                    LogError(string.Format("Repository index already contains module {0}, skipping", module.Info.FullName));
                }

                IvyRepository iRepo = module.GetRepository("internal");
                IvyRepository eRepo = module.GetRepository("public");
                IvyRepository def = null;
                module.Info.Repositories.Clear();
                if (OnlyPublishedPackages)
                {
                    module.Info.Repositories.Add(eRepo);
                    def = eRepo;
                }
                else
                {
                    if (iRepo == null)
                        iRepo = new IvyRepository() {
                            Name = "internal",
                            Url = BaseUrl
                        };
                    else
                        iRepo.Url = BaseUrl;
                    module.Info.Repositories.Add(iRepo);
                    if (module.Info.Published)
                        module.Info.Repositories.Add(eRepo);
                    def = iRepo;
                }

                foreach (IvyArtifact artifact in module.Artifacts)
                    artifact.Url = new Uri(def.Url, PackageSubDir + (PackageSubDir != "" ? "/" : "") + artifact.Filename);

                if (module.GetArtifact(ArtifactType.Ivy) == null)
                {
                    module.Artifacts.Add(new IvyArtifact() {
                        Extension = "xml",
                        Type = ArtifactType.Ivy,
                        Name = Path.GetFileNameWithoutExtension(file.Name),
                        Url = new Uri(def.Url, PackageSubDir + (PackageSubDir != "" ? "/" : "") + file.Name)
                    });
                }

                repo.Modules.Add(module);
                module.WriteIvyFile(file.DirectoryName, file.Name);
                if (!KeepIvyFileInfo)
                {
                    module.BasePath = null;
                    module.IvyFile = null;
                }
            }

            return repo;
        }

        Dictionary<string, ModuleRepository> SplitIndexByVersion(ModuleRepository repo)
        {
            if (repo == null)
                return null;

            var indexes = new Dictionary<string, ModuleRepository>();
            foreach (var mod in repo.Modules)
            {
                string version = mod.Info.UnityVersion.major + "." + mod.Info.UnityVersion.minor;
                if (!indexes.ContainsKey(version))
                    indexes[version] = new ModuleRepository();
                indexes[version].Modules.Add(mod);
            }

            return indexes;
        }

        void SetOutputPath(string path)
        {
            if (Path.HasExtension(path))
            {
                OutputPath = Utils.ValidateDir(Path.GetDirectoryName(path));
                OutputFilename = Path.GetFileName(path);
            }
            else
                OutputPath = Utils.ValidateDir(path);

            if (!Directory.Exists(OutputPath.LocalPath))
            {
                new DirectoryInfo(OutputPath.LocalPath).Create();
            }
        }

        bool Validate()
        {
            if (SourceFolder == null || !Directory.Exists(SourceFolder.LocalPath))
            {
                LogError(string.Format("SourceFolder {0} is invalid", SourceFolder));
                return false;
            }

            if (BaseUrl == null)
            {
                LogError("BaseUrl is invalid");
                return false;
            }

            if (OutputPath == null)
            {
                LogError("OutputPath is invalid");
                return false;
            }
            return true;
        }

        void LogError(string msg)
        {
            LogError(msg, null);
        }

        void LogError(string msg, Exception ex)
        {
            HasErrors = true;
            if (ex != null)
                ErrorMessage = string.Format("{0}\n{1}\n{2}", msg, ex.Message, ex.StackTrace);
            else
                ErrorMessage = msg;
            if (logErrorCallback != null)
                logErrorCallback(msg, ex);
        }
    }
}
