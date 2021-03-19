using System;
using System.Collections.Generic;
using System.Linq;
using NiceIO;
using Bee.DotNet;

namespace Unity.BuildSystem.CSharpSupport
{
    // Note: legacy; use CSharpProgram class instead. Keeping this one for a bit, to not disturb other branches too much.
    public class CSharpAssemblyBuilder
    {
        NPath _path;
        readonly List<NPath> _sources = new List<NPath>();
        readonly List<CSharpProgram> _references = new List<CSharpProgram>();
        List<string> _systemReferences;
        readonly List<string> _defines = new List<string>();
        readonly List<NPath> _prebuiltReferences = new List<NPath>();
        bool _unsafe;
        Framework _framework = Framework.Framework46;
        List<int> _ignoredWarnings = new List<int>();
        bool _copyReferencesNextToTarget = true;
        private bool _warnigsAsErrors = true;
        string _languageVersion = "6";
        private List<EmbeddedResource> _resources = new List<EmbeddedResource>();
        private CSharpCompiler _compiler;

        public CSharpAssemblyBuilder WithPath(NPath targetLocation)
        {
            _path = targetLocation;
            return this;
        }

        public CSharpAssemblyBuilder WithSource(params NPath[] sourcesFilesOrDirectories)
        {
            _sources.AddRange(sourcesFilesOrDirectories);
            return this;
        }

        public CSharpAssemblyBuilder WithSource(IEnumerable<NPath> sourcesFilesOrDirectories)
        {
            _sources.AddRange(sourcesFilesOrDirectories);
            return this;
        }

        public CSharpAssemblyBuilder WithSource(string sourceFilesOrDirectories)
        {
            _sources.Add(new NPath(sourceFilesOrDirectories));
            return this;
        }

        public CSharpAssemblyBuilder WithProjectReference(params CSharpProgram[] references)
        {
            _references.AddRange(references);
            return this;
        }

        public CSharpAssemblyBuilder WithSystemReference(params string[] systemReference)
        {
            if (_systemReferences == null)
                _systemReferences = new List<string>();
            _systemReferences.AddRange(systemReference);
            return this;
        }

        public CSharpAssemblyBuilder WithPrebuiltReference(params NPath[] assemblies)
        {
            _prebuiltReferences.AddRange(assemblies);
            return this;
        }

        public CSharpAssemblyBuilder WithPrebuiltReference(IEnumerable<NPath> assemblies)
        {
            _prebuiltReferences.AddRange(assemblies);
            return this;
        }

        public CSharpAssemblyBuilder WithDefines(params string[] defines)
        {
            return WithDefines((IEnumerable<string>)defines);
        }

        public CSharpAssemblyBuilder WithDefines(IEnumerable<string> defines)
        {
            foreach (var d in defines)
                if (d.Contains('='))
                    throw new ArgumentException("Invalid define " + d + "passed to CSharpAssemblyBuilder.WithDefines()");
            _defines.AddRange(defines);
            return this;
        }

        public CSharpAssemblyBuilder WithUnsafe()
        {
            _unsafe = true;
            return this;
        }

        public CSharpAssemblyBuilder WithLanguageVersion(string version)
        {
            _languageVersion = version;
            return this;
        }

        public CSharpAssemblyBuilder WithIgnoredWarning(params int[] warnings)
        {
            _ignoredWarnings.AddRange(warnings);
            return this;
        }

        public CSharpAssemblyBuilder WithCopyReferencesNextToTarget(bool value)
        {
            _copyReferencesNextToTarget = value;
            return this;
        }

        public CSharpAssemblyBuilder WithFramework(Framework framework)
        {
            _framework = framework;
            return this;
        }

        public CSharpAssemblyBuilder WithCompiler(CSharpCompiler compiler)
        {
            _compiler = compiler;
            return this;
        }

        public CSharpAssemblyBuilder WithWarningsAsErrors(bool value)
        {
            _warnigsAsErrors = value;
            return this;
        }

        public CSharpAssemblyBuilder WithResource(params EmbeddedResource[] resources)
        {
            _resources.AddRange(resources);
            return this;
        }

        public CSharpAssemblyBuilder WithResource(params NPath[] resources)
        {
            _resources.AddRange(resources.Select(r => new EmbeddedResource() { PathToEmbed = r, Identifier = r.FileName}));
            return this;
        }

        public CSharpProgram Complete(bool setupBuildInvocation = true)
        {
            if (_path == null)
                throw new ArgumentException($"You have to setup a path. Use .{nameof(WithPath)}()");

            if (_sources.Count == 0)
                throw new ArgumentException($"You have to setup sources. Use .{nameof(WithSource)}()");

            var p = new CSharpProgram
            {
                Path = _path,
                Sources = {_sources},
                Defines = {_defines},
                References = {_references},
                SystemReferences = {_systemReferences ?? new List<string>()},
                PrebuiltReferences = {_prebuiltReferences},
                Unsafe = _unsafe,
                Framework = _framework,
                IgnoredWarnings = {_ignoredWarnings},
                CopyReferencesNextToTarget = _copyReferencesNextToTarget,
                LanguageVersion = _languageVersion,
                WarningsAsErrors = _warnigsAsErrors,
                Resources = {_resources},
            };

            if (setupBuildInvocation)
                p.SetupSpecificConfiguration(new CSharpProgramConfiguration(CSharpCodeGen.Debug, _compiler ?? CSharpProgram.DetermineCompiler()));
            return p;
        }
    }
}
