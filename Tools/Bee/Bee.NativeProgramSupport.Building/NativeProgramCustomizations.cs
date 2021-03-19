using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using NiceIO;
using Unity.BuildSystem.NativeProgramSupport;

namespace Bee.NativeProgramSupport.Building
{
    public class LinkerCustomizations : Customizations<ObjectFileLinker>
    {
    }

    public class CompilerCustomizations : Customizations<CLikeCompilerSettings>
    {
    }

    public interface ICustomizationFor<TSettings> : IEnumerable
    {
        void Add(Func<TSettings, TSettings> customization);
        void Add(Func<TSettings, TSettings> customization, params NPath[] onlyOnThesePaths);
        void Add(Func<NativeProgramConfiguration, bool> condition, Func<TSettings, TSettings> customization);
        void Add(Func<NativeProgramConfiguration, bool> condition, Func<TSettings, TSettings> customization, params NPath[] onlyOnTheseFiles);
        TSettings For(NativeProgramConfiguration config, TSettings settings, NPath inputFile);
    }

    public abstract class CustomizationsBase<TSettings> : ICustomizationFor<TSettings>
    {
        public void Add(Func<TSettings, TSettings> customization) => Add(null, customization);
        public void Add(Func<TSettings, TSettings> customization, params NPath[] onlyOnThesePaths) => Add(null, customization, onlyOnThesePaths);
        public void Add(Func<NativeProgramConfiguration, bool> condition, Func<TSettings, TSettings> customization) => Add(condition, customization, null);
        public abstract void Add(Func<NativeProgramConfiguration, bool> condition, Func<TSettings, TSettings> customization, params NPath[] onlyOnTheseFiles);
        public abstract TSettings For(NativeProgramConfiguration config, TSettings settings, NPath inputFile);

        IEnumerator IEnumerable.GetEnumerator()
        {
            throw new NotImplementedException();
        }
    }

    public abstract class Customizations<TSettings> : CustomizationsBase<TSettings>
    {
        struct Entry
        {
            public Type Type;
            public Func<TSettings, TSettings> Callback;
            public HashSet<NPath> Path;
            public Func<NativeProgramConfiguration, bool> Condition;
        }

        readonly List<Entry> _entries = new List<Entry>();

        internal ICustomizationFor<T> TypedViewFor<T>(Func<NativeProgramConfiguration, bool> condition) where T : TSettings
        {
            return new TypedCustomizationFor<T>(this, condition);
        }

        public override void Add(Func<NativeProgramConfiguration, bool> condition, Func<TSettings, TSettings> callback, params NPath[] onlyOnTheseFiles)
        {
            Add(condition, callback, onlyOnTheseFiles);
        }

        internal void Add<T>(Func<T, T> callback, params NPath[] paths) where T : TSettings
        {
            Add(null, callback, paths);
        }

        private void Add<T>(Func<NativeProgramConfiguration, bool> condition, Func<T, T> callback, NPath[] onlyOnTheseFiles = null) where T : TSettings
        {
            var pathsHashSet = (onlyOnTheseFiles != null && onlyOnTheseFiles.Any()) ? new HashSet<NPath>(onlyOnTheseFiles) : null;
            _entries.Add(new Entry() {Condition = condition, Type = typeof(T), Callback = c => callback((T)c), Path = pathsHashSet});
        }

        public override TSettings For(NativeProgramConfiguration config, TSettings settings, NPath path)
        {
            var eligableEntries = _entries.Where(e => IsEntryEligable(config, settings, path, e)).ToList();
            foreach (var entry in eligableEntries)
                settings = entry.Callback.Invoke(settings);

            return settings;
        }

        private static bool IsEntryEligable(NativeProgramConfiguration config, TSettings settings, NPath path, Entry entry)
        {
            if (entry.Condition != null && !entry.Condition(config))
                return false;

            if (!entry.Type.IsInstanceOfType(settings))
                return false;

            if (entry.Path != null && !entry.Path.Contains(path))
                return false;

            return true;
        }

        class TypedCustomizationFor<T> : CustomizationsBase<T> where T : TSettings
        {
            readonly Customizations<TSettings> _customization;
            readonly Func<NativeProgramConfiguration, bool> _condition;

            Func<NativeProgramConfiguration, bool> Condition => _condition ?? (arg => true);

            public TypedCustomizationFor(Customizations<TSettings> customization,
                                         Func<NativeProgramConfiguration, bool> condition)
            {
                _customization = customization;
                _condition = condition;
            }

            public override void Add(Func<NativeProgramConfiguration, bool> condition, Func<T, T> customization, params NPath[] onlyOnTheseFiles)
            {
                var customizations = _customization;

                if (condition == null && _condition == null)
                    customizations.Add(customization, onlyOnTheseFiles);

                condition = condition ?? (arg => true);

                customizations.Add(c => Condition(c) && condition(c), customization, onlyOnTheseFiles);
            }

            public override T For(NativeProgramConfiguration config, T settings, NPath inputFile)
            {
                throw new NotImplementedException();
            }
        }
    }
}
