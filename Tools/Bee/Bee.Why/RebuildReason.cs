using System.Collections.Generic;
using System.Linq;
using NiceIO;

namespace Bee.Why
{
    internal abstract class RebuildReason
    {
    }

    internal class ActionChangedReason : RebuildReason
    {
        public ActionChangedReason(string oldAction, string newAction)
        {
            OldAction = oldAction;
            NewAction = newAction;
        }

        public string OldAction { get; }
        public string NewAction { get; }
    }

    internal class NotBuiltBeforeReason : RebuildReason
    {
    }

    internal class PreviousBuildFailedReason : RebuildReason
    {
    }

    internal class OutputsMissingReason : RebuildReason
    {
        public OutputsMissingReason(IEnumerable<NPath> outputsMissing)
        {
            OutputsMissing = outputsMissing.ToArray();
        }

        public NPath[] OutputsMissing { get; }
    }

    internal class InputListChangedReason : RebuildReason
    {
        public InputListChangedReason(string dependencyType, IEnumerable<NPath> oldList, IEnumerable<NPath> newList)
        {
            DependencyType = dependencyType;
            OldList = oldList.ToArray();
            NewList = newList.ToArray();
        }

        public string DependencyType { get; }
        public NPath[] OldList { get; }
        public NPath[] NewList { get; }

        public IEnumerable<NPath> FilesAdded => NewList.Except(OldList);
        public IEnumerable<NPath> FilesRemoved => OldList.Except(NewList);
    }

    internal abstract class InputChangedReason : RebuildReason
    {
        protected InputChangedReason(NPath file)
        {
            File = file;
        }

        public NPath File { get; }

        public abstract string Method { get; }
    }

    internal class InputHashChangedReason : InputChangedReason
    {
        public InputHashChangedReason(NPath file) : base(file)
        {
        }

        public override string Method => "timestamp and sha1";
    }

    internal class InputTimeStampChangedReason : InputChangedReason
    {
        public InputTimeStampChangedReason(NPath file) : base(file)
        {
        }

        public override string Method => "timestamp";
    }

    internal class NoGoodReason : RebuildReason
    {
    }

    internal class ResponseFileChangedReason : RebuildReason
    {
        public string OldContent { get; }
        public string NewContent { get; }

        public ResponseFileChangedReason(string oldContent, string newContent)
        {
            OldContent = oldContent;
            NewContent = newContent;
        }
    }

    internal class DependencyChanged : RebuildReason
    {
        public InputChangedReason RawChange { get; }
        public BuiltNodeInfo DependencyInfo { get; }

        public DependencyChanged(BuiltNodeInfo dependency, InputChangedReason raw)
        {
            DependencyInfo = dependency;
            RawChange = raw;
        }
    }
}
