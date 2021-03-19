using System;
using System.Collections;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
using Bee.Core;
using NiceIO;
using Unity.BuildTools;

namespace Bee.DotNet
{
    public class DotNetAssembly
    {
        public NPath Path { get; }
        public Framework Framework { get; }
        public DebugFormat DebugFormat { get; }
        public NPath DebugSymbolPath { get; }
        public NPath ReferenceAssemblyPath { get; }

        public DotNetAssembly[] RuntimeDependencies { get; private set; }

        public DotNetAssembly(NPath path, Framework framework, DebugFormat debugFormat = null,
                              NPath debugSymbolPath = null, DotNetAssembly[] runtimeDependencies = null, NPath referenceAssemblyPath = null)
        {
            Path = path;
            Framework = framework;
            RuntimeDependencies = runtimeDependencies ?? Array.Empty<DotNetAssembly>();
            DebugSymbolPath = debugSymbolPath;
            DebugFormat = debugFormat;
            ReferenceAssemblyPath = referenceAssemblyPath;
        }

        public static implicit operator RunnableProgram(DotNetAssembly assembly) => new DotNetRunnableProgram(assembly);

        public NPath[] Paths => new[] { Path, DebugSymbolPath};

        public DotNetAssembly WithRuntimeDependencies(DotNetAssembly[] runtimeDependencies)
        {
            var copy = (DotNetAssembly)MemberwiseClone();
            copy.RuntimeDependencies = runtimeDependencies ?? Array.Empty<DotNetAssembly>();
            return copy;
        }

        public NPath DeployTo(NPath targetDirectory)
        {
            var targetProgram = targetDirectory.Combine(Path.FileName);

            var files = new List<NPath>();

            if (targetDirectory != Path.Parent)
            {
                files.Add(Path);
                files.Add(DebugSymbolPath);
            }

            var dotNetAssemblies = RuntimeDependencies.SelectMany(r => r.RuntimeDependencies.Prepend(r)).ToList();
            foreach (var a in dotNetAssemblies)
            {
                files.Add(a.Path);
                if (a.DebugSymbolPath != null)
                    files.Add(a.DebugSymbolPath);
            }

            foreach (var file in files)
            {
                var targetFile = targetDirectory.Combine(file.FileName);
                if (targetFile != file)
                    CopyTool.Instance().Setup(targetFile, file);

                if (targetFile != targetProgram)
                    Backend.Current.AddDependency(targetProgram, targetFile);
            }

            return targetProgram;
        }
    }
}
