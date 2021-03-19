using System.Linq;
using Mono.Cecil;
using Mono.Cecil.Cil;
using NUnit.Framework;

namespace Unity.BindingsGenerator.Tests
{
    static internal class AssemblyPatcherAssertEqual
    {
        static public void AssertEqual(TypeDefinition expected, TypeDefinition actual)
        {
            var expectedMethods = expected.Methods.Where(m => m.IsConstructor == false).OrderBy(m => m.Name).OrderBy(m => m.Parameters.Count).ToList();
            var actualMethods = actual.Methods.Where(m => m.IsConstructor == false).OrderBy(m => m.Name).OrderBy(m => m.Parameters.Count).ToList();

            Assert.AreEqual(expectedMethods.Count(), actualMethods.Count());

            for (int i = 0; i < expectedMethods.Count; i++)
            {
                AssertEqual(expectedMethods[i], actualMethods[i]);
            }
        }

        static public void AssertEqual(MethodDefinition expected, MethodDefinition actual)
        {
            Assert.AreEqual(expected.Name, actual.Name);
            Assert.AreEqual(expected.Parameters.Count, actual.Parameters.Count);
            Assert.AreEqual(expected.CustomAttributes.Count, actual.CustomAttributes.Count);

            Assert.AreEqual(expected.ImplAttributes, actual.ImplAttributes);
            Assert.AreEqual(expected.IsPrivate, actual.IsPrivate);
            Assert.AreEqual(expected.HasBody, actual.HasBody);

            Assert.AreEqual(expected.HasThis, actual.HasThis);
            Assert.AreEqual(expected.ExplicitThis, actual.ExplicitThis);
            Assert.AreEqual(expected.CallingConvention, actual.CallingConvention);

            for (int i = 0; i < expected.CustomAttributes.Count; i++)
            {
                AssertEqual(expected.CustomAttributes[i], actual.CustomAttributes[i]);
            }

            for (int i = 0; i < expected.Parameters.Count; i++)
            {
                AssertEqual(expected.Parameters[i], actual.Parameters[i]);
            }

            if (expected.HasBody)
                AssertEqual(expected.Body, actual.Body);
        }

        public static void AssertEqual(CustomAttribute expected, CustomAttribute actual)
        {
            Assert.AreEqual(expected.AttributeType.Name, actual.AttributeType.Name);
        }

        static public void AssertEqual(ParameterDefinition expected, ParameterDefinition actual)
        {
            Assert.AreEqual(expected.Name, actual.Name);

            // If we are passing the type itself, we will get Expected suffix in expected case
            Assert.AreEqual(expected.ParameterType.FullName.Replace("Expected", ""), actual.ParameterType.FullName);

            Assert.AreEqual(expected.ParameterType.IsByReference, actual.ParameterType.IsByReference);
            Assert.AreEqual(expected.IsOut, actual.IsOut);
            Assert.AreEqual(expected.HasDefault, actual.HasDefault);
            Assert.AreEqual(expected.HasConstant, actual.HasConstant);
            Assert.AreEqual(expected.Constant, actual.Constant);
            Assert.AreEqual(expected.CustomAttributes.Count, actual.CustomAttributes.Count);

            for (int i = 0; i < expected.CustomAttributes.Count; i++)
            {
                AssertEqual(expected.CustomAttributes[i], actual.CustomAttributes[i]);
            }
        }

        static public void AssertEqual(MethodBody expected, MethodBody actual)
        {
            Assert.AreEqual(expected.Instructions.Count, actual.Instructions.Count);
            Assert.AreEqual(expected.InitLocals, actual.InitLocals);
            Assert.AreEqual(expected.HasVariables, actual.HasVariables);
            Assert.AreEqual(expected.Variables.Count, actual.Variables.Count);
            Assert.AreEqual(expected.Instructions.Count, actual.Instructions.Count);

            if (expected.HasVariables)
            {
                for (int i = 0; i < expected.Variables.Count; i++)
                {
                    AssertEqual(expected.Variables[i], actual.Variables[i]);
                }
            }

            for (int i = 0; i < expected.Instructions.Count; i++)
            {
                AssertEqual(expected.Instructions[i], actual.Instructions[i]);
            }
        }

        static public void AssertEqual(VariableDefinition expected, VariableDefinition actual)
        {
            Assert.AreEqual(expected.VariableType.FullName, actual.VariableType.FullName);
        }

        static public void AssertEqual(Instruction expected, Instruction actual)
        {
            Assert.AreEqual(expected.OpCode.ToString(), actual.OpCode.ToString());
            Assert.AreEqual(expected.Operand == null, actual.Operand == null);
            if (expected.Operand != null)
            {
                var expectedOperandString = expected.Operand.ToString().Replace("Expected::", "::");
                expectedOperandString = expected.Operand.ToString().Replace("Expected", "");
                var actualOperandString = actual.Operand.ToString();
                Assert.AreEqual(expectedOperandString, actualOperandString);
            }
        }
    }
}
