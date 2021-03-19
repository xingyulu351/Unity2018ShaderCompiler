using System;
using System.Linq;
using JamSharp.Runtime;
using NiceIO;
using Unity.BuildTools;

namespace Unity.BuildSystem
{
    public static class Cecil
    {
        static readonly NPath BaseLibDir = new NPath("External/Unity.Cecil/builds/lib");

        static string[] symbolRefs = new[] { "Unity.Cecil.dll", "Unity.Cecil.Pdb.dll", "Unity.Cecil.Mdb.dll" };
        static string[] allRefs = new[] { "Unity.Cecil.dll", "Unity.Cecil.Pdb.dll", "Unity.Cecil.Mdb.dll", "Unity.Cecil.Rocks.dll" };

        public static readonly NPath Current35 = BaseLibDir.Combine("net35");
        public static readonly NPath Current35Ref = Current35.Combine("Unity.Cecil.dll");

        static NPath[] _current35WithSymbolRefs;
        static NPath[] _current35AllRefs;

        public static NPath[] Current35WithSymbolRefs => _current35WithSymbolRefs ?? (_current35WithSymbolRefs = symbolRefs.Select(d => Current35.Combine(d)).ToArray());
        public static NPath[] Current35AllRefs => _current35AllRefs ?? (_current35AllRefs = allRefs.Select(d => Current35.Combine(d)).ToArray());

        public static readonly NPath Current40 = BaseLibDir.Combine("net40");
        public static readonly NPath Current40Ref = Current40.Combine("Unity.Cecil.dll");

        static NPath[] _current40WithSymbolRefs;
        static NPath[] _current40AllRefs;

        public static NPath[] Current40WithSymbolRefs => _current40WithSymbolRefs ?? (_current40WithSymbolRefs = symbolRefs.Select(d => Current40.Combine(d)).ToArray());
        public static NPath[] Current40AllRefs => _current40AllRefs ?? (_current40AllRefs = allRefs.Select(d => Current40.Combine(d)).ToArray());
    }

    public static class NRefactory
    {
        public static NPath[] Net40Assemblies => new NPath[]
        {
            "External/NRefactory/builds/5.5.1/net40/ICSharpCode.NRefactory.dll",
            "External/NRefactory/builds/5.5.1/net40/ICSharpCode.NRefactory.Cecil.dll",
            "External/NRefactory/builds/5.5.1/net40/ICSharpCode.NRefactory.CSharp.dll",
        };

        public static NPath Net35Assembly => "External/NRefactory/builds/3.2.1/net35/Unity.Legacy.NRefactory.dll";
    }

    public static class NUnitPaths
    {
        public static readonly NPath Framework = new NPath("External/NUnit/framework/net45/nunit.framework.dll");
    }

    public static class NodeJs
    {
        public static NPath NodeBinary
        {
            get
            {
                if (HostPlatform.IsWindows)
                    return "External/nodejs/builds/win64/node.exe";
                if (HostPlatform.IsOSX)
                    return "External/nodejs/builds/osx/bin/node";
                if (HostPlatform.IsLinux)
                    return "External/nodejs/builds/linux64/bin/node";
                throw new ArgumentException("Unknown platform");
            }
        }
    }
}
