using System;
using System.Collections.Generic;
using System.Runtime.CompilerServices;
using NUnit.Framework;
using Unity.BindingsGenerator.Core;
using UnityEngine.Bindings;
using Unity.BindingsGenerator.Core.Nodes;
using Unity.BindingsGenerator.TestFramework;

namespace Unity.BindingsGenerator.Tests
{
    [TestFixture]
    internal class PropertyTests : BindingsGeneratorTestsBase
    {
        #pragma warning disable 169

        private class AutoProp : UnityEngine.Object
        {
            public extern int Prop
            {
                [MethodImpl(MethodImplOptions.InternalCall)]
                get;

                [MethodImpl(MethodImplOptions.InternalCall)]
                set;
            }
        }

        private class AutoPropGetOnly : UnityEngine.Object
        {
            public extern int Prop
            {
                [MethodImpl(MethodImplOptions.InternalCall)]
                get;
            }
        }

        private class AutoPropSetOnly : UnityEngine.Object
        {
            public extern int Prop
            {
                [MethodImpl(MethodImplOptions.InternalCall)]
                set;
            }
        }

        private class NameOverride1 : UnityEngine.Object
        {
            [NativeMethod(Name = "Foo")]
            public extern int Prop
            {
                [MethodImpl(MethodImplOptions.InternalCall)]
                get;

                [MethodImpl(MethodImplOptions.InternalCall)]
                set;
            }
        }

        private class NameOverride2 : UnityEngine.Object
        {
            [NativeMethod(Name = "Foo")]
            public extern int Prop
            {
                [MethodImpl(MethodImplOptions.InternalCall)]
                get;

                [MethodImpl(MethodImplOptions.InternalCall)]
                [NativeMethod(Name = "SetBar")]
                set;
            }
        }

        private class NameOverride3 : UnityEngine.Object
        {
            public extern int Prop
            {
                [MethodImpl(MethodImplOptions.InternalCall)]
                [NativeMethod(Name = "Foo")]
                get;

                [MethodImpl(MethodImplOptions.InternalCall)]
                [NativeMethod(Name = "Bar")]
                set;
            }
        }

        private class ThreadSafe : UnityEngine.Object
        {
            [NativeMethod(IsThreadSafe = true)]
            public extern int Prop
            {
                [MethodImpl(MethodImplOptions.InternalCall)]
                get;

                [MethodImpl(MethodImplOptions.InternalCall)]
                set;
            }
        }

        private class StaticProperty
        {
            public static extern int Prop
            {
                [MethodImpl(MethodImplOptions.InternalCall)]
                get;

                [MethodImpl(MethodImplOptions.InternalCall)]
                set;
            }
        }

        private class ArrayProperty
        {
            public extern int[] Prop
            {
                [MethodImpl(MethodImplOptions.InternalCall)]
                set;
            }
        }

        private struct NonPodStruct
        {
            string i;
        }

        private class StructProperty
        {
            public extern NonPodStruct Prop
            {
                [MethodImpl(MethodImplOptions.InternalCall)]
                get;
            }
        }

        private class FieldTargetMemberProperty
        {
            [NativeProperty("mProp", false, TargetType.Field)]
            public extern int Prop
            {
                [MethodImpl(MethodImplOptions.InternalCall)]
                get;

                [MethodImpl(MethodImplOptions.InternalCall)]
                set;
            }
        }

        private class FieldTargetStaticProperty
        {
            [NativeProperty(TargetType = TargetType.Field)]
            public static extern int Prop
            {
                [MethodImpl(MethodImplOptions.InternalCall)]
                get;

                [MethodImpl(MethodImplOptions.InternalCall)]
                set;
            }
        }

        private class NonBlittableFieldTargetStaticProperty
        {
            [NativeProperty(TargetType = TargetType.Field)]
            public static extern string Prop
            {
                [MethodImpl(MethodImplOptions.InternalCall)]
                get;

                [MethodImpl(MethodImplOptions.InternalCall)]
                set;
            }
        }

        [StaticAccessor("ClassGetStaticAccessor()", StaticAccessorType.Arrow)]
        private class FieldTargetPropertyStaticAccessorOnClass
        {
            [NativeProperty("mProp", false, TargetType.Field)]
            public static extern int Prop
            {
                [MethodImpl(MethodImplOptions.InternalCall)]
                get;

                [MethodImpl(MethodImplOptions.InternalCall)]
                set;
            }
        }

        [StaticAccessor("ClassGetStaticAccessor()", StaticAccessorType.ArrowWithDefaultReturnIfNull)]
        private class FieldTargetPropertyStaticAccessorOnClassWithNullCheck
        {
            [NativeProperty("mProp", false, TargetType.Field)]
            public static extern int Prop
            {
                [MethodImpl(MethodImplOptions.InternalCall)]
                get;

                [MethodImpl(MethodImplOptions.InternalCall)]
                set;
            }
        }

        private class FieldTargetPropertyStaticAccessorOnProperty
        {
            [NativeProperty("mProp", false, TargetType.Field)]
            [StaticAccessor("ClassGetStaticAccessor()", StaticAccessorType.Arrow)]
            public static extern int Prop
            {
                [MethodImpl(MethodImplOptions.InternalCall)]
                get;

                [MethodImpl(MethodImplOptions.InternalCall)]
                set;
            }
        }

        private class FieldTargetPropertyStaticAccessorOnPropertyWithNullCheck
        {
            [NativeProperty("mProp", false, TargetType.Field)]
            [StaticAccessor("ClassGetStaticAccessor()", StaticAccessorType.ArrowWithDefaultReturnIfNull)]
            public static extern int Prop
            {
                [MethodImpl(MethodImplOptions.InternalCall)]
                get;

                [MethodImpl(MethodImplOptions.InternalCall)]
                set;
            }
        }

        private class FieldTargetPropertyStaticAccessorOnMethod
        {
            [NativeProperty("mProp", false, TargetType.Field)]
            public static extern int Prop
            {
                [MethodImpl(MethodImplOptions.InternalCall)]
                [StaticAccessor("ClassGetStaticAccessor()", StaticAccessorType.Arrow)]
                get;

                [MethodImpl(MethodImplOptions.InternalCall)]
                set;
            }
        }

        private class FieldTargetPropertyStaticAccessorOnMethodWithNullCheck
        {
            [NativeProperty("mProp", false, TargetType.Field)]
            public static extern int Prop
            {
                [MethodImpl(MethodImplOptions.InternalCall)]
                [StaticAccessor("ClassGetStaticAccessor()", StaticAccessorType.ArrowWithDefaultReturnIfNull)]
                get;

                [MethodImpl(MethodImplOptions.InternalCall)]
                set;
            }
        }

        [StaticAccessor("ClassGetStaticAccessor()", StaticAccessorType.Arrow)]
        private struct FieldTargetMemberPropertyOnBlittableStruct
        {
            [NativeProperty("mProp", false, TargetType.Field)]
            public extern int Prop
            {
                [MethodImpl(MethodImplOptions.InternalCall)]
                get;

                [MethodImpl(MethodImplOptions.InternalCall)]
                set;
            }
        }

        [StaticAccessor("ClassGetStaticAccessor()", StaticAccessorType.ArrowWithDefaultReturnIfNull)]
        private struct FieldTargetMemberPropertyOnBlittableStructWithNullCheck
        {
            [NativeProperty("mProp", false, TargetType.Field)]
            public extern int Prop
            {
                [MethodImpl(MethodImplOptions.InternalCall)]
                get;

                [MethodImpl(MethodImplOptions.InternalCall)]
                set;
            }
        }

        [StaticAccessor("ClassGetStaticAccessor()", StaticAccessorType.Arrow)]
        private struct FieldTargetPropertyWithStaticAccessorOnBlittableStruct
        {
            [NativeProperty("mProp", false, TargetType.Field)]
            public static extern int Prop
            {
                [MethodImpl(MethodImplOptions.InternalCall)]
                get;

                [MethodImpl(MethodImplOptions.InternalCall)]
                set;
            }
        }

        [StaticAccessor("ClassGetStaticAccessor()", StaticAccessorType.ArrowWithDefaultReturnIfNull)]
        private struct FieldTargetPropertyWithStaticAccessorOnBlittableStructWithNullCheck
        {
            [NativeProperty("mProp", false, TargetType.Field)]
            public static extern int Prop
            {
                [MethodImpl(MethodImplOptions.InternalCall)]
                get;

                [MethodImpl(MethodImplOptions.InternalCall)]
                set;
            }
        }

        private struct GlobalVariableTargetProperty
        {
            [NativeProperty("mProp", true, TargetType.Field)]
            public static extern int Prop
            {
                [MethodImpl(MethodImplOptions.InternalCall)]
                get;

                [MethodImpl(MethodImplOptions.InternalCall)]
                set;
            }
        }

        [Test(Description = "Test that no matter what kind of override we use the binding function name remains the same")]
        [TestCase(typeof(AutoProp),                         "Get_Custom_PropProp", true)]
        [TestCase(typeof(AutoProp),                         "Set_Custom_PropProp", true)]
        [TestCase(typeof(AutoPropGetOnly),                  "Get_Custom_PropProp", true)]
        [TestCase(typeof(AutoPropGetOnly),                  "Set_Custom_PropProp", false)]
        [TestCase(typeof(AutoPropSetOnly),                  "Get_Custom_PropProp", false)]
        [TestCase(typeof(AutoPropSetOnly),                  "Set_Custom_PropProp", true)]
        [TestCase(typeof(NameOverride1),                    "Get_Custom_PropProp", true)]
        [TestCase(typeof(NameOverride1),                    "Set_Custom_PropProp", true)]
        [TestCase(typeof(NameOverride2),                    "Get_Custom_PropProp", true)]
        [TestCase(typeof(NameOverride2),                    "Set_Custom_PropProp", true)]
        [TestCase(typeof(NameOverride3),                    "Get_Custom_PropProp", true)]
        [TestCase(typeof(NameOverride3),                    "Set_Custom_PropProp", true)]
        [TestCase(typeof(FieldTargetMemberProperty),         "Get_Custom_PropProp", true)]
        [TestCase(typeof(FieldTargetMemberProperty),         "Set_Custom_PropProp", true)]
        [TestCase(typeof(FieldTargetStaticProperty),         "Get_Custom_PropProp", true)]
        [TestCase(typeof(FieldTargetStaticProperty),         "Set_Custom_PropProp", true)]
        [TestCase(typeof(FieldTargetPropertyStaticAccessorOnClass), "Get_Custom_PropProp", true)]
        [TestCase(typeof(FieldTargetPropertyStaticAccessorOnClass), "Set_Custom_PropProp", true)]
        public void BindingFunctionNameWorks(Type type, string functionName, bool functionExists)
        {
            if (functionExists)
                Assert.IsTrue(SourceFileNodeFor(type).FindFunction(functionName) != null);
            else
                Assert.IsTrue(SourceFileNodeFor(type).FindFunction(functionName) == null);
        }

        [Test(Description = "Test that get and set functions have the right parameters (including type name override)")]
        [TestCase(typeof(AutoProp), "AutoProp")]
        public void ParametersWork(Type type, string nativeTypeName)
        {
            var getExpectation = new FunctionNode()
            {
                ReturnType = "SInt32",
                Parameters =
                {
                    new FunctionParameter {Type = "ICallType_ReadOnlyUnityEngineObject_Argument", Name = Naming.Self, IsSelf = true}
                },
                Statements = null
            };

            var setExpectation = new FunctionNode()
            {
                ReturnType = "void",
                Parameters =
                {
                    new FunctionParameter {Type = "ICallType_ReadOnlyUnityEngineObject_Argument", Name = Naming.Self, IsSelf = true},
                    new FunctionParameter {Type = "SInt32", Name = Naming.PropertyValue}
                },
                Statements = null
            };

            var source = SourceFileNodeFor(type);
            var getFunction = source.FindFunction("Get_Custom_PropProp");
            var setFunction = source.FindFunction("Set_Custom_PropProp");

            AssertEqual(getExpectation, getFunction);
            AssertEqual(setExpectation, setFunction);
        }

        [Test(Description = "Test that we call the right function (including name overrides)")]
        [TestCase(typeof(AutoProp), "GetProp", "SetProp")]
        [TestCase(typeof(NameOverride1), "GetFoo", "SetFoo")]
        [TestCase(typeof(NameOverride2), "GetFoo", "SetBar")]
        [TestCase(typeof(NameOverride3), "Foo", "Bar")]
        public void FunctionCallWorks(Type type, string nativeGet, string nativeSet)
        {
            var getExpectation = new ReturnStatementNode
            {
                Expression = new FunctionCallNode { Qualifier = new InstanceMemberQualifier(), FunctionName = nativeGet, Arguments = new List<FunctionArgument>()}
            };

            var setExpectation = new ExpressionStatementNode
            {
                Expression = new FunctionCallNode { Qualifier = new InstanceMemberQualifier(), FunctionName = nativeSet, Arguments = new List<FunctionArgument> { new FunctionArgument { Name = Naming.PropertyValue}}}
            };

            var source = SourceFileNodeFor(type);
            var getFunction = source.FindFunction("Get_Custom_PropProp");
            var setFunction = source.FindFunction("Set_Custom_PropProp");

            Assert.IsTrue(FunctionContainsStatement(getExpectation, getFunction));
            Assert.IsTrue(FunctionContainsStatement(setExpectation, setFunction));
        }

        [Test(Description = "Test that etw entry macro is emitted")]
        public void EtwEntryWorks()
        {
            var getExpectation = new StringStatementNode {Str = string.Format("{0}(PropertyTests_AutoProp_Get_Custom_PropProp)", Naming.EtwEntryMacro)};
            var setExpectation = new StringStatementNode {Str = string.Format("{0}(PropertyTests_AutoProp_Set_Custom_PropProp)", Naming.EtwEntryMacro)};

            var source = SourceFileNodeFor<AutoProp>();
            var getFunction = source.FindFunction("Get_Custom_PropProp");
            var setFunction = source.FindFunction("Set_Custom_PropProp");

            Assert.IsTrue(FunctionContainsStatement(getExpectation, getFunction));
            Assert.IsTrue(FunctionContainsStatement(setExpectation, setFunction));
        }

        [Test(Description = "Test that stack check works")]
        public void StackCheckWorks()
        {
            var getExpectation = new StringStatementNode {Str = string.Format("{0}(get_Prop)", Naming.StackCheckMacro)};
            var setExpectation = new StringStatementNode {Str = string.Format("{0}(set_Prop)", Naming.StackCheckMacro)};

            var source = SourceFileNodeFor<AutoProp>();
            var getFunction = source.FindFunction("Get_Custom_PropProp");
            var setFunction = source.FindFunction("Set_Custom_PropProp");

            Assert.IsTrue(FunctionContainsStatement(getExpectation, getFunction));
            Assert.IsTrue(FunctionContainsStatement(setExpectation, setFunction));
        }

        [Test(Description = "Test that thread safe check works")]
        public void ThreadSafeCheckWorks()
        {
            var getExpectation = new StringStatementNode {Str = string.Format("{0}(get_Prop)", Naming.ThreadSafeMacro)};
            var setExpectation = new StringStatementNode {Str = string.Format("{0}(set_Prop)", Naming.ThreadSafeMacro)};

            {
                var source = SourceFileNodeFor<AutoProp>();
                var getFunction = source.FindFunction("Get_Custom_PropProp");
                var setFunction = source.FindFunction("Set_Custom_PropProp");

                Assert.IsTrue(FunctionContainsStatement(getExpectation, getFunction));
                Assert.IsTrue(FunctionContainsStatement(setExpectation, setFunction));
            }

            {
                var source = SourceFileNodeFor<ThreadSafe>();
                var getFunction = source.FindFunction("Get_Custom_PropProp");
                var setFunction = source.FindFunction("Set_Custom_PropProp");

                Assert.IsFalse(FunctionContainsStatement(getExpectation, getFunction));
                Assert.IsFalse(FunctionContainsStatement(setExpectation, setFunction));
            }
        }

        [Test(Description = "Test that static property works")]
        public void StaticPropertyWorks()
        {
            var getExpectation = new ReturnStatementNode
            {
                Expression = new FunctionCallNode
                {
                    Qualifier = new StaticMemberQualifier { ClassName = typeof(StaticProperty).RegisteredName()},
                    FunctionName = "GetProp",
                    Arguments = new List<FunctionArgument>()
                }
            };

            var setExpectation = new ExpressionStatementNode
            {
                Expression = new FunctionCallNode
                {
                    Qualifier = new StaticMemberQualifier { ClassName = typeof(StaticProperty).RegisteredName()},
                    FunctionName = "SetProp",
                    Arguments = new List<FunctionArgument> { new FunctionArgument { Name = Naming.PropertyValue}}
                }
            };

            var source = SourceFileNodeFor<StaticProperty>();
            var getFunction = source.FindFunction("Get_Custom_PropProp");
            var setFunction = source.FindFunction("Set_Custom_PropProp");

            Assert.IsTrue(FunctionContainsStatement(getExpectation, getFunction));
            Assert.IsTrue(FunctionContainsStatement(setExpectation, setFunction));
        }

        [Test(Description = "Test that array properties are marshalled correctly")]
        [TestCase(typeof(ArrayProperty), "Marshalling::ArrayMarshaller<SInt32>")]
        public void ArrayPropertyWorks(Type type, string expectedType)
        {
            var setFunction = FunctionNodeForMethod(type, "set_Prop");

            var declarationExpectation = new VariableDeclarationStatementNode
            {
                Name = Naming.PropertyValue.Marshalled(),
                Type = expectedType,
            };
            Assert.IsTrue(FunctionContainsStatement(declarationExpectation, setFunction));
        }

        [Test(Description = "Test that struct properties are converted to out refs correctly")]
        [TestCase(typeof(StructProperty), "NonPodStruct__", "ICallType_Generated_Unity_BindingsGenerator_Tests_PropertyTests_NonPodStruct")]
        public void StructPropertyWorks(Type type, string parameterType, string marshalledType)
        {
            var source = SourceFileNodeFor(type);
            var functionNode = source.FindFunction("CUSTOM_get_Prop_Injected");

            var parameterExpectation = new List<FunctionParameter>
            {
                null, // don't care about self parameter
                new FunctionParameter {Type = Naming.Reference(parameterType), IsOutReturn = true}
            };
            AssertEqual(parameterExpectation, functionNode.Parameters);

            var declarationExpectation = new VariableDeclarationStatementNode
            {
                Name = Naming.ReturnVar.Marshalled(),
                Type = marshalledType,
            };
            Assert.IsTrue(FunctionContainsStatement(declarationExpectation, functionNode));

            var marshalExpectation = new ExpressionStatementNode
            {
                Expression = new FunctionCallNode
                {
                    Qualifier = new StaticMemberQualifier
                    {
                        ClassName = "Marshalling"
                    },
                    FunctionName = "Unmarshal",
                    Arguments =
                    {
                        new FunctionArgument {Name = Naming.AddressOf(Naming.ReturnVar)},
                        new FunctionArgument {Name = Naming.AddressOf(Naming.ReturnVar.Marshalled())},
                    }
                }
            };
            Assert.IsTrue(FunctionContainsStatement(marshalExpectation, functionNode));
        }

        [Test(Description = "Test that properties targeting member fields generate correct field reference")]
        [TestCase(typeof(FieldTargetMemberProperty), true)]
        [TestCase(typeof(FieldTargetMemberPropertyOnBlittableStruct), false)]
        public void FieldTargetMemberPropertyReferencesCorrectField(Type type, bool isSelfMarshalled)
        {
            var setFunction = FunctionNodeForMethod(type, "get_Prop");

            var declarationExpectation = new ReturnStatementNode
            {
                Expression = new FieldReferenceNode
                {
                    Qualifier = new InstanceMemberQualifier { Expression = isSelfMarshalled ? Naming.Self.Marshalled() : Naming.Self },
                    Name = "mProp"
                }
            };
            Assert.IsTrue(FunctionContainsStatement(declarationExpectation, setFunction));
        }

        [Test(Description = "Test that properties targeting member fields generate correct null check")]
        [TestCase(typeof(FieldTargetMemberPropertyOnBlittableStructWithNullCheck), false)]
        public void FieldTargetMemberPropertyReferencesCorrectFieldWithNullCheck(Type type, bool isSelfMarshalled)
        {
            var setFunction = FunctionNodeForMethod(type, "get_Prop");

            var nullCheck = new IfStatementNode()
            {
                Condition = new StringExpressionNode { Str = "ClassGetStaticAccessor() == NULL" },
                Statements = { new ReturnStatementNode { Expression = new StringExpressionNode { Str = "0" } } }
            };
            Assert.IsTrue(FunctionContainsStatement(nullCheck, setFunction));
        }

        [Test(Description = "Test that properties targeting to static fields generate correct field reference")]
        public void FieldTargetStaticPropertyReferencesCorrectField()
        {
            var setFunction = FunctionNodeForMethod(typeof(FieldTargetStaticProperty), "get_Prop");

            var declarationExpectation = new ReturnStatementNode
            {
                Expression = new FieldReferenceNode
                {
                    Qualifier = new StaticMemberQualifier { ClassName = typeof(FieldTargetStaticProperty).RegisteredName() },
                    Name = "Prop"
                }
            };
            Assert.IsTrue(FunctionContainsStatement(declarationExpectation, setFunction));
        }

        [Test(Description = "Test that properties targeting to static fields generate correct field reference")]
        [TestCase(typeof(FieldTargetStaticProperty), "_unity_value")]
        [TestCase(typeof(NonBlittableFieldTargetStaticProperty), "_unity_value_marshalled")]
        public void FieldTargetStaticPropertySetAssignsCorrectField(Type typeContainingProp, string variableToAssign)
        {
            var setFunction = FunctionNodeForMethod(typeContainingProp, "set_Prop");

            var declarationExpectation = new ExpressionStatementNode
            {
                Expression = new AssignNode
                {
                    Lhs = new FieldReferenceNode
                    {
                        Qualifier = new StaticMemberQualifier { ClassName = typeContainingProp.RegisteredName() },
                        Name = "Prop"
                    },
                    Rhs = new StringExpressionNode { Str = variableToAssign }
                }
            };
            Assert.IsTrue(FunctionContainsStatement(declarationExpectation, setFunction));
        }

        [Test(Description = "Test that properties targeting to fields with a static accessor generate correct field reference")]
        [TestCase(typeof(FieldTargetPropertyStaticAccessorOnClass))]
        [TestCase(typeof(FieldTargetPropertyStaticAccessorOnMethod))]
        [TestCase(typeof(FieldTargetPropertyStaticAccessorOnProperty))]
        [TestCase(typeof(FieldTargetPropertyWithStaticAccessorOnBlittableStruct))]
        public void FieldTargetStaticAccessorPropertyGetReturnsCorrectField(Type type)
        {
            var setFunction = FunctionNodeForMethod(type, "get_Prop");

            var declarationExpectation = new ReturnStatementNode
            {
                Expression = new FieldReferenceNode
                {
                    Qualifier = new InstanceMemberQualifier
                    {
                        Expression = "ClassGetStaticAccessor()",
                        IsReference = false
                    },
                    Name = "mProp"
                }
            };
            Assert.IsTrue(FunctionContainsStatement(declarationExpectation, setFunction));
        }

        [Test(Description = "Test that properties targeting to fields with a static accessor generate null check")]
        [TestCase(typeof(FieldTargetPropertyStaticAccessorOnClassWithNullCheck))]
        [TestCase(typeof(FieldTargetPropertyStaticAccessorOnMethodWithNullCheck))]
        [TestCase(typeof(FieldTargetPropertyStaticAccessorOnPropertyWithNullCheck))]
        [TestCase(typeof(FieldTargetPropertyWithStaticAccessorOnBlittableStructWithNullCheck))]
        public void FieldTargetStaticAccessorPropertyGetReturnsCorrectFieldWithNullCheck(Type type)
        {
            var setFunction = FunctionNodeForMethod(type, "get_Prop");

            var nullCheck = new IfStatementNode()
            {
                Condition = new StringExpressionNode { Str = "ClassGetStaticAccessor() == NULL" },
                Statements = { new ReturnStatementNode { Expression = new StringExpressionNode { Str = "0" } } }
            };
            Assert.IsTrue(FunctionContainsStatement(nullCheck, setFunction));
        }

        private class FieldTargetPropertyStaticAccessorAndNativeNameOnMethod
        {
            [NativeProperty("mProp", false, TargetType.Field)]
            [StaticAccessor("StaticAccessor()", StaticAccessorType.Arrow)]
            public static extern int Prop
            {
                [MethodImpl(MethodImplOptions.InternalCall)]
                [NativeName("getterName")]
                get;

                [MethodImpl(MethodImplOptions.InternalCall)]
                [NativeName("setterName")]
                set;
            }
        }

        private class FieldTargetPropertyStaticAccessorAndNativeNameOnMethodWithNullCheck
        {
            [NativeProperty("mProp", false, TargetType.Field)]
            [StaticAccessor("StaticAccessor()", StaticAccessorType.ArrowWithDefaultReturnIfNull)]
            public static extern int Prop
            {
                [MethodImpl(MethodImplOptions.InternalCall)]
                [NativeName("getterName")]
                get;

                [MethodImpl(MethodImplOptions.InternalCall)]
                [NativeName("setterName")]
                set;
            }
        }

        [Test]
        public void FieldTargetPropertyStaticAccessorAndNativeNameOnMethod_GetReferencesCorrectFields()
        {
            var getFunction = FunctionNodeForMethod(typeof(FieldTargetPropertyStaticAccessorAndNativeNameOnMethod), "get_Prop");

            var getDeclarationExpectation = new ReturnStatementNode
            {
                Expression = new FieldReferenceNode
                {
                    Qualifier = new InstanceMemberQualifier
                    {
                        Expression = "StaticAccessor()",
                        IsReference = false
                    },
                    Name = "getterName"
                }
            };
            Assert.IsTrue(FunctionContainsStatement(getDeclarationExpectation, getFunction));
        }

        [Test]
        public void FieldTargetPropertyStaticAccessorAndNativeNameOnMethodWithNullCheck_GetReferencesCorrectFields()
        {
            var getFunction = FunctionNodeForMethod(typeof(FieldTargetPropertyStaticAccessorAndNativeNameOnMethodWithNullCheck), "get_Prop");

            var nullCheck = new IfStatementNode()
            {
                Condition = new StringExpressionNode { Str = "StaticAccessor() == NULL" },
                Statements = { new ReturnStatementNode { Expression = new StringExpressionNode { Str = "0" } } }
            };
            Assert.IsTrue(FunctionContainsStatement(nullCheck, getFunction));
        }

        [Test]
        public void FieldTargetPropertyStaticAccessorAndNativeNameOnMethod_SetReferencesCorrectFields()
        {
            var setFunction = FunctionNodeForMethod(typeof(FieldTargetPropertyStaticAccessorAndNativeNameOnMethod), "set_Prop");

            var declarationExpectation = new ExpressionStatementNode
            {
                Expression = new AssignNode
                {
                    Lhs = new FieldReferenceNode
                    {
                        Qualifier = new InstanceMemberQualifier
                        {
                            Expression = "StaticAccessor()",
                            IsReference = false
                        },
                        Name = "setterName"
                    },
                    Rhs = new StringExpressionNode { Str = "_unity_value" }
                }
            };
            Assert.IsTrue(FunctionContainsStatement(declarationExpectation, setFunction));
        }

        [Test]
        public void FieldTargetPropertyStaticAccessorAndNativeNameOnMethodWithNullCheck_SetReferencesCorrectFields()
        {
            var setFunction = FunctionNodeForMethod(typeof(FieldTargetPropertyStaticAccessorAndNativeNameOnMethodWithNullCheck), "set_Prop");

            var nullCheck = new IfStatementNode()
            {
                Condition = new StringExpressionNode { Str = "StaticAccessor() == NULL" },
                Statements = { new ReturnStatementNode { Expression = new StringExpressionNode { Str = null } } }
            };
            Assert.IsTrue(FunctionContainsStatement(nullCheck, setFunction));
        }

        [Test]
        public void GlobalVariableTargetPropertyGetReturnsCorrectVariable()
        {
            var setFunction = FunctionNodeForMethod(typeof(GlobalVariableTargetProperty), "get_Prop");

            var declarationExpectation = new ReturnStatementNode
            {
                Expression = new FieldReferenceNode
                {
                    Qualifier = new FreeQualifier(),
                    Name = "mProp"
                }
            };
            Assert.IsTrue(FunctionContainsStatement(declarationExpectation, setFunction));
        }
    }
}
