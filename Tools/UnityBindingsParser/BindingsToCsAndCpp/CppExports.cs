using System;
using System.Collections.Generic;
using System.Linq;
using System.Reflection;

namespace BindingsToCsAndCpp
{
    class CppExports : IExports
    {
        public int EntryRepeats { get; set; }
        public string TargetPlatform { get; set; }

        public string Output { get { return output.Output; } }

        protected IndentedWriter output = new IndentedWriter();

        readonly List<IExports> _customExports;
        readonly List<CppUnit> _units;
        private readonly string _filename;

        public static CppExports Prepare(string filename, List<CppUnit> units, string platform)
        {
            var customExports = new List<IExports>();

            var types = Assembly.GetExecutingAssembly().GetTypes().Where(x => x.GetInterface("IExports") != null && x.GetCustomAttributes(typeof(OrderAttribute), false).Count() == 1);
            var list = types.OrderBy(d => ((OrderAttribute)d.GetCustomAttributes(typeof(OrderAttribute), false)[0]).Order);
            foreach (var type in list)
            {
                var x = Activator.CreateInstance(type) as IExports;
                x.TargetPlatform = platform;
                customExports.Add(x);
            }
            return new CppExports(customExports, units, filename);
        }

        public CppExports()
        {
            EntryRepeats = 1;
        }

        CppExports(List<IExports> customExports, List<CppUnit> units, string filename)
        {
            _customExports = customExports;
            _units = units;
            _filename = filename;
        }

        public bool IsTargetMetro
        {
            get
            {
                return string.Equals(TargetPlatform, "metro", StringComparison.CurrentCultureIgnoreCase)
                    || string.Equals(TargetPlatform, "wp8", StringComparison.CurrentCultureIgnoreCase);
            }
        }

        public void Generate()
        {
            foreach (var export in _customExports)
            {
                var str = export.GenerateHeader(this, _filename);
                output.Append(str);

                for (var i = 0; i < export.EntryRepeats; i++)
                {
                    str = export.GenerateEntriesHeader(this, _filename, i);
                    output.Append(str);

                    foreach (var entry in _units)
                    {
                        str = export.GenerateEntry(this, _filename, i, entry);
                        output.Append(str);
                    }

                    str = export.GenerateEntriesFooter(this, _filename, i);
                    output.Append(str);
                }

                str = export.GenerateFooter(this, _filename);
                output.Append(str);
            }
            output.AppendLine("#endif");
        }

        public virtual string GenerateHeader(CppExports sender, string filename)
        {
            var str = output.ToString();
            output.Clear();
            return str;
        }

        public virtual string GenerateEntriesHeader(CppExports sender, string filename, int count)
        {
            var str = output.ToString();
            output.Clear();
            return str;
        }

        public virtual string GenerateEntry(CppExports sender, string filename, int count, CppProperty entry)
        {
            var str = output.ToString();
            output.Clear();
            return str;
        }

        public virtual string GenerateEntry(CppExports sender, string filename, int count, CppUnit entry)
        {
            var str = output.ToString();
            output.Clear();
            return str;
        }

        public virtual string GenerateEntriesFooter(CppExports sender, string filename, int count)
        {
            var str = output.ToString();
            output.Clear();
            return str;
        }

        public virtual string GenerateFooter(CppExports sender, string filename)
        {
            var str = output.ToString();
            output.Clear();
            return str;
        }

        bool _isInsideClassCondition;
        bool _isInsideCondition;

        public Unit GetPreviousClass(CppUnit current)
        {
            var idx = _units.FindIndex(x => x == current);
            if (idx <= 0)
                return current.Parent;
            var unit = _units.Take(idx).LastOrDefault(x => x.Parent != current.Parent && (x is CppProperty || x is CppMethod));
            return unit != null ? unit.Parent : current.Parent;
        }

        public CppUnit GetPreviousUnit(CppUnit current)
        {
            var idx = _units.FindIndex(x => x == current);
            return idx > 0 ? _units.Take(idx).LastOrDefault(x => x is CppProperty || x is CppMethod) : null;
        }

        public void OutputConditions(CppUnit current)
        {
            OutputConditions(current, GetPreviousUnit(current));
        }

        public void FinishConditions()
        {
            if (!_units.Any(x => x is CppProperty || x is CppMethod))
                return;

            if (_isInsideCondition)
                output.AppendEndConditions(_units.Last(x => x is CppProperty || x is CppMethod).Conditions);

            var klass = _units.Last(x => x is CppProperty || x is CppMethod).Parent;
            if (_isInsideClassCondition && klass != null)
                output.AppendEndConditions(klass.Conditions);

            _isInsideCondition = _isInsideClassCondition = false;
        }

        void OutputConditions(Unit current, Unit previousBlock)
        {
            var currentClass = current.Parent;
            var previousBlockClass = previousBlock != null && previousBlock.Parent != null ? previousBlock.Parent : currentClass;

            var currentBlockConditions = current.Conditions;

            if (!_isInsideClassCondition && !_isInsideCondition && currentBlockConditions.Count == 0)
                return;

            // if this block doesn't have its own #if conditions
            if (currentClass != null && currentBlockConditions.SequenceEqual(currentClass.Conditions))
            {
                // if we're already inside an #if opened by a class
                if (_isInsideClassCondition &&
                    // and the conditions of the #if are different from the current ones
                    currentClass != previousBlockClass && !currentClass.Conditions.SequenceEqual(previousBlockClass.Conditions))
                {
                    // close the #if opened by the previous class
                    output.AppendEndConditions(previousBlockClass.Conditions);
                    output.AppendStartConditions(currentBlockConditions);
                    _isInsideClassCondition = currentBlockConditions.Count > 0;
                }
                else if (!_isInsideClassCondition)
                {
                    if (_isInsideCondition)
                        output.AppendEndConditions(previousBlock.Conditions);
                    output.AppendStartConditions(currentBlockConditions);
                    _isInsideClassCondition = currentBlockConditions.Count > 0;
                    _isInsideCondition = false;
                }
            }
            else // the current block sets its own conditions
            {
                // if we're inside an #if set by a class
                if (_isInsideClassCondition)
                {
                    // only close the previous #if and open a new one if the previous #if conditions
                    // are different than the current ones. just saves a few sequences of #endif/#if
                    if (!previousBlockClass.Conditions.SequenceEqual(currentBlockConditions))
                    {
                        output.AppendEndConditions(previousBlockClass.Conditions);
                        output.AppendStartConditions(currentBlockConditions);
                    }
                    _isInsideCondition = currentBlockConditions.Count > 0;
                    _isInsideClassCondition = false;
                }
                // if we're inside an #if set by another block
                else if (_isInsideCondition)
                {
                    // if the previous #if is the same as the current one, don't close and open the #if
                    if (previousBlock != null && !currentBlockConditions.SequenceEqual(previousBlock.Conditions))
                    {
                        output.AppendEndConditions(previousBlock.Conditions);
                        output.AppendStartConditions(currentBlockConditions);
                        _isInsideCondition = currentBlockConditions.Count > 0;
                    }
                }
                else
                {
                    output.AppendStartConditions(currentBlockConditions);
                    _isInsideCondition = currentBlockConditions.Count > 0;
                }
            }
        }

        protected string GetClassName(CppUnit unit, string separator = "_")
        {
            var ret = "";
            var parent = unit.Parent;
            while (parent != null)
            {
                ret = parent.Name + separator + ret;
                parent = parent.Parent;
            }
            return ret.Length > 0 ? ret.Substring(0, ret.Length - separator.Length) : unit.ClassName;
        }
    }

    class OrderAttribute : Attribute
    {
        public int Order;

        public OrderAttribute(int order)
        {
            Order = order;
        }
    }

    interface IExports
    {
        int EntryRepeats { get; set; }
        string TargetPlatform { get; set; }

        string Output { get; }

        string GenerateHeader(CppExports sender, string filename);
        string GenerateEntriesHeader(CppExports sender, string filename, int count);
        string GenerateEntry(CppExports sender, string filename, int count, CppProperty entry);
        string GenerateEntry(CppExports sender, string filename, int count, CppUnit entry);
        string GenerateEntriesFooter(CppExports sender, string filename, int count);
        string GenerateFooter(CppExports sender, string filename);
    }
}
