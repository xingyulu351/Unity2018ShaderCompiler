using System;
using System.Collections.Generic;
using System.Linq;
using NiceIO;

namespace Unity.BuildSystem.CSharpSupport
{
    public class SolutionFileBuilder
    {
        readonly List<Tuple<string, IProjectFile>> _storage = new List<Tuple<string, IProjectFile>>();
        List<CSharpProgram> _programs = new List<CSharpProgram>();
        NPath _path;

        public SolutionFileBuilder WithProgram(params CSharpProgram[] programs)
        {
            return WithProgram(null, programs);
        }

        public SolutionFileBuilder WithProgram(string groupName, params CSharpProgram[] programs)
        {
            _programs.AddRange(programs);
            return WithProjectFile(groupName, programs.Select(p => p.ProjectFile).Cast<IProjectFile>().ToArray());
        }

        public SolutionFileBuilder WithProjectFile(string groupName, params IProjectFile[] projectFiles)
        {
            _storage.AddRange(projectFiles.Where(p => p != null).Select(projectFile => new Tuple<string, IProjectFile>(groupName, projectFile)));
            return this;
        }

        public SolutionFileBuilder WithPath(NPath path)
        {
            _path = path;
            return this;
        }

        public SolutionFile Complete()
        {
            var projectFiles = AllRecursiveDependenciesOf(_programs).Select(p => p.ProjectFile);

            var missingProjectFiles = projectFiles.Where(p => _storage.All(s => s.Item2 != p)).ToArray();
            WithProjectFile(null, missingProjectFiles);

            return new SolutionFile(Path, _storage);
        }

        private NPath Path
        {
            get
            {
                if (_path != null)
                    return _path;

                var projectPath = _storage.First().Item2.Path;
                return projectPath.Parent.Combine(projectPath.FileNameWithoutExtension + ".sln");
            }
        }

        static IEnumerable<CSharpProgram> AllRecursiveDependenciesOf(IEnumerable<CSharpProgram> programs, HashSet<CSharpProgram> processed = null)
        {
            processed = processed ?? new HashSet<CSharpProgram>();

            foreach (var assembly in programs.Where(a => !processed.Contains(a)))
            {
                processed.Add(assembly);
                yield return assembly;
                foreach (var dependentProjectFile in AllRecursiveDependenciesOf(assembly.References.ForAny(), processed))
                    yield return dependentProjectFile;
            }
        }
    }
}
