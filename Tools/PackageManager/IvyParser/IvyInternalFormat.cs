#pragma warning disable 414

using System.Collections.Generic;
using System.Xml.Serialization;
using Unity.PackageManager.Ivy;
using System.ComponentModel;

namespace Unity.PackageManager.IvyInternal
{
    [XmlType("visibility")]
    public enum Visibility
    {
        [XmlEnum("private")]
        Private,
        [XmlEnum("public")]
        Public
    }

    [XmlType("packageType")]
    public enum IvyPackageType
    {
        [XmlEnum("Unknown")]
        Unknown,
        [XmlEnum("PlaybackEngine")]
        PlaybackEngine,
        [XmlEnum("UnityExtension")]
        UnityExtension,
        [XmlEnum("PackageManager")]
        PackageManager
    }

    [XmlType("type")]
    public enum IvyArtifactType
    {
        [XmlEnum("none")]
        None,
        [XmlEnum("package")]
        Package,
        [XmlEnum("ivy")]
        Ivy,
        [XmlEnum("dll")]
        Dll,
        [XmlEnum("notes")]
        ReleaseNotes,
        [XmlEnum("debug")]
        DebugSymbols,
    }

    [XmlRoot("root")]
    public class IvyRoot : XmlSerializable
    {
        [XmlElement("ivy-repository", Namespace = "http://ant.apache.org/ivy/extra")]
        public ModuleRepository Repository;

        [XmlElement("ivy-module")]
        public IvyModule Module;
    }

    [XmlType("ivy-repository")]
    public class ModuleRepository
    {
        [XmlElement("ivy-module")]
        public IvyModules Modules { get; set; }
    }

    [XmlType("modules")]
    public class IvyModules : List<IvyModule>
    {}

    [XmlRoot("ivy-module")]
    public class IvyModule : XmlSerializable
    {
        [XmlAttribute("version")]
        string Version = "2.0";

        [XmlAttribute("timestamp", Namespace = "http://ant.apache.org/ivy/extra")]
        public string Timestamp;

        [XmlAttribute("basepath", Namespace = "http://ant.apache.org/ivy/extra")]
        public string BasePath;

        [XmlAttribute("selected", Namespace = "http://ant.apache.org/ivy/extra")]
        [DefaultValue(false)]
        public bool Selected;

        [XmlElement("info", Order = 1)]
        public IvyInfo Info { get; set; }

        [XmlElement("publications", Order = 2)]
        IvyArtifacts xmlArtifacts
        {
            get
            {
                if (artifacts.Count == 0)
                    return null;
                return artifacts;
            }
            set
            {
                if (value == null)
                    artifacts.Clear();
                else
                    artifacts = value;
            }
        }

        [XmlElement("dependencies", Order = 3)]
        IvyDependencies xmlDependencies
        {
            get
            {
                if (dependencies.Count == 0)
                    return null;
                return dependencies;
            }
            set
            {
                if (value == null)
                    dependencies.Clear();
                else
                    dependencies = value;
            }
        }

        [XmlIgnore]
        IvyArtifacts artifacts = new IvyArtifacts();

        [XmlIgnore]
        public IvyArtifacts Artifacts { get { return artifacts; } }

        [XmlIgnore]
        IvyDependencies dependencies = new IvyDependencies();

        [XmlIgnore]
        public IvyDependencies Dependencies { get { return dependencies; } }
    }

    [XmlRoot("info")]
    public class IvyInfo : XmlSerializable
    {
        [XmlAttribute("version")]
        public string Version;
        [XmlAttribute("organisation")]
        public string Organisation;
        [XmlAttribute("module")]
        public string Module;
        [XmlAttribute("revision")]
        public string Revision;
        [XmlAttribute("branch")]
        public string Branch;
        [XmlAttribute("status")]
        public string Status;
        [XmlAttribute("publication")]
        public string Publication;
        [XmlAttribute("packageType", Namespace = "http://ant.apache.org/ivy/extra")]
        public IvyPackageType Type;
        [XmlAttribute("unityVersion", Namespace = "http://ant.apache.org/ivy/extra")]
        public string UnityVersion;
        [XmlAttribute("published", Namespace = "http://ant.apache.org/ivy/extra")]
        [DefaultValueAttribute(false)]
        public bool Published;

        [XmlAttribute("title", Namespace = "http://ant.apache.org/ivy/extra")]
        public string Title;

        [XmlElement("description")]
        public string Description;

        [XmlElement("repository")]
        List<IvyRepository> xmlRepositories
        {
            get
            {
                if (repositories.Count == 0)
                    return null;
                return repositories;
            }
            set
            {
                if (value == null)
                    repositories.Clear();
                else
                    repositories = value;
            }
        }

        [XmlIgnore]
        public List<IvyRepository> Repositories
        {
            get { return repositories; }
        }

        [XmlIgnore]
        List<IvyRepository> repositories = new List<IvyRepository>();
    }

    [XmlType("publications")]
    public class IvyArtifacts : List<IvyArtifact>
    {
    }

    [XmlType("artifact")]
    public class IvyArtifact
    {
        [XmlAttribute("name")]
        public string Name;
        [XmlAttribute("type")]
        public IvyArtifactType Type;
        [XmlAttribute("ext")]
        public string Extension;
        [XmlAttribute("url")]
        public string Url;
        [XmlAttribute("guid", Namespace = "http://ant.apache.org/ivy/extra")]
        public string Guid;
    }

    [XmlType("dependencies")]
    public class IvyDependencies : List<IvyDependency>
    {
    }

    [XmlType("dependency")]
    public class IvyDependency
    {
        [XmlAttribute("org")]
        public string Organisation;

        [XmlAttribute("name")]
        public string Name;

        [XmlAttribute("branch")]
        public string Branch;

        [XmlAttribute("rev")]
        public string Revision;

        [XmlAttribute("revConstraint")]
        public string RevisionConstraint;

        [XmlAttribute("force")]
        public bool Force;
    }

    [XmlType("repository")]
    public class IvyRepository
    {
        [XmlAttribute("name")]
        public string Name;
        [XmlAttribute("url")]
        public string Url;
    }
}
