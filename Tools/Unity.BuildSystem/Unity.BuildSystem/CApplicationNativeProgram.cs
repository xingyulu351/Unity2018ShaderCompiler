using System;
using System.Collections.Generic;
using System.Linq;
using JamSharp.Runtime;
using NiceIO;
using Unity.BuildSystem.NativeProgramSupport;
using Unity.BuildTools;

namespace Unity.BuildSystem
{
    // NativeProgram object used while generating IDE project files
    public class CApplicationNativeProgram : NativeProgram
    {
        static readonly Dictionary<string, CApplicationNativeProgram> Programs = new Dictionary<string, CApplicationNativeProgram>();

        public static IEnumerable<CApplicationNativeProgram> GetOrMake(JamList targets)
        {
            return GetOrMake(targets.Elements);
        }

        public static IEnumerable<CApplicationNativeProgram> GetOrMake(params string[] names)
        {
            foreach (var target in names)
            {
                CApplicationNativeProgram result;
                if (Programs.TryGetValue(target, out result))
                {
                    yield return result;
                    continue;
                }
                result = new CApplicationNativeProgram(target);
                Programs[target] = result;
                yield return result;
            }
        }

        public static CApplicationNativeProgram GetOrMake(string name) => GetOrMake(new[] {name}).Single();

        public static NativeProgram Get(string name) => Get(new[] {name}).Single();

        public static NativeProgram[] Get(params string[] names)
        {
            var result = new List<NativeProgram>();
            foreach (var name in names)
            {
                if (!Programs.ContainsKey(name))
                    throw new ArgumentException($"{nameof(CApplicationNativeProgram) + nameof(CApplicationNativeProgram.Get)} invoked with argument {name} but there's no program registered with this name");
                result.Add(Programs[name]);
            }
            return result.ToArray();
        }

        public static IEnumerable<NativeProgram> All => Programs.Values;

        private CApplicationNativeProgram(string target) : base(target)
        {
        }

        public void SetOutputName(JamList name, JamList config, JamList platform)
        {
            var condition = ConditionFor(config, platform);
            OutputName.Set(condition, name.Single());
        }

        public void SetOutputDirectory(JamList name, JamList config, JamList platform)
        {
            var condition = ConditionFor(config, platform);
            OutputDirectory.Set(condition, name.Single());
        }

        public void SetOutputExtension(JamList extension, JamList config, JamList platform)
        {
            var condition = ConditionFor(config, platform);

            OutputExtension.Set(condition, extension.Single());
        }

        public void SetOutputName(JamList name, Func<NativeProgramConfiguration, bool> condition)
        {
            OutputName.Set(condition, name.Single());
        }

        public void SetOutputDirectory(JamList name, Func<NativeProgramConfiguration, bool> condition)
        {
            OutputDirectory.Set(condition, name.Single());
        }

        public void AddDefines(JamList defines, JamList config, JamList platform)
        {
            var condition = ConditionFor(config, platform);
            AddDefines(defines, condition);
        }

        public void AddDefines(JamList defines, Func<NativeProgramConfiguration, bool> condition)
        {
            foreach (var define in defines.Elements)
            {
                base.Defines.Add(condition, define);
            }
        }

        static NPath AdjustRelativePath(NPath p)
        {
            if (p.IsRelative)
            {
                var searchSource = GlobalVariables.Singleton["SEARCH_SOURCE"];
                if (searchSource.Any())
                {
                    NPath folderForRelative = searchSource.Elements.Single();
                    p = folderForRelative.Combine(p);
                }
            }
            return p;
        }

        public void AddIncludeDirectories(JamList includeDirs, JamList config, JamList platform)
        {
            var condition = ConditionFor(config, platform);
            foreach (var includeDir in includeDirs.Elements)
            {
                if (string.IsNullOrEmpty(includeDir))
                    continue;
                var path = new NPath(includeDir);
                path = AdjustRelativePath(path);
                if (!path.IsRelative && path.IsChildOf(Paths.UnityRoot))
                    path = path.RelativeTo(Paths.UnityRoot);
                base.IncludeDirectories.Add(condition, path);
            }
        }

        public void AddSources(JamList srcs)
        {
            var g = GlobalVariables.Singleton;
            foreach (var src in srcs.Elements)
            {
                var lumpSources = g.GetOrCreateVariableOnTargetContext(src, "LUMPSOURCES").Single();
                if (lumpSources.Elements.Any())
                {
                    foreach (var lumpSource in lumpSources.Elements)
                        AddSingleSource(lumpSource);
                    continue;
                }

                AddSingleSource(src);
            }

            var rootPath = AdjustRelativePath("");
            if (!IncludeDirectories.ValuesWithoutCondition.Contains(rootPath))
                IncludeDirectories.Add(rootPath);
        }

        void AddSingleSource(string src)
        {
            src = JamList.StripGrist(src);
            var p = new NPath(src);
            p = AdjustRelativePath(p);
            if (!p.IsRelative && p.IsChildOf(Paths.UnityRoot))
                p = p.RelativeTo(Paths.UnityRoot);
            Sources.Add(p);
        }

        Func<NativeProgramConfiguration, bool> ConditionFor(JamList config, JamList platform)
        {
            if (config == null && platform == null)
                return null;
            if (config.Empty() && platform.Empty())
                return null;
            return nativeProgramConfig => MatchesConfig(config, nativeProgramConfig.CodeGen) && MatchesPlatform(platform, nativeProgramConfig.ToolChain.LegacyPlatformIdentifier);
        }

        bool MatchesConfig(JamList config, CodeGen nConfig)
        {
            return config.Elements.Empty() || config.Elements.Any(e => e == "*") || config.Elements.Contains(nConfig.ToJam());
        }

        bool MatchesPlatform(JamList plat, string nPlatform)
        {
            return plat.Elements.Empty() || plat.Elements.Any(e => e == "*") || plat.Elements.Contains(nPlatform);
        }

        public void C_LinkPrebuiltLibraries(JamList libraries, JamList config, JamList platform)
        {
            PrebuiltLibraries.Add(ConditionFor(config, platform), libraries.Select(s => new LegacyCApplicationLibrary(s)));
        }

        public void AddPerFileDefines(JamList sources, JamList defines, JamList config, JamList platform)
        {
            sources = sources.GetBoundPath();
            PerFileDefines.Add(ConditionFor(config, platform), defines, ProcessPaths(sources));
        }

        public void AddPerFileIncludeDirectories(JamList sources, JamList includepaths, JamList config, JamList platform)
        {
            PerFileIncludeDirectories.Add(ConditionFor(config, platform), includepaths.Select(i => new NPath(i)), ProcessPaths(sources));
        }

        IEnumerable<NPath> ProcessPaths(JamList paths)
        {
            paths = paths.GetBoundPath();
            return paths.Select(p => new NPath(p));
        }
    }
}
