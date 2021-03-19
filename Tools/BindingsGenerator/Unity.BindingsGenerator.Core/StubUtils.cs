using System.Collections.Generic;
using Mono.Cecil;
using UnityEngine.Bindings;
using Unity.BindingsGenerator.Core.Nodes;

namespace Unity.BindingsGenerator.Core
{
    public class StubUtils
    {
        internal static string DefaultNativeReturnValue(TypeReference type)
        {
            if (type.Resolve().IsEnum)
                return string.Format("({0})0", TypeUtils.RegisteredNameFor(type));

            switch (type.MetadataType)
            {
                case MetadataType.Boolean:
                    return "false";
                case MetadataType.Char:
                case MetadataType.Byte:
                case MetadataType.SByte:
                case MetadataType.Int16:
                case MetadataType.UInt16:
                case MetadataType.Int32:
                case MetadataType.UInt32:
                case MetadataType.Int64:
                case MetadataType.UInt64:
                    return "0";
                case MetadataType.Single:
                    return "0.0f";
                case MetadataType.Double:
                    return "0.0";
                case MetadataType.IntPtr:
                    return "NULL";
                case MetadataType.String:
                    return "scripting_string_new(\"\")";
            }

            if (TypeUtils.CouldBeNull(type))
                return Naming.ScriptingNull;

            return null;
        }

        internal static NativeConditionalAttribute NativeConditionalAttributeFor(MethodDefinition method)
        {
            NativeConditionalAttribute methodConditionalAttribute;
            NativeConditionalAttribute propertyConditionalAttribute = null;
            NativeConditionalAttribute classConditionalAttribute;

            var declaringProperty = method.GetMatchingPropertyDefinition();

            var hasMethodNativeConditional = method.TryGetAttributeInstance(out methodConditionalAttribute);
            var hasPropertyNativeConditional = declaringProperty != null &&
                declaringProperty.TryGetAttributeInstance(out propertyConditionalAttribute);
            var hasClassNativeConditional = method.DeclaringType.TryGetAttributeInstance(out classConditionalAttribute);

            // Stub disabled on method
            if (hasMethodNativeConditional && !methodConditionalAttribute.Enabled)
                return null;

            // Stub set on method
            if (hasMethodNativeConditional && methodConditionalAttribute.Enabled && methodConditionalAttribute.Condition != null)
                return methodConditionalAttribute;

            // Stub disabled on property
            if (hasPropertyNativeConditional && !propertyConditionalAttribute.Enabled)
                return null;

            // Stub set on property
            if (hasPropertyNativeConditional && propertyConditionalAttribute.Enabled &&
                propertyConditionalAttribute.Condition != null)
                return propertyConditionalAttribute;

            // Stub set on class
            if (hasClassNativeConditional && classConditionalAttribute.Enabled && classConditionalAttribute.Condition != null)
                return classConditionalAttribute;

            // Enabled method inherits from disabled class
            if (hasMethodNativeConditional && hasClassNativeConditional && methodConditionalAttribute.Enabled &&
                classConditionalAttribute.Condition != null)
                return classConditionalAttribute;

            return null;
        }

        public static void AddStubIfApplicable(MethodDefinition methodDefinition, FunctionNode functionNode,
            bool mustReturnStructAsOutParameter)
        {
            var conditionalAttribute = NativeConditionalAttributeFor(methodDefinition);

            if (conditionalAttribute != null)
            {
                IStatementNode statementForStub;
                if (methodDefinition.ReturnType.MetadataType == MetadataType.Void)
                    statementForStub = null;
                else if (mustReturnStructAsOutParameter)
                {
                    statementForStub = new ExpressionStatementNode(
                        new AssignNode(
                            new StringExpressionNode(Naming.ReturnVar),
                            new StringExpressionNode(conditionalAttribute.StubReturnStatement ??
                                DefaultNativeReturnValue(methodDefinition.ReturnType))));
                }
                else
                {
                    statementForStub = new ReturnStatementNode
                    {
                        Expression = new StringExpressionNode
                        {
                            Str = conditionalAttribute.StubReturnStatement ??
                                DefaultNativeReturnValue(methodDefinition.ReturnType)
                        }
                    };
                }
                var stubStatement = new StubStatementNode
                {
                    Condition = new StringExpressionNode {Str = conditionalAttribute.Condition},
                    Statements = functionNode.Statements
                };

                if (statementForStub != null)
                    stubStatement.StatementsForStub.Add(statementForStub);

                functionNode.Statements = new List<IStatementNode> {stubStatement};
            }
        }
    }
}
