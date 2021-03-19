using System;
using System.Collections.Generic;
using System.Collections.Specialized;
using System.Runtime.InteropServices;
using NiceIO;
using Unity.BuildSystem;
using Unity.BuildTools;

namespace Bee.Core
{
    public interface IBackend
    {
        void AddWriteTextAction(NPath file, string contents, string actionName = "WriteText");
        void AddDependency(NPath from, NPath to);
        void AddDependency(NPath from, IEnumerable<NPath> to);

        void ScanCppFileForHeaders(NPath objectFile, NPath cppFile, NPath[] includeDirectories);

        void AddAction(
            string actionName,
            NPath[] targetFiles,
            NPath[] inputs,
            string executableStringFor,
            string[] commandLineArguments,
            bool supportResponseFile = false,
            Dictionary<string, string> environmentVariables = null,
            NPath[] rspAndBatchFiles = null,
            bool allowUnexpectedOutput = true,
            bool allowUnwrittenOutputFiles = false,
            string[] allowedOutputSubstrings = null);
        void AddAliasDependency(string alias, NPath path);

        void AddImplicitDependenciesToAllActionsWithInputsThatLiveIn(NPath directory, NPath makeDependOnThis);
        IEnumerable<NPath> AllDirectoriesProducingImplicitInputs { get; }

        void RegisterGlobInfluencingGraph(NPath directory);
        void RegisterFileInfluencingGraph(params NPath[] files);
        void AddAliasToAliasDependency(string alias, string otherAlias);

        bool CanHandleAnsiColors { get; }
        void Register(IBackendRegistrable br);
    }

    /// <summary>
    /// A set of actions, implicit dependencies, or similar, that can be registered with a backend.
    /// </summary>
    public interface IBackendRegistrable
    {
        void Register(IBackend backend);
    }

    public interface IJamBackend : IBackend
    {
        void NotifyOfLump(NPath lumpCpp, NPath[] membersOfLump);
    }

    public abstract class Backend
    {
        private static IBackend _current;
        public static IBackend Current
        {
            get
            {
                if (_current == null && System.Diagnostics.Debugger.IsAttached)
                {
                    Current = new DummyBackend();
                    return Current;
                }

                return _current;
            }
            set { _current = value; }
        }

        readonly HashSet<IBackendRegistrable> _previouslyRegistered = new HashSet<IBackendRegistrable>();
        public HashSet<IBackendRegistrable> PreviouslyRegistered => _previouslyRegistered;

        public void Register(IBackendRegistrable o)
        {
            if (_previouslyRegistered.Add(o))
                o.Register((IBackend)this);
        }
    }

    public class DummyBackend : IBackend
    {
        public void AddWriteTextAction(NPath file, string contents, string actionName = "WriteText")
        {
        }

        public void AddDependency(NPath @from, NPath to)
        {
        }

        public void AddDependency(NPath @from, IEnumerable<NPath> to)
        {
        }

        public void ScanCppFileForHeaders(NPath objectFile, NPath cppFile, NPath[] includeDirectories)
        {
        }

        public void AddAction(
            string actionName,
            NPath[] targetFiles,
            NPath[] inputs,
            string executableStringFor,
            string[] commandLineArguments,
            bool supportResponseFile = false,
            Dictionary<string, string> environmentVariables = null,
            NPath[] rspAndBatchFiles = null,
            bool allowUnexpectedOutput = true,
            bool allowUnwrittenOutputFiles = false,
            string[] allowedOutputSubstrings = null)
        {
        }

        public void AddAliasDependency(string alias, NPath path)
        {
        }

        public void AddImplicitDependenciesToAllActionsWithInputsThatLiveIn(NPath directory, NPath makeDependOnThis)
        {
        }

        public IEnumerable<NPath> AllDirectoriesProducingImplicitInputs => new NPath[0];

        public void RegisterGlobInfluencingGraph(NPath directory)
        {
        }

        public void RegisterFileInfluencingGraph(params NPath[] files)
        {
        }

        public void AddAliasToAliasDependency(string alias, string otherAlias)
        {
        }

        public bool CanHandleAnsiColors { get; }

        public void Register(IBackendRegistrable br)
        {
        }
    }
}
