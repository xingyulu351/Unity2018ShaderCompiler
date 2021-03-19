using System;
using System.Collections.Generic;
using System.Linq;
using Mono.Cecil;
using Mono.Cecil.Rocks;

namespace AssemblyPatcher.Extensions
{
    static class CecilExtensions
    {
        public static bool HasCompatibleParametersWith(this MethodReference self, MethodReference other)
        {
            //TODO: Take into account type patching / mapping.
            var sourceParams = self.Parameters;
            var otherParams = other.Parameters;

            if (sourceParams.Count != otherParams.Count)
                return false;

            for (int i = 0; i < sourceParams.Count; i++)
            {
                var sourceParam = sourceParams[i].ParameterType as GenericParameter;
                var otherParam = otherParams[i].ParameterType as GenericParameter;
                if (sourceParam != null && otherParam != null)
                {
                    return sourceParam.Position == otherParam.Position;
                }

                if (sourceParams[i].ParameterType.FullName != otherParams[i].ParameterType.FullName)
                    return false;
            }

            return true;
        }

        public static bool HasCompatibleSignatureWithUnsupportedMethod(this MethodDefinition candidate, MethodReference original, Func<TypeReference, TypeReference> typeReferenceMapper)
        {
            if (candidate.ReturnType.FullName != original.ReturnType.FullName)
                return false;

            var originalResolved = original.Resolve();
            Func<int, int> fixStaticMethodOffset = index => index + (originalResolved.IsStatic ? 0 : 1);

            var originalParamCount = ActualParameterCount(original);
            if (candidate.Parameters.Count != originalParamCount + 1)
                return false;

            for (int i = 0; i < original.Parameters.Count; i++)
            {
                if (candidate.Parameters[fixStaticMethodOffset(i)].ParameterType.FullName != typeReferenceMapper(original.Parameters[i].ParameterType).FullName)
                {
                    return false;
                }
            }

            if (!originalResolved.IsStatic && candidate.Parameters[0].ParameterType.FullName != typeReferenceMapper(original.DeclaringType).FullName)
            {
                return false;
            }

            // Check the final string param.
            for (int i = originalParamCount; i < candidate.Parameters.Count; i++)
            {
                if (candidate.Parameters[i].ParameterType.FullName != candidate.Module.TypeSystem.String.FullName) return false;
            }

            return true;
        }

        public static bool HasUnsupportedCompatibleMethodName(this MethodReference method, TypeReference returnType)
        {
            return method.Name == Naming.ForUnsupportedMethodWithReturnType(returnType);
        }

        public static IEnumerable<MethodDefinition> GetAllMethods(this TypeDefinition self)
        {
            var ret = self.Methods.AsEnumerable();

            return self.BaseType == null
                ? ret
                : ret.Concat(GetAllMethods(self.BaseType.Resolve()));
        }

        public static TypeReference MakeGenericType(this TypeReference self, params TypeReference[] arguments)
        {
            if (self.GenericParameters.Count != arguments.Length)
                throw new ArgumentException();

            var instance = new GenericInstanceType(self);
            foreach (var argument in arguments)
                instance.GenericArguments.Add(argument);

            return instance;
        }

        public static MethodReference MakeGeneric(this MethodReference self, params TypeReference[] arguments)
        {
            var reference = new MethodReference(self.Name, self.ReturnType)
            {
                DeclaringType = self.DeclaringType.MakeGenericType(arguments),
                HasThis = self.HasThis,
                ExplicitThis = self.ExplicitThis,
                CallingConvention = self.CallingConvention,
            };

            foreach (var parameter in self.Parameters)
                reference.Parameters.Add(new ParameterDefinition(parameter.ParameterType));

            foreach (var generic_parameter in self.GenericParameters)
                reference.GenericParameters.Add(new GenericParameter(generic_parameter.Name, reference));

            return reference;
        }

        public static TypeReference UnWrap(this TypeReference type)
        {
            var typeWithElement = type as TypeSpecification;
            if (typeWithElement != null) return typeWithElement.ElementType;

            return type;
        }

        public static TypeReference Clone(this TypeReference typeReference)
        {
            var newTypeReference = new TypeReference(typeReference.Namespace, typeReference.Name, typeReference.Module, typeReference.Scope, typeReference.IsValueType);
            if (typeReference.IsByReference)
                newTypeReference = newTypeReference.MakeByReferenceType();

            if (typeReference.IsArray)
                newTypeReference = newTypeReference.MakeArrayType();

            return newTypeReference;
        }

        private static int ActualParameterCount(MethodReference original)
        {
            return original.Parameters.Count + (original.Resolve().IsStatic ? 0 : 1);
        }
    }
}
