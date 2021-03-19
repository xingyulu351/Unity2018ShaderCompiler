using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Bee.Core;
using NiceIO;
using Unity.BuildTools;
using Bee.DotNet;

namespace Unity.BuildSystem.CSharpSupport
{
    public class ProjectFile : IProjectFile
    {
        private static readonly string CSharpProjectFileTemplate = ResourceHelper.ReadTextFromResource(typeof(ProjectFile).Assembly, "template.csproj");

        public CSharpProgram Program { get; }
        public NPath Path { get; }

        internal ProjectFile(
            CSharpProgram program,
            NPath path,
            IEnumerable<NPath> embeddedResources = null,
            IEnumerable<NPath> contentResources = null)
        {
            Program = program;
            Path = path;

            embeddedResources = embeddedResources ?? Array.Empty<NPath>();
            contentResources = contentResources ?? Array.Empty<NPath>();

            var programResources = Program.Resources.ForAny().Concat(embeddedResources.Select(e => new EmbeddedResource() {PathToEmbed = e}));

            var csprojContent = ProjectFileContentsFor(CSharpProgram.SourcesIn(Program.Sources.ForAny()).ToArray(), programResources.ToArray(), contentResources.ToArray());
            TextFile.Setup(path, csprojContent, "CSProject");

            Backend.Current.AddDependency(Path, Program.References.ForAny().Select(r => r.ProjectFilePath));
            // We need the referenced .csproj files to exist, but not depend on their contents
            if (Program.CsprojReferences.ForAny().Any())
            {
                var csProjReferencePaths =
                    Program.CsprojReferences.ForAny().Select(r => !r.KeepPathRelative && r.Path.IsRelative ? Paths.UnityRoot.Combine(r.Path) : r.Path);
                Backend.Current.AddDependency(Path, csProjReferencePaths);
            }
        }

        string ProjectFileContentsFor(NPath[] sourceFiles, EmbeddedResource[] embeddedResources, NPath[] contentResources)
        {
            var allFiles = sourceFiles.Concat(embeddedResources.Select(e => e.PathToEmbed)).Concat(contentResources).ToArray();

            var outputPath = new NPath($"artifacts/Bee.CSharpSupport/{Name}");
            if (Program.UseOutputPathInProjectFile)
                outputPath = Program.Path.Parent;
            var result = CSharpProjectFileTemplate;
            result = result.Replace("@@OUTPUT_PATH@@", outputPath.RelativeTo(Path.Parent).ToString(SlashMode.Backward));

            // Some projects are entirely deep inside some subfolder, and it's nicer to have VS project not contain the
            // full deep nested folders path to get to the files. Find common prefix that is the same for all the paths,
            // and remove that from folder hierachy.
            var commonPrefixChars = allFiles.Any() ? allFiles.First().ToString().Substring(0, allFiles.Min(s => s.ToString().Length)).TakeWhile((c, i) => allFiles.All(s => s.ToString()[i] == c)).ToArray() : Array.Empty<char>();
            var commonPrefix = new string(commonPrefixChars);

            var filesContent = new StringBuilder();
            foreach (var f in sourceFiles)
            {
                var noneAction = f.Extension == "bindings";
                EmitFile(noneAction ? "None" : "Compile", f, filesContent, commonPrefix);
            }

            foreach (var f in embeddedResources)
            {
                EmitFile("EmbeddedResource", f.PathToEmbed, filesContent, commonPrefix, f.Identifier != null ? $"<LogicalName>{f.Identifier}</LogicalName>" : null);
            }
            foreach (var f in contentResources)
            {
                EmitFile("Content", f, filesContent, commonPrefix);
            }
            result = result.Replace("@@FILES@@", filesContent.ToString());

            var referencesContent = new StringBuilder();
            IEnumerable<string> systemReferences = Program.SystemReferences.ForAny();
            IEnumerable<NPath> prebuiltReferences = Program.PrebuiltReferences.ForAny();

            SanitizeReferences(ref systemReferences, ref prebuiltReferences);

            foreach (var sr in systemReferences)
                referencesContent.AppendLine($"    <Reference Include=\"{sr}\" />");
            foreach (var r in prebuiltReferences)
            {
                referencesContent.AppendLine($"    <Reference Include=\"{r.FileNameWithoutExtension}\" >");
                referencesContent.AppendLine($"      <HintPath>{r.RelativeTo(Path.Parent).ToString(SlashMode.Backward)}</HintPath>");
                referencesContent.AppendLine($"    </Reference>");
            }

            result = result.Replace("@@REFERENCES@@", referencesContent.ToString());

            result = result.Replace("@@ROOT_NAMESPACE@@", Name);
            result = result.Replace("@@ASSEMBLY_NAME@@", Name);
            result = result.Replace("@@DEFINES_DEBUG@@", Program.Defines.For(new CSharpProgramConfiguration(CSharpCodeGen.Debug, null)).SeparateWith(";"));
            result = result.Replace("@@DEFINES_RELEASE@@", Program.Defines.For(new CSharpProgramConfiguration(CSharpCodeGen.Release, null)).SeparateWith(";"));
            result = result.Replace("@@LANGVERSION@@", Program.LanguageVersion);

            result = result.Replace("@@FRAMEWORK_VERSION@@", Program.Framework.MsBuildName);
            result = result.Replace("@@WARNINGS_AS_ERRORS@@", Program.WarningsAsErrors ? "true" : "false");
            result = result.Replace("@@IGNORED_WARNINGS@@", Program.IgnoredWarnings.ForAny().Select(w => $"{w:D4}").SeparateWith(";"));
            result = result.Replace("@@ALLOW_UNSAFE@@", Program.Unsafe ? "true" : "false");
            result = result.Replace("@@BUILD_EVENTS@@", Program.CprojBuildEvents ?? "");

            var projectReferencesContent = new StringBuilder();
            foreach (var projectRef in Program.References.ForAny())
            {
                if (projectRef == null)
                    throw new ArgumentNullException($"Program {Program.Path} has a Reference that is null");
                projectReferencesContent.AppendLine(
                    $@"    <ProjectReference Include={
                            projectRef.ProjectFile.Path.RelativeTo(Path.Parent).InQuotes(SlashMode.Backward)
                        } >
      <Project>{projectRef.ProjectFile.Guid}</Project>
      <Name>{projectRef.ProjectFile.Name}</Name>
    </ProjectReference>");
            }
            foreach (var projectRef in Program.CsprojReferences.ForAny())
            {
                projectReferencesContent.AppendLine(
                    $@"    <ProjectReference Include={
                            projectRef.Path.RelativeTo(Path.Parent).InQuotes(SlashMode.Backward)
                        } >
      <Project>{projectRef.Guid}</Project>
      <Name>{projectRef.Name}</Name>
    </ProjectReference>");
            }
            result = result.Replace("@@PROJECT_REFERENCES@@", projectReferencesContent.ToString());
            result = result.Replace("@@PROJECT_GUID@@", Guid);
            result = result.Replace("@@OUTPUT_TYPE@@", Program.Path.FileName.EndsWith("exe") ? "Exe" : "Library");
            return result;
        }

        void SanitizeReferences(ref IEnumerable<string> systemReferences, ref IEnumerable<NPath> prebuiltReferences)
        {
            if (Program.Framework.IsDotNetFramework)
                systemReferences = systemReferences.Union(new[] {"System", "System.Core"});

            // Many of our current C# projects compile for 2.0 framework, yet reference Linq or similar things. These
            // don't exist in vanilla 2.0 version, but do exist in Mono, so compilation works out fine.
            // For IDE syntax highlighting & intellisense, point these to our own mono installation assemblies.
            var monoRoot = "External/Mono/builds/monodistribution/lib/mono/2.0";
            if (Program.Framework is Framework20 && new NPath(monoRoot).DirectoryExists())
            {
                var notPartOf20 = systemReferences.Intersect(new[] {"System.Core", "System.Xml.Linq"}).ToArray();
                systemReferences = systemReferences.Except(notPartOf20);
                prebuiltReferences = prebuiltReferences.Union(notPartOf20.Select(r => new NPath($"{monoRoot}/{r}.dll")));
            }
        }

        private void EmitFile(string action, NPath src, StringBuilder filesContent, string commonPrefix, string extraTag = null)
        {
            if (!src.IsRelative)
                src = src.RelativeTo(Paths.UnityRoot);
            var compilePath = src.RelativeTo(Path.Parent).ToString(SlashMode.Backward);
            filesContent.AppendLine($"  <{action} Include=\"{compilePath}\">");
            if (!src.IsChildOf(Path.Parent))
            {
                // Some IDEs (e.g. Visual Studio) need Link to be present for files that are outside of csproj folder;
                // the Link value provides folder structure then in the project view.
                if (commonPrefix.Length == 0)
                {
                    var linkPath = src.ToString(SlashMode.Backward);
                    filesContent.AppendLine($"    <Link>{linkPath}</Link>");
                }
                else
                {
                    var linkPath = src.RelativeTo(commonPrefix).ToString(SlashMode.Backward);
                    filesContent.AppendLine($"    <Link>{linkPath}</Link>");
                }
            }
            if (extraTag != null)
                filesContent.AppendLine($"    {extraTag}");
            filesContent.AppendLine($"  </{action}>");
        }

        public string Name
        {
            get
            {
                var result = new NPath(Program.Path.FileNameWithoutExtension);
                return result.HasExtension("gen") ? result.FileNameWithoutExtension : result.ToString();
            }
        }

        public string Guid
        {
            get
            {
                if (!string.IsNullOrEmpty(Program.ExplicitGuid))
                    return Program.ExplicitGuid;
                return GuidForName(Path.ToString());
            }
        }

        public static string GuidForName(string name)
        {
            byte[] stringbytes = Encoding.UTF8.GetBytes(name);
            byte[] hashedBytes = new System.Security.Cryptography.SHA1CryptoServiceProvider().ComputeHash(stringbytes);
            Array.Resize(ref hashedBytes, 16);
            return "{" + new Guid(hashedBytes).ToString().ToUpperInvariant() + "}";
        }
    }
}
