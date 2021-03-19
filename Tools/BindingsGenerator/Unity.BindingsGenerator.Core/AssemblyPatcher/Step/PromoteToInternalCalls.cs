using System;
using System.Collections.Generic;
using Mono.Cecil;

namespace Unity.BindingsGenerator.Core.AssemblyPatcher.Step
{
    internal class PromoteToInternalCalls : Visitor
    {
        public PromoteToInternalCalls(IList<Action> postProcessActions)
        {
        }

        public override void Visit(MethodDefinition methodDefinition)
        {
            VerifyAndMarkAsICall(methodDefinition);
        }

        private static void VerifyAndMarkAsICall(MethodDefinition methodDefinition)
        {
            if (!methodDefinition.RequiresBindingsGeneration())
                return;

            methodDefinition.IsInternalCall = true;
        }
    }
}
