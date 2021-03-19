namespace UnderlyingModel
{
    public class ObsoleteInfoType
    {
        public ObsoleteInfoType()
        {
            IsObsolete = false;
            IsError = false;
            ObsoleteText = "";
        }

        public ObsoleteInfoType(bool isObsolete, bool isError, string obsoleteText)
        {
            IsObsolete = isObsolete;
            IsError = isError;
            ObsoleteText = obsoleteText;
        }

        public bool IsObsolete { get; private set; }
        public bool IsError { get; private set; }
        public string ObsoleteText { get; private set; }
    }
}
