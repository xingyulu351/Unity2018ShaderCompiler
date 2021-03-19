public interface InterfaceAA
{
}

public interface InterfaceA : InterfaceAA
{
}

namespace NamespaceI
{
    public interface InterfaceB
    {
    }
}

public interface InterfaceC
{
}

public class AnotherClass : InterfaceC
{
}

public class ClassImplementingInterfaces : AnotherClass, InterfaceA, NamespaceI.InterfaceB
{
}
