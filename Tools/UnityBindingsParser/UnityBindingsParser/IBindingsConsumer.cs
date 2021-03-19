namespace UnityBindingsParser
{
    public interface IBindingsConsumer
    {
        void OnCppRaw(string content);
        void OnCppRaw(string[] content);
        void OnCsRaw(string content);
        void OnCsRaw(string[] content);
        void OnDocumentation(string content);
        void OnPlainContent(string content);
        void OnAttribute(string content);

        void OnConvertExample();
        void OnExample(string content);

        void OnObsolete(string kind, string content);
        void OnConditional(string content);

        void StartEnum(string content);
        void OnEnumMember(string content);
        void EndEnum();

        void StartClass(string codeContent);
        void OnThreadSafe();
        void OnCustom(string content);
        void OnCustomProp(string content);
        void OnAuto(string content);
        void OnAutoProp(string codeContent);
        void OnAutoPtrProp(string content);
        void OnCsNone(string content);
        void EndClass();

        void StartNonSealedClass(string content);
        void EndNonSealedClass();

        void StartStaticClass(string content);
        void EndStaticClass();

        void StartStruct(string codeContent);
        void EndStruct();

        void StartStructNoLayout(string content);
        void EndStructNoLayout();

        void OnSyncJobs();
    }
}
