using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Reflection;
using Unity.TinyProfiling;

namespace Unity.BuildSystem
{
    //Jam's Jambase.jam has a rule called SubInclude. it basically executes a jamfile, and keeps track that if it was already SubIncluded before, that we do not run it again.
    //It also sets up a bunch of global state like SUBDIR, SEARCH_SOURCE, etc.  We want to get rid of all that, and turn these invocations into direct c# method invocations.
    //This SubIncludeRunner is a first step towards that. It's an improvement on SubInclude("Called","With","Strings"), as you pass in a reference to a method so Find All Usages
    //starts working.  Over time we will reduce our dependency on this global state, so we no longer have to keep restoring it, and we  will also remove the need to check
    //if something is only called once.
    public class SubIncludeRunner
    {
        readonly HashSet<MethodInfo> m_Invoked = new HashSet<MethodInfo>();

        public void RunOnce(Action a)
        {
            ValidateArgument(a);
            if (m_Invoked.Contains(a.Method))
                return;
            m_Invoked.Add(a.Method);
            using (new SubIncludeGlobalStateBackup())
            {
                using (TinyProfiler.Section(a.Method.DeclaringType?.Name))
                {
                    a();
                }
            }
        }

        [DebuggerStepThrough]
        void ValidateArgument(Delegate d)
        {
            if (!d.Method.IsStatic)
                throw new ArgumentException("Only static methods are supported");
            if (d.Method.GetParameters().Any())
                throw new ArgumentException($"Method {d.Method.Name} has parameters, this is not supported");
        }
    }
}
