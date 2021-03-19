using System;
using System.Collections.Generic;
using System.Linq;
using NiceIO;
using Unity.BuildTools;

namespace JamSharp.Runtime
{
    public class JamTarget
    {
        readonly string _targetName;

        static JamTarget()
        {
        }

        public JamTarget(NPath path)
        {
            _targetName = path.ToString();
        }

        public JamTarget(string name)
        {
            if (name == null)
                throw new ArgumentException();
            _targetName = name;
        }

        JamTarget(JamList jamlist)
        {
            _targetName = jamlist.Single();
        }

        public virtual string JamTargetName => _targetName ?? Path.ToString();

        public virtual NPath Path
        {
            get
            {
                if (_targetName == null)
                    throw new ArgumentException("No targetname specific, and Path is not overriden");
                return new NPath(_targetName);
            }
        }

        public void DependOn(IEnumerable<NPath> dependencies)
        {
            BuiltinRules.Depends(this, new JamList((IEnumerable<NPath>)dependencies.ToArray()));
        }

        public void DependOn(params JamTarget[] dependencies)
        {
            BuiltinRules.Depends(this, new JamList(dependencies));
        }

        public void DependOn(params string[] dependencies)
        {
            BuiltinRules.Depends(this, new JamList(dependencies));
        }

        public static implicit operator JamTarget(NPath path)
        {
            return new JamTarget(path);
        }

        public static implicit operator JamTarget(JamList jamlist)
        {
            return new JamTarget(jamlist);
        }

        public static implicit operator NPath(JamTarget target)
        {
            return target.Path;
        }

        public override string ToString()
        {
            return JamTargetName;
        }

        public string GetBoundPath()
        {
            return new JamList(JamTargetName).GetBoundPath().Single();
        }

        public string BoundIfNoPath()
        {
            if (JamTargetName.Contains("/") || JamTargetName.Contains("\\"))
                return new JamList(JamTargetName).SetGrist("").Single();
            return GetBoundPath();
        }
    }
}
