using System;
using Unity.BindingsGenerator.TestDependencyAssembly;
namespace Unity.BindingsGenerator.TestDependentAssembly
{
    [TypeAttribute(typeof(InternalClass2))]
    public class MyClass
    {
        public MyClass()
        {
            // Access marked with ✅ should be allowed, ❌ should generate an error.

            /*✅*/ var publicClass = new PublicClass();
            /*❌*/ var internalClass = new InternalClass();
            /*✅*/ var internalClassWithAttribute = new InternalClassWithAttribute();
            /*❌*/ var internalClassWithAttributeSpecifyingWrongModule = new InternalClassWithAttributeSpecifyingWrongModule();
            /*✅*/ var internalClassWithAttributeSpecifyingCorrectModule = new InternalClassWithAttributeSpecifyingCorrectModule();

            /*✅*/ var publicClassPublicNestedClass = new PublicClass.PublicNestedClass();
            /*❌*/ var publicClassInternalNestedClass = new PublicClass.InternalNestedClass();
            /*✅*/ var publicClassInternalNestedClassWithAttribute = new PublicClass.InternalNestedClassWithAttribute();

            /*❌*/ var internalClassPublicNestedClass = new InternalClass.PublicNestedClass();
            /*❌*/ var internalClassInternalNestedClass = new InternalClass.InternalNestedClass();
            /*❌*/ var internalClassInternalNestedClassWithAttribute = new InternalClass.InternalNestedClassWithAttribute();

            /*✅*/ var internalClassWithAttributePublicNestedClass = new InternalClassWithAttribute.PublicNestedClass();
            /*❌*/ var internalClassWithAttributeInternalNestedClass = new InternalClassWithAttribute.InternalNestedClass();
            /*✅*/ var internalClassWithAttributeInternalNestedClassWithAttribute = new InternalClassWithAttribute.InternalNestedClassWithAttribute();

            /*✅*/ publicClass.PublicMethod();
            /*❌*/ publicClass.InternalMethod();
            /*✅*/ publicClass.InternalMethodWithAttribute();
            /*❌*/ publicClass.InternalMethodWithAttributeSpecifyingWrongModule();
            /*✅*/ publicClass.InternalMethodWithAttributeSpecifyingCorrectModule();

            /*✅*/ publicClass.PublicField = 0;
            /*❌*/ publicClass.InternalField = 0;
            /*✅*/ publicClass.InternalFieldWithAttribute = 0;
            /*❌*/ publicClass.InternalFieldWithAttributeSpecifyingWrongModule = 0;
            /*✅*/ publicClass.InternalFieldWithAttributeSpecifyingCorrectModule = 0;

            /*✅*/ publicClass.PublicPropertyWithInternalSetter = 0;
            /*✅*/ publicClass.PublicPropertyWithInternalSetterWithAttributeOnSetter = 0;
            /*❌*/ publicClass.PublicPropertyWithInternalSetterWithAttributeOnProperty = 0;

            /*❌*/ publicClass.InternalProperty = 0;
            /*✅*/ publicClass.InternalPropertyWithAttributeOnProperty = 0;
            /*✅*/ publicClass.InternalPropertyWithWithAttributeOnSetter = 0;

            /*✅*/ internalClassWithAttribute.PublicMethod();
            /*❌*/ internalClassWithAttribute.InternalMethod();
            /*✅*/ internalClassWithAttribute.InternalMethodWithAttribute();

            /*✅*/ internalClassWithAttribute.PublicField = 0;
            /*❌*/ internalClassWithAttribute.InternalField = 0;
            /*✅*/ internalClassWithAttribute.InternalFieldWithAttribute = 0;

            /*✅*/ PublicEnum publicEnum;
            /*❌*/ InternalEnum internalEnum;
            /*✅*/ InternalEnumWithAttribute internalEnumWithAttribute;

            /*✅*/ PublicInterface publicInterface;
            /*❌*/ InternalInterface internalInterface;
            /*✅*/ InternalInterfaceWithAttribute internalInterfaceWithAttribute;

            /*❌*/ GenericMethod<InternalClass4>();
        }

        [TypeAttribute(typeof(InternalClass3))]
        void GenericMethod<T>() {}
    }

    [AttributeUsage(AttributeTargets.All)]
    public class TypeAttribute : Attribute
    {
        public TypeAttribute(Type type) {}
    }
}
