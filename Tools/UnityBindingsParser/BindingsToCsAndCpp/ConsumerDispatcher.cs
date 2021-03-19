using UnityBindingsParser;

namespace BindingsToCsAndCpp
{
    internal class ConsumerDispatcher : IBindingsConsumer
    {
        readonly IBindingsConsumer[] _consumers;

        public ConsumerDispatcher(IBindingsConsumer[] consumers)
        {
            _consumers = consumers;
        }

        public void OnCppRaw(string content)
        {
            foreach (var c in _consumers)
                c.OnCppRaw(content);
        }

        public void OnCppRaw(string[] content)
        {
            foreach (var c in _consumers)
                c.OnCppRaw(content);
        }

        public void OnCsRaw(string content)
        {
            foreach (var c in _consumers)
                c.OnCsRaw(content);
        }

        public void OnCsRaw(string[] content)
        {
            foreach (var c in _consumers)
                c.OnCsRaw(content);
        }

        public void OnDocumentation(string content)
        {
            foreach (var c in _consumers)
                c.OnDocumentation(content);
        }

        public void OnPlainContent(string content)
        {
            foreach (var c in _consumers)
                c.OnPlainContent(content);
        }

        public void OnAttribute(string content)
        {
            foreach (var c in _consumers)
                c.OnAttribute(content);
        }

        public void OnConvertExample()
        {
            foreach (var c in _consumers)
                c.OnConvertExample();
        }

        public void OnExample(string content)
        {
            foreach (var c in _consumers)
                c.OnExample(content);
        }

        public void OnObsolete(string kind, string content)
        {
            foreach (var c in _consumers)
                c.OnObsolete(kind, content);
        }

        public void OnConditional(string content)
        {
            foreach (var c in _consumers)
                c.OnConditional(content);
        }

        public void StartEnum(string content)
        {
            foreach (var c in _consumers)
                c.StartEnum(content);
        }

        public void OnEnumMember(string content)
        {
            foreach (var c in _consumers)
                c.OnEnumMember(content);
        }

        public void EndEnum()
        {
            foreach (var c in _consumers)
                c.EndEnum();
        }

        public void StartClass(string content)
        {
            foreach (var c in _consumers)
                c.StartClass(content);
        }

        public void OnThreadSafe()
        {
            foreach (var c in _consumers)
                c.OnThreadSafe();
        }

        public void OnCustom(string content)
        {
            foreach (var c in _consumers)
                c.OnCustom(content);
        }

        public void OnCustomProp(string content)
        {
            foreach (var c in _consumers)
                c.OnCustomProp(content);
        }

        public void OnAuto(string content)
        {
            foreach (var c in _consumers)
                c.OnAuto(content);
        }

        public void OnAutoProp(string content)
        {
            foreach (var c in _consumers)
                c.OnAutoProp(content);
        }

        public void OnAutoPtrProp(string content)
        {
            foreach (var c in _consumers)
                c.OnAutoPtrProp(content);
        }

        public void OnCsNone(string content)
        {
            foreach (var c in _consumers)
                c.OnCsNone(content);
        }

        public void EndClass()
        {
            foreach (var c in _consumers)
                c.EndClass();
        }

        public void StartNonSealedClass(string content)
        {
            foreach (var c in _consumers)
                c.StartNonSealedClass(content);
        }

        public void EndNonSealedClass()
        {
            foreach (var c in _consumers)
                c.EndNonSealedClass();
        }

        public void StartStaticClass(string content)
        {
            foreach (var c in _consumers)
                c.StartStaticClass(content);
        }

        public void EndStaticClass()
        {
            foreach (var c in _consumers)
                c.EndStaticClass();
        }

        public void StartStruct(string content)
        {
            foreach (var c in _consumers)
                c.StartStruct(content);
        }

        public void EndStruct()
        {
            foreach (var c in _consumers)
                c.EndStruct();
        }

        public void StartStructNoLayout(string content)
        {
            foreach (var c in _consumers)
                c.StartStructNoLayout(content);
        }

        public void EndStructNoLayout()
        {
            foreach (var c in _consumers)
                c.EndStructNoLayout();
        }

        public void OnSyncJobs()
        {
            foreach (var c in _consumers)
                c.OnSyncJobs();
        }
    }
}
