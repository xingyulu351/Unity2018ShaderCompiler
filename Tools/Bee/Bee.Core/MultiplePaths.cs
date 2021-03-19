using System;
using System.Linq;
using NiceIO;

namespace Bee.Core
{
    public abstract class MultiplePaths
    {
        protected MultiplePaths(params NPath[] paths)
        {
            ValidateInputPaths(paths);
            Paths = paths;
        }

        private void ValidateInputPaths(NPath[] paths)
        {
            if (paths.Length == 0)
                throw new ArgumentException($"{GetType().Name} must have at least one path");

            NPath parentDir = null;

            foreach (var path in paths)
            {
                if (path == null)
                    continue;
                if (parentDir == null)
                {
                    parentDir = path.Parent;
                    continue;
                }

                if (path.Parent != parentDir)
                    throw new ArgumentException(
                        $"All files in MultiplePaths must share the same parent. found parent; {parentDir} and parent {path.Parent}");
            }

            if (parentDir == null)
                throw new ArgumentException($"{GetType().Name} cannot have paths that are all null");
        }

        public NPath[] Paths { get; internal set; }

        internal T With<T>(Action<T> modification)
        {
            var result = (T)MemberwiseClone();
            modification(result);
            return result;
        }
    }

    public static class MultiplePathsExtensions
    {
        public static T WithNewParentDirectory<T>(this T multiplePaths, NPath newParent) where T : MultiplePaths
        {
            return multiplePaths.With((T copy) =>
            {
                copy.Paths = multiplePaths.Paths.Select(p => p == null ? null : newParent.Combine(p.FileName)).ToArray();
            });
        }
    }
}
