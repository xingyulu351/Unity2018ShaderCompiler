using System;
using System.Collections.Generic;
using System.Linq;

namespace JamSharp.Runtime
{
    public class GlobalVariables
    {
        static GlobalVariables s_Singleton;

        public static string[] BlacklistedGlobalVariables { get; } =
        {
            "", // Jam has a variable with an empty name for some reason...
            "JAMDATE",
            "JAMVERSION",
            "DPG_JAMVERSION",
            "PATCHED_JAM_VERSION",
            "JAM_NUM_JOBS",
            "JAM_MULTI_JOBS",
            "JAM_JOB_LIST",
            "JAM_PROCESS_PATH",
            "JAMUNAME",
            "DEPCACHE",

            // This one is causing is problems on Katana - the slashes are coming out the wrong way around on
            // Windows machines. Something to do with Python/cygwin probably... but we don't appear to use it,
            // so we'll just blacklist it.
            "PWD",

            // Leftover variables from JamEntryPoint.jam:
            "temp",
            "mytop",
            "DEBUG",
            "DEBUG_BUILDPROGRAM",
            "DEBUG_PORT",
            "monobuilds",
            "mcs",
            "monoDll",
            "p",
            "winArch",
            "optionsString",
            "address",
            "port",
            "addressAndPort"
        };

        public GlobalVariables()
        {
            Storage = new CSharpVariableStorage();
        }

        public static GlobalVariables Singleton => s_Singleton ?? (s_Singleton = new GlobalVariables());

        public CSharpVariableStorage Storage { get; }

        public JamList this[string variableName]
        {
            get
            {
                if (BlacklistedGlobalVariables.Contains(variableName))
                    throw new ArgumentException($"GlobalVariable \"{variableName}\" is blacklisted and must not be used.");
                return Storage.GlobalVariable(variableName);
            }
        }

        public JamList[] GetOrCreateVariableOnTargetContext(JamList targetNames, JamList variableNames)
        {
            var result = new List<JamList>();

            foreach (var targetName in targetNames.Elements)
            {
                foreach (var variableName in variableNames.Elements)
                    result.Add(Storage.VariableOnTarget(variableName, targetName));
            }
            return result.ToArray();
        }

        public JamList[] DereferenceElementsNonFlat(JamList variableNames)
        {
            return variableNames.Elements.Select(e => Storage.GlobalVariable(e)).ToArray();
        }

        public JamList DereferenceElements(string[] variableNames)
        {
            var result = new List<string>();
            foreach (var variableName in variableNames)
            {
                var jamList = this[variableName];
                var collection = jamList.Elements;
                result.AddRange(collection);
            }

            return new JamList(result.ToArray());
        }

        public void PushSettingsFor(string target)
        {
            Storage.PushSettingsFor(target);
        }

        public void PopSettingsFor(string target)
        {
            Storage.PopSettingsFor(target);
        }

        class StringTable
        {
            readonly Dictionary<string, StringTableEntry> _cache = new Dictionary<string, StringTableEntry>();

            readonly List<string> _strings = new List<string>();

            public string[] StringArray => _strings.ToArray();

            StringTableEntry EntryFor(string s0)
            {
                if (s0 == null)
                    throw new ArgumentNullException(nameof(s0));
                StringTableEntry result;
                if (_cache.TryGetValue(s0, out result))
                    return result;
                _strings.Add(s0);

                result = new StringTableEntry { index = _strings.Count() - 1 };
                _cache[s0] = result;
                return result;
            }

            public StringTableEntry[] EntryFor(string[] strings)
            {
                var result = new StringTableEntry[strings.Length];
                for (int i = 0; i != result.Length; i++)
                    result[i] = EntryFor(strings[i]);
                return result;
            }

            public string Get(StringTableEntry e)
            {
                return _strings[e.index];
            }
        }
    }
}
