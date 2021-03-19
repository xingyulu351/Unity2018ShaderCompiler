using System;
using System.Runtime.CompilerServices;

[assembly: InternalsVisibleTo("Unity.BindingsGenerator.TestDependentAssembly")]

namespace Unity.BindingsGenerator.TestDependencyAssembly
{
    class VisibleToOtherModulesAttribute : Attribute
    {
        public VisibleToOtherModulesAttribute()
        {
        }

        public VisibleToOtherModulesAttribute(params string[] modules)
        {
        }
    }

    public class PublicClass
    {
        public void PublicMethod() {}
        internal void InternalMethod() {}
        [VisibleToOtherModules]
        internal void InternalMethodWithAttribute() {}
        [VisibleToOtherModules("WrongModule")]
        internal void InternalMethodWithAttributeSpecifyingWrongModule() {}
        [VisibleToOtherModules("WrongModule", "Unity.BindingsGenerator.TestDependentAssembly")]
        internal void InternalMethodWithAttributeSpecifyingCorrectModule() {}

        public int PublicField;
        internal int InternalField;
        [VisibleToOtherModules]
        internal int InternalFieldWithAttribute;
        [VisibleToOtherModules("WrongModule")]
        internal int InternalFieldWithAttributeSpecifyingWrongModule;
        [VisibleToOtherModules("WrongModule", "Unity.BindingsGenerator.TestDependentAssembly")]
        internal int InternalFieldWithAttributeSpecifyingCorrectModule;

        public int PublicProperty { get; set; }
        public int PublicPropertyWithInternalSetter { get; internal set; }
        [VisibleToOtherModules]
        public int PublicPropertyWithInternalSetterWithAttributeOnProperty { get; internal set; }
        public int PublicPropertyWithInternalSetterWithAttributeOnSetter { get; [VisibleToOtherModules] internal set; }

        internal int InternalProperty { get; set; }
        [VisibleToOtherModules]
        internal int InternalPropertyWithAttributeOnProperty { get; set; }
        internal int InternalPropertyWithWithAttributeOnSetter { get; [VisibleToOtherModules]  set; }

        public class PublicNestedClass {}
        internal class InternalNestedClass {}
        [VisibleToOtherModules]
        internal class InternalNestedClassWithAttribute {}
    }

    internal class InternalClass
    {
        public class PublicNestedClass {}
        internal class InternalNestedClass {}
        [VisibleToOtherModules]
        internal class InternalNestedClassWithAttribute {}
    }

    internal class InternalClass2 {}
    internal class InternalClass3 {}
    internal class InternalClass4 {}

    [VisibleToOtherModules]
    internal class InternalClassWithAttribute
    {
        public void PublicMethod() {}
        internal void InternalMethod() {}
        [VisibleToOtherModules]
        internal void InternalMethodWithAttribute() {}

        public int PublicField;
        internal int InternalField;
        [VisibleToOtherModules]
        internal int InternalFieldWithAttribute;

        public class PublicNestedClass {}
        internal class InternalNestedClass {}
        [VisibleToOtherModules]
        internal class InternalNestedClassWithAttribute {}
    }

    [VisibleToOtherModules("WrongModule")]
    internal class InternalClassWithAttributeSpecifyingWrongModule
    {
    }

    [VisibleToOtherModules("WrongModule", "Unity.BindingsGenerator.TestDependentAssembly")]
    internal class InternalClassWithAttributeSpecifyingCorrectModule
    {
    }

    public enum PublicEnum
    {
    }

    internal enum InternalEnum
    {
    }

    [VisibleToOtherModules]
    internal enum InternalEnumWithAttribute
    {
    }

    public interface PublicInterface
    {
    }

    internal interface InternalInterface
    {
    }

    [VisibleToOtherModules]
    internal interface InternalInterfaceWithAttribute
    {
    }
}
