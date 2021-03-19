using System;
using System.Collections.Generic;
using System.Linq;
using Mono.Cecil;
using Mono.Cecil.Cil;
using Mono.Cecil.Rocks;

namespace Unity.BindingsGenerator.Core.AssemblyPatcher.Step
{
    internal class PassStructByRefInjector : Visitor
    {
        private readonly IList<Action> _postProcessActions;
        private readonly ScriptingBackend _scriptingBackend;

        public PassStructByRefInjector(IList<Action> postProcessActions, ScriptingBackend scriptingBackend)
        {
            _postProcessActions = postProcessActions;
            _scriptingBackend = scriptingBackend;
        }

        public override void Visit(MethodDefinition originalMethod)
        {
            Patch(originalMethod);
        }

        public void Patch(MethodDefinition originalMethod)
        {
            if (!originalMethod.RequiresBindingsGeneration())
                return;

            var originalMethodReturnsStruct = TypeUtils.IsStruct(originalMethod.ReturnType);
            var isExternInstanceMethodInBlittableStruct = TypeUtils.IsExternInstanceMethodInStruct(originalMethod, _scriptingBackend);

            var methodRequiresInjection = TypeUtils.MethodHasByValueStructParameter(originalMethod) || originalMethodReturnsStruct || isExternInstanceMethodInBlittableStruct;
            if (!methodRequiresInjection)
                return;

            var injectedMethod = new MethodDefinition(Naming.ByRefInjectedMethod(originalMethod.Name), originalMethod.Attributes, ReturnTypeForMethod(originalMethod));
            injectedMethod.IsPrivate = true;
            injectedMethod.ImplAttributes = MethodImplAttributes.InternalCall;
            injectedMethod.HasThis = originalMethod.HasThis;
            injectedMethod.ExplicitThis = originalMethod.ExplicitThis;
            injectedMethod.CallingConvention = originalMethod.CallingConvention;

            _postProcessActions.Add(() =>
            {
                if (isExternInstanceMethodInBlittableStruct)
                {
                    injectedMethod.Parameters.Add(new ParameterDefinition(Naming.Self, ParameterAttributes.None, originalMethod.DeclaringType.MakeByReferenceType()));
                    injectedMethod.IsStatic = true;
                    injectedMethod.HasThis = false;
                }

                foreach (var parameter in originalMethod.Parameters)
                {
                    var type = parameter.ParameterType;

                    ParameterDefinition injectedParameter;
                    if (TypeUtils.IsByValueStruct(type))
                        injectedParameter = new ParameterDefinition(parameter.Name, parameter.Attributes, parameter.ParameterType.MakeByReferenceType());
                    else
                        injectedParameter = new ParameterDefinition(parameter.Name, parameter.Attributes, parameter.ParameterType);

                    if (parameter.HasConstant)
                        injectedParameter.Constant = parameter.Constant;

                    foreach (var customAttribute in parameter.CustomAttributes)
                    {
                        if (!customAttribute.IsBindingsAttribute())
                            injectedParameter.CustomAttributes.Add(customAttribute);
                    }
                    injectedMethod.Parameters.Add(injectedParameter);
                }

                if (originalMethodReturnsStruct)
                {
                    injectedMethod.Parameters.Add(new ParameterDefinition(Naming.ReturnVar, ParameterAttributes.Out, originalMethod.ReturnType.MakeByReferenceType()));
                }

                originalMethod.DeclaringType.Methods.Add(injectedMethod);
            });

            originalMethod.Body = new MethodBody(originalMethod);
            originalMethod.ImplAttributes &= (~MethodImplAttributes.InternalCall);
            originalMethod.ImplAttributes |= MethodImplAttributes.Managed;

            VariableDefinition tempRetStruct = null;
            if (originalMethodReturnsStruct)
            {
                tempRetStruct = new VariableDefinition(originalMethod.ReturnType);
                originalMethod.Body.Variables.Add(tempRetStruct);
                originalMethod.Body.InitLocals = true;
            }

            var processor = originalMethod.Body.GetILProcessor();

            if (!originalMethod.IsStatic)
                processor.Append(Instruction.Create(OpCodes.Ldarg_0));

            foreach (var parameter in originalMethod.Parameters)
            {
                var type = parameter.ParameterType;

                if (TypeUtils.IsByValueStruct(type))
                    processor.Append(Instruction.Create(OpCodes.Ldarga, parameter));
                else
                    processor.Append(Instruction.Create(OpCodes.Ldarg, parameter));
            }

            if (originalMethodReturnsStruct)
            {
                processor.Append(Instruction.Create(OpCodes.Ldloca, tempRetStruct));
            }

            processor.Append(Instruction.Create(OpCodes.Call, injectedMethod));

            if (originalMethodReturnsStruct)
            {
                processor.Append(Instruction.Create(OpCodes.Ldloc_0));
            }

            processor.Append(Instruction.Create(OpCodes.Ret));

            originalMethod.Body.OptimizeMacros();
        }

        private static TypeReference ReturnTypeForMethod(MethodDefinition methodDefinition)
        {
            return !TypeUtils.IsStruct(methodDefinition.ReturnType)
                ? methodDefinition.ReturnType
                : methodDefinition.DeclaringType.Module.TypeSystem.Void;
        }
    }
}
