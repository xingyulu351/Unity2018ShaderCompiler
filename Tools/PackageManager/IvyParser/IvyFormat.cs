#pragma warning disable 414

using System;
using System.Collections.Generic;
using System.IO;
using Unity.DataContract;
using System.Linq;

namespace Unity.PackageManager.Ivy
{
    public enum Visibility
    {
        Private,
        Public
    }

    public enum ArtifactType
    {
        None,
        Package,
        Ivy,
        Dll,
        ReleaseNotes,
        DebugSymbols,
    }

    public class ModuleRepository
    {
        List<IvyModule> modules;
        public List<IvyModule> Modules
        {
            get
            {
                if (modules == null)
                    modules = new List<IvyModule>();
                return modules;
            }
        }

        public IvyModule GetPackage(PackageType type)
        {
            foreach (IvyModule mod in modules)
            {
                if (mod.Info.Type == type)
                    return mod;
            }
            return null;
        }

        public IvyModule GetPackage(string org, string name, string version)
        {
            foreach (IvyModule mod in modules)
            {
                if (mod.Info.FullName == string.Format("{0}.{1}.{2}", org, name, version))
                    return mod;
            }
            return null;
        }

        public override string ToString()
        {
            return IvyParser.Serialize(this);
        }

        public static ModuleRepository FromIvyFile(string fullpath)
        {
            return IvyParser.ParseFile<ModuleRepository>(fullpath);
        }

        public string WriteIvyFile(string outputPath)
        {
            return WriteIvyFile(outputPath, null);
        }

        public string WriteIvyFile(string outputPath, string filename)
        {
            if (filename == null)
                filename = "ivy.xml";
            if (!Directory.Exists(outputPath))
                Directory.CreateDirectory(outputPath);
            string ivyPath = Path.Combine(outputPath, filename);
            string xml = IvyParser.Serialize(this);
            using (var sw = File.CreateText(ivyPath))
            {
                sw.Write(xml);
            }
            return ivyPath;
        }

        public ModuleRepository Clone()
        {
            return Cloner.CloneObject<ModuleRepository>(this);
        }
    }

    public class IvyModule
    {
        public override bool Equals(object other)
        {
            if (other is PackageInfo)
                return this == (other as PackageInfo);
            return this == (other as IvyModule);
        }

        public override int GetHashCode()
        {
            int hash = 17;
            hash = hash * 23 + Info.Organisation.GetHashCode();
            hash = hash * 23 + Info.Module.GetHashCode();
            hash = hash * 23 + Info.Type.GetHashCode();
            hash = hash * 23 + Info.Version.GetHashCode();
            hash = hash * 23 + Info.UnityVersion.GetHashCode();
            return hash;
        }

        public static bool operator==(IvyModule a, object z)
        {
            if ((object)a == null && z == null)
                return true;
            if ((object)a == null || z == null)
                return false;
            return a.GetHashCode() == z.GetHashCode();
        }

        public static bool operator!=(IvyModule a, object z)
        {
            return !(a == z);
        }

        public override string ToString()
        {
            return IvyParser.Serialize(this);
        }

        public static IvyModule FromIvyFile(string fullpath)
        {
            return IvyParser.ParseFile<IvyModule>(fullpath);
        }

        public string WriteIvyFile()
        {
            if (BasePath == null)
                throw new InvalidOperationException("Can't save IvyModule without path information");
            return WriteIvyFile(BasePath, IvyFile, true);
        }

        public string WriteIvyFile(string outputPath)
        {
            return WriteIvyFile(outputPath, IvyFile, false);
        }

        public string WriteIvyFile(string outputPath, string filename)
        {
            return WriteIvyFile(outputPath, filename ?? IvyFile, false);
        }

        string WriteIvyFile(string outputPath, string filename, bool savePath)
        {
            if (filename == null)
                filename = "ivy.xml";
            if (!Directory.Exists(outputPath))
                Directory.CreateDirectory(outputPath);
            string ivyPath = Path.Combine(outputPath, filename);
            if (!savePath)
                BasePath = IvyFile = null;
            string xml = IvyParser.Serialize(this);
            using (var sw = File.CreateText(ivyPath))
            {
                sw.Write(xml);
            }
            BasePath = outputPath;
            IvyFile = filename;
            return ivyPath;
        }

        public PackageInfo ToPackageInfo()
        {
            var pi = new PackageInfo() {
                unityVersion = Info.UnityVersion,
                name = Info.Module,
                organisation = Info.Organisation,
                version = Info.Version,
                type = Info.Type,
                basePath = BasePath,
                description = Info.Description,
                loaded = Loaded,
                files = Artifacts.ToDictionary(p => p.Filename, e => new PackageFileData((PackageFileType)(int)e.Type, e.Url != null ? e.Url.ToString() : "", e.Guid ?? ""))
            };
            var notes = GetArtifact(ArtifactType.ReleaseNotes);
            if (notes != null && BasePath != null)
            {
                var notesPath = Path.Combine(BasePath, notes.Filename);
                if (File.Exists(notesPath))
                    pi.releaseNotes = File.ReadAllText(notesPath);
            }
            return pi;
        }

        public static IvyModule FromPackageInfo(PackageInfo package)
        {
            IvyModule module = new IvyModule();
            module.Info.Organisation = package.organisation;
            module.Info.UnityVersion = package.unityVersion;
            module.Info.Module = package.name;
            module.Info.Version = package.version;
            module.Info.Type = package.type;
            module.BasePath = package.basePath;
            module.Loaded = package.loaded;
            module.Artifacts.Add(
                new IvyArtifact() {
                    Name = module.Info.FullName,
                    Type = ArtifactType.Ivy,
                    Extension =  "xml"
                });
            if (package.files != null)
                module.Artifacts.AddRange(package.files.Keys.Select(f => new IvyArtifact(f)
                {
                    Type = (ArtifactType)(int)package.files[f].type,
                    Url = new Uri(package.files[f].url)
                }));
            return module;
        }

        /// <summary>
        /// Returns the first artifact that matches the supplied type, or null
        /// </summary>
        /// <param name="type"></param>
        /// <returns></returns>
        public IvyArtifact GetArtifact(ArtifactType type)
        {
            return Artifacts.FirstOrDefault(x => x.Type == type);
        }

        public IvyArtifact GetArtifact(string filename)
        {
            return Artifacts.FirstOrDefault(x => x.Filename == filename);
        }

        public IvyRepository GetRepository(string name)
        {
            return Info.repositories.FirstOrDefault(x => x.Name == name);
        }

        public IvyModule Clone()
        {
            return Cloner.CloneObject<IvyModule>(this);
        }

        public string Name
        {
            get { return string.Format("{0}.{1}", Info.Organisation, Info.Module); }
        }

        public PackageVersion Version { get { return Info.Version; } }
        public PackageVersion UnityVersion { get { return Info.UnityVersion; } }

        public string Timestamp;
        public bool Public
        {
            get { return Info.Published; }
            set { Info.Published = value; }
        }

        IvyInfo info;
        public IvyInfo Info
        {
            get
            {
                if (info == null)
                    info = new IvyInfo();
                return info;
            }
            set { info = value; }
        }

        IvyArtifacts artifacts;
        public IvyArtifacts Artifacts
        {
            get
            {
                if (artifacts == null)
                    artifacts = new IvyArtifacts();
                return artifacts;
            }
        }

        IvyDependencies dependencies;
        public IvyDependencies Dependencies
        {
            get
            {
                if (dependencies == null)
                    dependencies = new IvyDependencies();
                return dependencies;
            }
        }

        public bool Loaded;
        public string BasePath;
        public bool Selected;
        public string IvyFile;
    }

    public class IvyInfo
    {
        public PackageVersion Version;
        public string Organisation;
        public string Module;
        public string Revision;
        public string Branch;
        public string Status;
        public string Publication;
        public PackageType Type;
        public PackageVersion UnityVersion;
        public string Title;
        public string Description;
        public bool Published;

        internal List<IvyRepository> repositories;
        public List<IvyRepository> Repositories
        {
            get
            {
                if (repositories == null)
                    repositories = new List<IvyRepository>();
                return repositories;
            }
        }

        public string FullName
        {
            get { return String.Format("{0}.{1}.{2}", Organisation, Module, Version); }
        }

        public IvyInfo Clone()
        {
            return Cloner.CloneObject<IvyInfo>(this);
        }
    }

    public class IvyArtifacts : List<IvyArtifact>
    {
        public IvyArtifacts Clone()
        {
            return Cloner.CloneObject<IvyArtifacts>(this);
        }
    }

    public class IvyArtifact
    {
        public string Name;
        public ArtifactType Type;
        public string Extension;
        public Uri Url;
        public Uri PublicUrl;
        public string Guid;

        List<string> configurations;
        public List<string> Configurations
        {
            get
            {
                if (configurations == null)
                    configurations = new List<string>();
                return configurations;
            }
        }

        public string Filename { get { return String.IsNullOrEmpty(Extension) ? Name : String.Format("{0}.{1}", Name, Extension); } }
        public string MD5Filename { get { return String.Format("{0}.md5", Name); } }
        public Uri MD5Uri
        {
            get
            {
                if (Url == null)
                    return null;
                return new Uri(Url.ToString().Replace(Filename, MD5Filename));
            }
        }

        public IvyArtifact()
        {
        }

        public IvyArtifact(string filename) : this(filename, ArtifactType.None) {}

        public IvyArtifact(string filename, ArtifactType type)
        {
            Name = Path.GetFileNameWithoutExtension(filename);
            Extension = Path.GetExtension(filename);
            Type = type;
        }

        public string WriteToDisk(Guid jobId, string basePath, byte[] bytes)
        {
            string path = Path.Combine(basePath, jobId.ToString());
            Directory.CreateDirectory(path);
            File.WriteAllBytes(Path.Combine(path, Filename), bytes);
            return Path.Combine(path, Filename);
        }

        public IvyArtifact Clone()
        {
            return Cloner.CloneObject<IvyArtifact>(this);
        }
    }

    public class IvyDependencies : List<IvyDependency>
    {
        public IvyDependencies Clone()
        {
            return Cloner.CloneObject<IvyDependencies>(this);
        }
    }

    public class IvyDependency
    {
        public string Organisation;
        public string Name;
        public string Branch;
        public string Revision;
        public string RevisionConstraint;
        public bool Force;

        public IvyDependency Clone()
        {
            return Cloner.CloneObject<IvyDependency>(this);
        }
    }

    public class IvyRepository
    {
        public string Name;
        public Uri Url;
        public string Pattern = "packages/";

        public IvyRepository() {}
        public IvyRepository(string name, Uri url)
        {
            Name = name;
            Url = url;
        }

        public Uri BuildFullUrl(string file)
        {
            if (Url == null)
                return new Uri(new Uri("http://replace.me"), Pattern + file);
            return new Uri(Url, Pattern + file);
        }

        public IvyRepository Clone()
        {
            return Cloner.CloneObject<IvyRepository>(this);
        }
    }
}
