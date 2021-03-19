using System.Runtime.CompilerServices;
using UnityEngine.Bindings;

namespace Unity.BindingsGenerator.AssemblyVisitorTestAssembly
{
    [NativeType(Header = "NativeTypeUsedAsArgumentType")]
    class NativeTypeUsedAsArgumentType : UnityEngine.Object
    {
    }

    [NativeType(Header = "NativeTypeUsedAsReturnType")]
    class NativeTypeUsedAsReturnType : UnityEngine.Object
    {
    }

    [NativeType(Header = "NativeTypeUsedAsGetterType")]
    class NativeTypeUsedAsGetterType : UnityEngine.Object
    {
    }

    [NativeType(Header = "NativeTypeUsedAsSetterType")]
    class NativeTypeUsedAsSetterType : UnityEngine.Object
    {
    }

    [NativeType(Header = "NativeTypeUsedAsPropertyType")]
    class NativeTypeUsedAsPropertyType : UnityEngine.Object
    {
    }

    [NativeType(Header = "NativeTypeUser")]
    class NativeTypeUser : UnityEngine.Object
    {
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void ArgumentUser(NativeTypeUsedAsArgumentType arg);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern NativeTypeUsedAsReturnType ReturnValueUser();

        public extern NativeTypeUsedAsGetterType GetterOnlyPropUser
        {
            [MethodImpl(MethodImplOptions.InternalCall)]
            get;
        }

        public extern NativeTypeUsedAsSetterType SetterOnlyPropUser
        {
            [MethodImpl(MethodImplOptions.InternalCall)]
            set;
        }

        public extern NativeTypeUsedAsPropertyType PropertyUser
        {
            [MethodImpl(MethodImplOptions.InternalCall)]
            get;

            [MethodImpl(MethodImplOptions.InternalCall)]
            set;
        }
    }
}
