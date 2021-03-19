using System;
using System.Collections.Generic;
using Unity.BindingsGenerator.Core.AssemblyPatcher.Step;

namespace Unity.BindingsGenerator.Core.AssemblyPatcher
{
    public class AssemblyPatcher
    {
        private NonFilteringVisitor _visitor;
        private readonly ScriptingBackend _scriptingBackend;

        public AssemblyPatcher(NonFilteringVisitor visitor, ScriptingBackend scriptingBackend)
        {
            _visitor = visitor;
            _scriptingBackend = scriptingBackend;
        }

        public AssemblyPatcher Patch()
        {
            var postProcessActions = new List<Action>();

            // Used to remove attributes and generate method overloads for default parameters. see 9f74a1a28f71 for code
            var patcher = this
                .PromoteToInternalCalls(postProcessActions)
                .PassStructsByRef(postProcessActions)
                .RunPostProcessSteps(postProcessActions)
            ;

            return patcher;
        }

        private AssemblyPatcher PromoteToInternalCalls(IList<Action> postProcessActions)
        {
            _visitor.Accept(new PromoteToInternalCalls(postProcessActions));
            return this;
        }

        private AssemblyPatcher PassStructsByRef(IList<Action> postProcessActions)
        {
            _visitor.Accept(new PassStructByRefInjector(postProcessActions, _scriptingBackend));
            return this;
        }

        private AssemblyPatcher RunPostProcessSteps(IList<Action> postProcessActions)
        {
            foreach (var action in postProcessActions)
            {
                action.Invoke();
            }

            return this;
        }
    }
}
