using System;

namespace BindingsToCsAndCpp
{
    public class UnbalancedBracesException : Exception
    {
        public UnbalancedBracesException(string content) : base(content)
        {
        }
    }

    public class InvalidMethodSignatureException : Exception
    {
        public string Signature { get; private set; }
        public InvalidMethodSignatureException(string message, string signature)
            : base(message)
        {
            Signature = signature;
        }

        public InvalidMethodSignatureException(string signature)
            : base("The method signature '" + signature + "' is not valid, aborting.")
        {
            Signature = signature;
        }
    }
}
