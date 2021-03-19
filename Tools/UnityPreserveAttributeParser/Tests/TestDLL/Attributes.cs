using System;

namespace UnityEngineTestDLL
{
    [AttributeUsage(AttributeTargets.Method | AttributeTargets.Class | AttributeTargets.Field | AttributeTargets.Struct | AttributeTargets.Property | AttributeTargets.Constructor | AttributeTargets.Interface | AttributeTargets.Enum, Inherited = false)]
    internal class UsedByNativeCodeAttribute : Attribute
    {
        public UsedByNativeCodeAttribute()
        {
        }

        public UsedByNativeCodeAttribute(string name)
        {
            Name = name;
        }

        public string Name { get; set; }
    }

    [AttributeUsage(AttributeTargets.Method | AttributeTargets.Class | AttributeTargets.Field | AttributeTargets.Struct | AttributeTargets.Property | AttributeTargets.Constructor | AttributeTargets.Interface | AttributeTargets.Enum, Inherited = false)]
    internal class RequiredByNativeCodeAttribute : Attribute
    {
        public RequiredByNativeCodeAttribute()
        {
        }

        public RequiredByNativeCodeAttribute(string name)
        {
            Name = name;
        }

        public RequiredByNativeCodeAttribute(bool optional)
        {
            Optional = optional;
        }

        public RequiredByNativeCodeAttribute(string name, bool optional)
        {
            Name = name;
            Optional = optional;
        }

        public string Name { get; set; }
        public bool Optional { get; set; }
        public bool GenerateProxy { get; set; }
    }

    [AttributeUsage(AttributeTargets.Field, Inherited = false)]
    internal class NativeNameAttribute : Attribute
    {
        public NativeNameAttribute(string nativeName)
        {
            NativeName = nativeName;
        }

        public string NativeName { get; set; }
    }


    [AttributeUsage(AttributeTargets.Class | AttributeTargets.Struct, Inherited = false)]
    internal class NativeHeaderAttribute : Attribute
    {
        public NativeHeaderAttribute(string nativeHeader)
        {
            NativeHeader = nativeHeader;
        }

        public string NativeHeader { get; set; }
    }

    [AttributeUsage(AttributeTargets.Class | AttributeTargets.Struct, Inherited = false)]
    public sealed class NativeClassAttribute : Attribute
    {
        public string QualifiedNativeName { get; private set; }
        public string Declaration { get; private set; }

        public NativeClassAttribute(string qualifiedCppName)
        {
            QualifiedNativeName = qualifiedCppName;
            Declaration = "class " + qualifiedCppName;
        }

        public NativeClassAttribute(string qualifiedCppName, string declaration)
        {
            QualifiedNativeName = qualifiedCppName;
            Declaration = declaration;
        }
    }
}
