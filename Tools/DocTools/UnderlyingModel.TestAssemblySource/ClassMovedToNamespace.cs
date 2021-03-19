using System;

#pragma warning disable 0618 // Type or member is obsolete

[Obsolete("ClassMovedToNamespace is moved to NamespaceA", false)]
public class ClassMovedToNamespace
{
}

namespace NamespaceA
{
    public class ClassMovedToNamespace
    {
        public void Foo()
        {
        }
    }

    public class ClassWithFieldReferencingMovedClass
    {
        public ClassMovedToNamespace fieldA;
    }

    //this class references [[ClassMovedToNamespace.Foo]]
    public class ClassReferencingMovedClassFunction
    {
    }
}

//this class references [[ClassMovedToNamespace.Foo]]
public class ClassWithFunctionReferencingMovedClass
{
    public void FunctionWithMovedClassArgument(ClassMovedToNamespace movedClassInstance)
    {
    }
}
#pragma warning restore 0618 // Type or member is obsolete
