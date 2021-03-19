//~ NestedTypes/L2 -> TopLevel
public class NestedTypes
{
    public string Run()
    {
        //:< NestedTypes+L2|NestedTypes+L2::Method()
        //:> TopLevel|TopLevel::Method()
        return new L1().Method() + "|" + new L2().Method();
    }

    public class L2 : TopLevel
    {
        public override string Method()
        {
            return "NestedTypes+L2::Method()";
        }
    }

    public class L1 : L2
    {
        public override string Method()
        {
            return GetType().BaseType.FullName;
        }
    }
}

public class TopLevel
{
    public virtual string Method()
    {
        return "TopLevel::Method()";
    }
}
