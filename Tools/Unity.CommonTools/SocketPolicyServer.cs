namespace Unity.CommonTools
{
    public class SocketPolicyServer : SocketResponder
    {
        public static readonly string Allow = @"
<?xml version=""1.0""?>
<cross-domain-policy>
   <allow-access-from domain=""*"" />
</cross-domain-policy>";

        public new static readonly int Port = 843;

        public SocketPolicyServer(string policy, int port) : base(port, "<policy-file-request/>", policy)
        {
        }

        public SocketPolicyServer(string policy) : this(policy, Port)
        {
        }
    }
}
