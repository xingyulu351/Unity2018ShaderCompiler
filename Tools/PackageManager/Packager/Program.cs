#pragma warning disable 162,168,219

using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using Mono.Options;
using Unity.DataContract;
using Unity.PackageManager.Ivy;
using System.Linq;

namespace Unity.Packager
{
    public class Program
    {
        public static string ErrorMessage;
        public static string Message;

        bool DoIndex;
        bool DoStagingIndex;
        bool DoIvy;
        bool DoPackage;
        bool DoUpdate;
        bool DoPublish;
        bool Publish;
        bool SplitIndexByVersion;
        string Organisation;
        string Module;
        string Version;
        string UnityVersion;
        PackageType PackageType;
        string Description;
        Uri BaseUrl;
        Uri IvyPath;
        Uri SourceFolder;
        Uri OutputPath;
        string OutputFilename;
        string BuildNumber;
        bool TeamCity;
        string Revision;
        string Branch;
        string Channel;
        string PackageSubDir = "packages";
        string ReleaseNotes;
        bool RequiresThreading;
        bool NeedsNewPackage;
        DirectoryInfo TempDir;

        OptionSet ops;

        public static int Main(string[] args)
        {
            var p = new Program();
            var ret = p.Setup(args);
            p.Cleanup();
            return ret;
        }

        public static int Invoke(string[] args)
        {
            var p = new Program() { RequiresThreading = true };
            try
            {
                var ret = p.Setup(args);
                if (ret != 1)
                    p.Cleanup();
                return ret;
            }
            catch (Exception ex)
            {
                ErrorMessage = ex.Message;
                return -1;
            }
        }

        public int Setup(string[] args)
        {
            //Debugger.Break ();
            ops = new OptionSet()
            {
                { "g|org=", "Organisation name", (v) => Organisation = v },
                { "n|module=", "Module name", (v) => Module = v },
                { "v|version=", "Version", (v) => Version = v },
                { "u|uversion=", "Unity Version", (v) => UnityVersion = v },
                { "b|build=", "Build number (added as a fourth segment of the version)", (v) => BuildNumber = v },
                { "d|description=", "Path to text file containing description of package", (v) => Description = ValidateDescription(v) },
                { "t|type=", "Package Type (pm, pb, ee)", (v) => PackageType = ValidatePackageType(v) },
                { "r|repository=", "Repository url for basing package locations", (v) => BaseUrl = ValidateUri(v)},
                { "revision=", "Revision", (v) => Revision = v },
                { "branch=", "Branch", (v) => Branch = v },
                { "channel=", "Channel", (v) => Channel = v },
                { "packagedir=", "Package Directory", (v) => PackageSubDir = v },
                { "releasenotes=", "Release Notes", (v) => ReleaseNotes = v },

                { "i=", "Path to existing ivy file to read data from", (v) => IvyPath = ValidateIvyPath(v) },
                { "o=", "Output path or filename. If it does not contain a filename, one will be generated depending on the operation", (v) => ValidateOutputPath(v)},
                { "s=", "Path to package contents", (v) => SourceFolder = ValidateSource(v)},

                { "x|index", "Create package repository index", (v) => DoIndex = (v != null) },
                { "y|ivy", "Create/Update package ivy.xml (no zipping)", (v) => DoIvy = (v != null) },
                { "z|zip", "Create package for publishing (includes -y flag)", (v) => DoPackage = (v != null) },
                { "w|update", "Update existing zipped package metadata", (v) => DoUpdate = (v != null) },
                { "staging", "Create package repository index for the staging server", (v) => DoStagingIndex = (v != null) },
                { "split-index", "Split the package repository index by versions, instead of one big file. Requires -x", (v) => SplitIndexByVersion = (v != null) },
                { "publish=", "Set publish flag on package (will be included/excluded from staging repo. implies -w)", (v) => { DoPublish = true; Publish = bool.Parse(v); } },

                { "teamcity", "Running on TeamCity (outputs TC-specific messages)", (v) => TeamCity = (v != null) },
            };

            try
            {
                List<string> extra = ops.Parse(args);
            }
            catch (Exception ex)
            {
                LogError("Error parsing options", ex);
                ops.WriteOptionDescriptions(Console.Out);
                return -1;
            }

            TempDir = Utils.GetTempDirectory();

            if (OutputPath == null)
                OutputPath = ValidateDir(TempDir.FullName);

            if (BaseUrl == null)
                BaseUrl = OutputPath;

            if (DoPublish)
                DoUpdate = true;

            if (!DoIndex && !DoStagingIndex && !DoIvy && !DoPackage)
                DoPackage = true;

            if (DoPackage)
                DoIvy = true;

            if (DoIvy && IvyPath == null)
            {
                if (Organisation == null)
                {
                    LogError("Error: missing Organisation");
                    ops.WriteOptionDescriptions(Console.Out);
                    return -1;
                }
                else if (Module == null)
                {
                    LogError("Error: missing Package name");
                    ops.WriteOptionDescriptions(Console.Out);
                    return -1;
                }
                else if (Version == null)
                {
                    LogError("Error: missing Package version");
                    ops.WriteOptionDescriptions(Console.Out);
                    return -1;
                }
                else if (UnityVersion == null)
                {
                    LogError("Error: missing Unity version");
                    ops.WriteOptionDescriptions(Console.Out);
                    return -1;
                }
                else if (PackageType == PackageType.Unknown)
                {
                    LogError("Error: missing Package type");
                    ops.WriteOptionDescriptions(Console.Out);
                    return -1;
                }
            }

            if (DoIndex)
                return RunIndex();
            else if (DoStagingIndex)
            {
                if (SourceFolder == null)
                    SourceFolder = ValidateDir(Path.Combine(Path.GetDirectoryName(IvyPath.LocalPath), PackageSubDir));

                return RunStagingIndex();
            }
            else
                return RunPackage();
        }

        int RunStagingIndex()
        {
            var indexer = new Indexer(Log, LogError);
            indexer.SourceFolder = SourceFolder;
            indexer.IvyPath = IvyPath;
            indexer.OutputPath = OutputPath;
            indexer.GeneratePublicIndexes();
            return 0;
        }

        int RunPackage()
        {
            IvyModule packageIvy = null;
            string originalPackage = null;
            DirectoryInfo outputPath = new DirectoryInfo(OutputPath.LocalPath);
            DirectoryInfo ivyPath = new DirectoryInfo(OutputPath.LocalPath);
            DirectoryInfo packageContentsPath = null;
            string sourcePath = null;
            IvyModule originalModule = null;
            IvyModule module = null;

            if (IvyPath != null)
            {
                sourcePath = Path.GetDirectoryName(IvyPath.LocalPath);
                originalModule = IvyParser.ParseFile<IvyModule>(IvyPath.LocalPath);
                module = ProcessModule(originalModule);
            }
            else
            {
                sourcePath = SourceFolder.LocalPath;
                module = ProcessModule((string)null);
            }

            if (module == null)
                return -1;
            string outputFilename = OutputFilename ?? module.Info.FullName + ".zip";

            if (DoPackage && !DoUpdate)
            {
                packageContentsPath = ProcessPackage(TempDir, module);
                ivyPath = packageContentsPath;
            }

            // if it's an update that changed the version, the ivy file will be saved when the package is done, not here (see UpdatePackageWithNewVersion())
            if (DoIvy && !NeedsNewPackage)
            {
                module.WriteIvyFile(ivyPath.FullName, !DoPackage ? OutputFilename : null);
            }

            if (DoPackage && !DoUpdate)
            {
                CompressPackage(module, TempDir, packageContentsPath, outputPath, outputFilename);
            }
            else if (DoUpdate)
            {
                return UpdatePackage(originalModule, module, outputPath.FullName, outputFilename);
            }
            return 0;
        }

        int RunIndex()
        {
            return RunIndex(false);
        }

        int RunIndex(bool usePublicUrls)
        {
            var indexer = new Indexer(Log, LogError);
            indexer.SourceFolder = SourceFolder;
            indexer.OutputPath = OutputPath;
            indexer.BaseUrl = BaseUrl;
            if (SplitIndexByVersion)
                indexer.GenerateSplitInternalIndex();
            else
                indexer.GenerateFullInternalIndex();
            return 0;
        }

        IvyModule ProcessModule(string ivyPath)
        {
            IvyModule module;

            if (ivyPath != null)
            {
                if (!File.Exists(ivyPath))
                {
                    LogError(string.Format("ivy file not found at {0}", ivyPath));
                    return null;
                }
                module = IvyParser.ParseFile<IvyModule>(ivyPath);
            }
            else
                module = new IvyModule();

            return ProcessModule(module);
        }

        IvyModule ProcessModule(IvyModule original)
        {
            if (original == null)
                return null;

            IvyModule module = original.Clone();

            if (Organisation != null)
                module.Info.Organisation = Organisation;

            if (Module != null)
                module.Info.Module = Module;

            if (UnityVersion != null)
                module.Info.UnityVersion = new PackageVersion(UnityVersion);

            if (Version != null)
            {
                var v = new PackageVersion(Version);
                NeedsNewPackage = DoUpdate && module.Info.Version != v;
                module.Info.Version = v;
            }
            else if (module.Info.Version == null)
            {
                NeedsNewPackage = DoUpdate;
                module.Info.Version = module.Info.UnityVersion;
            }

            if (PackageType != PackageType.Unknown)
                module.Info.Type = PackageType;

            if (Description != null)
                module.Info.Description = Description;

            if (Revision != null)
                module.Info.Revision = Revision;

            if (Branch != null)
                module.Info.Branch = Branch;

            if (Channel != null)
                module.Info.Status = Channel;

            if (BuildNumber != null)
            {
                PackageVersion pv = new PackageVersion(module.Info.Version);
                pv = new PackageVersion(string.Format("{0}.{1}.{2}.{3}", pv.major, pv.minor, pv.micro, BuildNumber));
                module.Info.Version = pv;
                if (TeamCity)
                    Console.WriteLine(@"##teamcity[buildNumber " + module.Info.Version + "]");
            }

            if (ReleaseNotes != null)
                module.UpdateReleaseNotes(module.Info.FullName);

            if (DoPublish)
            {
                if (BaseUrl == null)
                    LogError("BaseUrl needs to be specified to publish a package. Send it via the -r flag");

                if (Publish)
                {
                    IvyRepository def = module.GetRepository("internal");
                    if (def != null)
                        module.Info.Repositories.Remove(def);

                    module.Info.Repositories.Add(new IvyRepository() {
                        Name = "public",
                        Url = BaseUrl
                    });
                    module.Info.Published = true;
                }
                else
                {
                    module.Info.Published = false;
                    IvyRepository def = module.GetRepository("public");
                    if (def != null)
                        module.Info.Repositories.Remove(def);
                    module.Info.Repositories.Add(new IvyRepository() {
                        Name = "internal",
                        Url = BaseUrl
                    });
                }
            }
            else if (DoUpdate && BaseUrl != null)
            {
                if (module.Info.Published)
                {
                    IvyRepository def = module.GetRepository("public");
                    def.Url = BaseUrl;
                }
            }

            if (SplitIndexByVersion && !DoIndex)
            {
                LogError("--index-version cannot be specified without -x");
            }

            return module;
        }

        int UpdatePackageWithNewVersion(IvyModule index, IvyModule original, IvyModule updated, string sourceZip, string targetZip, Dictionary<string, string> filesToAdd)
        {
            var outputPath = Path.GetDirectoryName(targetZip);
            var oldBaseName = Path.GetFileNameWithoutExtension(IvyPath.LocalPath);
            var oldBasePathAndName = Path.Combine(outputPath, oldBaseName);
            var newBaseName = Path.GetFileNameWithoutExtension(targetZip);
            var newBasePathAndName = Path.Combine(outputPath, Path.GetFileNameWithoutExtension(targetZip));
            var ivyBackup = Utils.GetTempFilename(TempDir);

            File.Move(IvyPath.LocalPath, ivyBackup);

            foreach (var art in index.Artifacts)
            {
                art.Url = new Uri(art.Url.ToString().Replace(oldBaseName, newBaseName));
            }

            Log("Updating package {0} to {1}", original.Info.FullName, updated.Info.FullName);
            string hash = ZipUtils.UpdateZip(sourceZip, targetZip,
                string.Format("{0}/{1}/{2}", original.Info.Organisation, original.Info.Module, original.Info.Version),
                string.Format("{0}/{1}/{2}", updated.Info.Organisation, updated.Info.Module, updated.Info.Version),
                filesToAdd.Keys.ToArray(), filesToAdd.Values.ToList()
            );

            if (hash == null)
            {
                Log("Error ocurred updating package. Restoring original ivy file.");
                File.Move(ivyBackup, IvyPath.LocalPath);
                return -1;
            }

            foreach (var mod in index.Artifacts)
                mod.Name = newBaseName;

            if (ReleaseNotes != null)
            {
                Log("Writing Release Notes");
                File.Copy(ReleaseNotes, newBasePathAndName + ".notes", true);
            }

            File.WriteAllText(newBasePathAndName + ".md5", hash);

            Log("Writing ivy file");
            index.WriteIvyFile(outputPath, newBaseName + ".xml");

            Log("Cleaning old files");
            File.Delete(sourceZip);
            File.Delete(oldBasePathAndName + ".md5");
            if (File.Exists(oldBasePathAndName + ".notes"))
                File.Delete(oldBasePathAndName + ".notes");

            return 0;
        }

        int UpdatePackage(IvyModule original, IvyModule updated, string outputPath, string outputZipName)
        {
            var basePathInZip = string.Format("{0}/{1}/{2}/", original.Info.Organisation, original.Info.Module, original.Info.Version);
            var newBasePathInZip = string.Format("{0}/{1}/{2}/", updated.Info.Organisation, updated.Info.Module, updated.Info.Version);

            IvyArtifact zip = updated.GetArtifact(ArtifactType.Package);
            var zipFile = Utils.BuildPath(original.BasePath, zip.Filename);
            var ivyFile = ZipUtils.ExtractFile(zipFile, basePathInZip + "ivy.xml", TempDir.FullName);

            IvyModule packageModule = IvyParser.ParseFile<IvyModule>(ivyFile);
            IvyModule updatedModule = ProcessModule(packageModule);

            var baseName = Path.GetFileNameWithoutExtension(outputZipName);
            var basePathAndName = Path.Combine(outputPath, baseName);

            Dictionary<string, string> filesToAdd = new Dictionary<string, string>();

            if (ReleaseNotes != null)
            {
                updatedModule.UpdateReleaseNotes(baseName);

                filesToAdd.Add(ReleaseNotes, newBasePathInZip + "Release.notes");
            }

            updatedModule.BasePath = null;
            updatedModule.WriteIvyFile(Path.GetDirectoryName(ivyFile));
            filesToAdd.Add(ivyFile, newBasePathInZip + Path.GetFileName(ivyFile));

            if (NeedsNewPackage)
            {
                if (RequiresThreading)
                {
                    System.Threading.ThreadPool.QueueUserWorkItem((object state) => {
                        UpdatePackageWithNewVersion(updated, packageModule, updatedModule, zipFile, Path.Combine(outputPath, OutputFilename ?? updated.Info.FullName + ".zip"), filesToAdd);
                        Cleanup();
                    });
                    Message = string.Format("Package update to version {0} is being processed. This can take a while. The new package will show up in the index eventually.", updated.Info.Version);
                    return 1;
                }
                else
                    return UpdatePackageWithNewVersion(updated, packageModule, updatedModule, zipFile, Path.Combine(outputPath, OutputFilename ?? updated.Info.FullName + ".zip"), filesToAdd);
            }
            else
            {
                string hash = ZipUtils.UpdateZip(zipFile, filesToAdd.Keys.ToArray(), filesToAdd.Values.ToArray());
                if (hash == null)
                    return -1;

                if (ReleaseNotes != null)
                    File.Copy(ReleaseNotes, basePathAndName + ".notes", true);

                // update md5 file with new calculated hash
                File.WriteAllText(basePathAndName + ".md5", hash);
            }

            return 0;
        }

        public void Cleanup()
        {
            if (TempDir != null && TempDir.Exists)
                try { TempDir.Delete(true); } catch { /* do not care */ }
        }

        void LogError(string msg)
        {
            LogError(msg, null);
        }

        void LogError(string msg, Exception ex)
        {
            if (TeamCity)
                Console.Write(@"##teamcity[message text='");
            Console.Write(msg);

            if (ex != null)
            {
                if (TeamCity)
                    Console.Write(@"' errorDetails='");
                Console.Write(ex.Message);
                if (TeamCity)
                    Console.Write("' ");
            }
            if (TeamCity)
                Console.Write("status='ERROR']");
            Console.WriteLine();

            if (!TeamCity && ex != null)
            {
                if (!RequiresThreading)
                    throw ex;
                else
                    Console.WriteLine(ex.StackTrace);
                //throw new InvalidOperationException ();
            }
            ErrorMessage = msg;
        }

        void Log(string format, params object[] args)
        {
            string msg = format;
            if (args != null)
                msg = String.Format(format, args);
            if (TeamCity)
                Console.Write(@"##teamcity[progressMessage '");
            Console.Write(msg);
            if (TeamCity)
                Console.Write(@"']");
            Console.WriteLine();
        }

        DirectoryInfo ProcessPackage(DirectoryInfo basePath, IvyModule module)
        {
            module.BasePath = null; // Don't inject local base path into package

            Log("Creating package tree: {0} {1} {2} {3}", basePath, module.Info.Organisation, module.Info.Module, module.Info.Version);
            DirectoryInfo packageContentsPath = Utils.CreatePackageContentsTree(basePath.FullName, module.Info.Organisation, module.Info.Module, module.Info.Version);

            Log("Copying package contents");
            Utils.CopyFilesRecurse(SourceFolder.LocalPath, packageContentsPath);

            Log("Generating package information");
            UpdateModuleArtifactList(packageContentsPath, "", module);

            return packageContentsPath;
        }

        readonly Dictionary<string, ArtifactType> knownFileTypes = new Dictionary<string, ArtifactType>()
        {
            {".notes", ArtifactType.ReleaseNotes}
        };
        void UpdateModuleArtifactList(DirectoryInfo dir, string basePath, IvyModule module)
        {
            foreach (FileInfo file in dir.GetFiles())
            {
                if (file.Name == "ivy.xml")
                    continue;

                if (!knownFileTypes.ContainsKey(file.Extension))
                    continue;

                if (module.GetArtifact(basePath + file.Name) != null)
                {
                    module.Artifacts.Add(new IvyArtifact() {
                        Name = basePath + Path.GetFileNameWithoutExtension(file.Name),
                        Extension = file.Extension.TrimStart('.'),
                        Type = knownFileTypes[file.Extension]
                    });
                }
            }

            /*
            foreach (DirectoryInfo subdir in dir.GetDirectories ())
            {
                UpdateModuleArtifactList (subdir, basePath + subdir.Name + "/", module);
            }
            */
        }

        /// <summary>
        /// Compresses the package.
        /// </summary>
        /// <returns>The ivy file path</returns>
        /// <param name="module">Module.</param>
        /// <param name="basePath">Base path.</param>
        /// <param name="packageContentsPath">Package contents path.</param>
        /// <param name="outputPath">Output path.</param>
        /// <param name="outputFilename">Output filename.</param>
        string CompressPackage(IvyModule module, DirectoryInfo basePath, DirectoryInfo packageContentsPath, DirectoryInfo outputPath, string outputFilename)
        {
            Log("Compressing");
            return Utils.CreatePackage(module, packageContentsPath, basePath, outputPath, outputFilename);
        }

        static string ValidateDescription(string path)
        {
            if (!File.Exists(path))
                return null;
            return File.ReadAllText(path, System.Text.Encoding.UTF8);
        }

        static PackageType ValidatePackageType(string type)
        {
            switch (type)
            {
                case "pm":
                    return PackageType.PackageManager;
                    break;
                case "pb":
                    return PackageType.PlaybackEngine;
                    break;
                case "ee":
                    return PackageType.UnityExtension;
                    break;
                default:
                    return PackageType.Unknown;
                    break;
            }
        }

        static Uri ValidateSource(string path)
        {
            if (!Directory.Exists(path))
                throw new ArgumentException("directory not found: " + path, path);
            return new Uri("file://" + Path.GetFullPath(path));
        }

        static Uri ValidateDir(string path)
        {
            return new Uri("file://" + Path.GetFullPath(path));
        }

        static Uri ValidateIvyPath(string path)
        {
            if (!File.Exists(path))
            {
                string ivypath = Path.Combine(path, "ivy.xml");
                if (!File.Exists(ivypath))
                    throw new ArgumentException("ivy file not found on " + path, path);
                return new Uri("file://" + Path.GetFullPath(ivypath));
            }
            return new Uri("file://" + Path.GetFullPath(path));
        }

        void ValidateOutputPath(string path)
        {
            if (Path.HasExtension(path))
            {
                OutputPath = ValidateDir(Path.GetDirectoryName(path));
                OutputFilename = Path.GetFileName(path);
            }
            else
                OutputPath = ValidateDir(path);

            if (!Directory.Exists(OutputPath.LocalPath))
            {
                new DirectoryInfo(OutputPath.LocalPath).Create();
            }
        }

        Uri ValidateUri(string uri)
        {
            if (uri == null)
                throw new ArgumentNullException(uri);
            if (!uri.EndsWith("/"))
                uri += "/";
            Uri ret;
            if (!Uri.TryCreate(uri, UriKind.Absolute, out ret))
                throw new ArgumentException("Url '" + uri + "' is invalid", uri);
            return ret;
        }
    }

    static class IvyFormatExtensions
    {
        public static void UpdateReleaseNotes(this IvyModule module, string name)
        {
            var notes = module.GetArtifact(ArtifactType.ReleaseNotes);
            if (notes == null)
            {
                module.Artifacts.Add(new IvyArtifact() {
                    Name = name,
                    Extension = "notes",
                    Type = ArtifactType.ReleaseNotes
                });
            }
            else
            {
                notes.Name = name;
            }
        }
    }
}
