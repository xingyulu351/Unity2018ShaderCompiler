//~ Base -> NewBase
public class BaseTypeRetargeting : Base
{
    public override string VirtualMethod()
    {
        return "BaseTypeRetargeting::VirtualMethod()";
    }

    public string Run()
    {
        //:< BaseTypeRetargeting/Base::Method()/BaseTypeRetargeting::VirtualMethod()
        //:> BaseTypeRetargeting/NewBase::Method()/BaseTypeRetargeting::VirtualMethod()
        return GetType().Name + "/" + Method() + "/" + VirtualMethod();
    }
}

public class Base
{
    protected string Method()
    {
        return "Base::Method()";
    }

    public virtual string VirtualMethod()
    {
        return "Base::VirtualMethod()";
    }
}

public class NewBase
{
    protected string Method()
    {
        return "NewBase::Method()";
    }

    public virtual string VirtualMethod()
    {
        return "NewBase::VirtualMethod()";
    }
}
