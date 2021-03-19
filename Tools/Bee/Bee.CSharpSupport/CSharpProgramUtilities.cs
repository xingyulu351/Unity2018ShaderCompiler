using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using System.Text.RegularExpressions;
using NiceIO;
using Unity.BuildTools;

namespace Unity.BuildSystem.CSharpSupport
{
    // Reference to an already existing .csproj file
    public class CSharpProjectFileReference : IProjectFile
    {
        public string Name { get; } // name to display in IDE
        public string Guid { get; } // project GUID
        public NPath Path { get; } // .csproj path
        public bool KeepPathRelative { get; }

        public CSharpProjectFileReference(string name, string guid, NPath path, bool keepPathRelative = false)
        {
            Name = name;
            Guid = guid;
            Path = path;
            KeepPathRelative = keepPathRelative;
        }

        public CSharpProjectFileReference(NPath csprojPath, string explicitName = null)
        {
            if (csprojPath.FileExists())
            {
                Path = csprojPath;
                var csproj = csprojPath.MakeAbsolute(Paths.UnityRoot).ReadAllText();
                var match = Regex.Match(csproj, "<ProjectGuid>(.*)</ProjectGuid>");
                if (!match.Success)
                    Errors.Exit($"IDE project generation; could not find ProjectGuid in referenced C# project '{csprojPath}'");
                Guid = match.Groups[1].Value;
                match = Regex.Match(csproj, "<AssemblyName>(.*)</AssemblyName>");
                if (!match.Success)
                    Errors.Exit($"IDE project generation; could not find AssemblyName in referenced C# project '{csprojPath}'");
                Name = explicitName ?? match.Groups[1].Value;
            }
            else
            {
                Errors.Exit($"IDE project generation; referenced C# project '{csprojPath}' file was not found");
            }
            KeepPathRelative = false;
        }

        public CSharpProjectFileReference(ProjectFile csproj)
        {
            Name = csproj.Name;
            Guid = csproj.Guid;
            Path = csproj.Program.ProjectFilePath;
            KeepPathRelative = true;
        }
    }

    public interface IProjectFile
    {
        string Guid { get; }
        string Name { get; }
        NPath Path { get; }
    }

    public enum CSharpCodeGen
    {
        Debug,
        Release
    }

    public class CSharpProgramConfiguration
    {
        public CSharpProgramConfiguration(CSharpCodeGen codeGen, CSharpCompiler compiler)
        {
            CodeGen = codeGen;
            Compiler = compiler;
        }

        public CSharpCodeGen CodeGen { get; }

        public CSharpCompiler Compiler { get; }
    }
}
