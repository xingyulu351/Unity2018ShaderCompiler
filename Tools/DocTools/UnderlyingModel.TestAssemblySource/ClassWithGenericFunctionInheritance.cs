using System.Security.Cryptography.X509Certificates;

public class GenericClassWithFunction2<T>
{
    public void Foo()
    {
    }
}

public class ConcreteClass : GenericClassWithFunction2<float>
{
}
