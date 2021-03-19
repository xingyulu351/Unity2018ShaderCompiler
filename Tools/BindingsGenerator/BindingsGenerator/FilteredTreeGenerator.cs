using Mono.Cecil;
using Unity.BindingsGenerator.Core;

namespace BindingsGenerator
{
    internal sealed class FilteredTreeGenerator : TreeGenerator
    {
        private readonly TypeFilter _filter;

        public FilteredTreeGenerator(string inputFile, string[] defines, ScriptingBackend scriptingBackend)
            : base(scriptingBackend)
        {
            _filter = new TypeFilter(inputFile, defines);
        }

        public override void Visit(MethodDefinition methodDefinition)
        {
            if (!_filter.IsDeclaredInTheRightInputFile(methodDefinition))
                return;

            base.Visit(methodDefinition);
        }

        public override void Visit(PropertyDefinition propertyDefinition)
        {
            if (!_filter.IsDeclaredInTheRightInputFile(propertyDefinition))
                return;

            GenerateProperty(propertyDefinition);
        }
    }
}
