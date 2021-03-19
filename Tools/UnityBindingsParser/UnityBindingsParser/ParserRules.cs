using System.Text.RegularExpressions;

namespace UnityBindingsParser
{
    public partial class Parser
    {
        private PatternBasedDispatcher m_TopLevelRules;
        private PatternBasedDispatcher m_CommonRules;

        private void InitRules()
        {
            m_CommonRules = new PatternBasedDispatcher(FlushAccumulatedContent)
            {
                { ParserUtil.DocumentationMarker, ReadDocumentation, MarkerType.DocMarker},
                { "CONVERTEXAMPLE", _ => m_Consumer.OnConvertExample() },
                { "BEGIN DOC", ReadMultilineDocumentation },
                { "BEGIN EX", ReadExample },
                { "OBSOLETE", ReadObsolete },
                { "CONDITIONAL", line => m_Consumer.OnConditional(line), MarkerType.Conditional },
                { ParserUtil.DocumentationFile, ImportDocFile, MarkerType.Docfile },
                { ParserUtil.AttributeMarker, m_Consumer.OnAttribute, MarkerType.Attribute},
                { "FLUSHCONDITIONS", _ => {}},
                { "THREAD_SAFE", ThrowThreadSafeNotSupportedException }
            };
            m_TopLevelRules = m_CommonRules.CombinedWith(new PatternBasedDispatcher
            {
                { "ENUM", ReadEnum, MarkerType.EnumDecl },
                { "C++RAW", line => EnterState(ParsingState.CppRaw, line), MarkerType.CppRaw },
                { "CSRAW", line => EnterState(ParsingState.CsRaw, line), MarkerType.CsRaw },
                { "THREAD_AND_SERIALIZATION_SAFE", MarkerReaderFor(m_Consumer.OnThreadSafe) },
                { "THREAD_AND_SERIALIZATION_UNSAFE_THROW_EXCEPTION", ThrowUnsafeThrowExceptionNotSupportedException },
                { "CLASS", ReadClass, MarkerType.Class },
                { "STATIC_CLASS", ReadStaticClass, MarkerType.Class },
                { "NONSEALED_CLASS", ReadNonSealedClass, MarkerType.Class },
                { "STRUCT", ReadStruct, MarkerType.Struct },
                { "STRUCT_NOLAYOUT", ReadStructNoLayout, MarkerType.StructNoLayout }
            });
        }

        private PatternBasedDispatcher GetMultilineDocDispatcher(CancellationToken cancellationToken)
        {
            return new PatternBasedDispatcher(FlushAccumulatedContent)
            {
                {"CONVERTEXAMPLE", _ => m_Consumer.OnConvertExample(), MarkerType.ConvertExampleMultiline},
                {"BEGIN EX", ReadExample, MarkerType.BeginExMultiline},
                {"END DOC", _ => cancellationToken.Cancel(), MarkerType.EnddocMultiline}
            };
        }

        private PatternBasedDispatcher GetEnumDispatcher(CancellationToken cancellationToken)
        {
            return m_CommonRules.CombinedWith(
                new PatternBasedDispatcher
                {
                    {
                        new Regex(@"^\s*(\w+\s*=.*)"), m_Consumer.OnEnumMember,
                        MarkerType.EnumMemberWithAssignment
                    },
                    {"END", _ => cancellationToken.Cancel(), MarkerType.EnumCancel},
                    {
                        new Regex(@"\}"), _ => cancellationToken.Cancel(),
                        MarkerType.EnumCancel
                    },
                    {
                        new Regex(@"^\s*(\w+\s*,*)"), m_Consumer.OnEnumMember,
                        MarkerType.EnumMemberNoAssignment
                    },
                    {
                        ParserUtil.SimpleCommentMarker, ReadSimpleComment,
                        MarkerType.SimpleComment
                    },
                    {"#if", HandlePoundIf, MarkerType.StartPoundIf},
                    {"#endif", HandleEndIf, MarkerType.EndPoundIf}
                });
        }

        private PatternBasedDispatcher GetTypeBodyRules(CancellationToken cancellationToken)
        {
            return m_TopLevelRules.CombinedWith(
                new PatternBasedDispatcher
                {
                    { "AUTO_PTR_PROP", SimpleBlockReaderFor(m_Consumer.OnAutoPtrProp)},
                    { "AUTO_PROP", SimpleBlockReaderFor(m_Consumer.OnAutoProp) },
                    { "AUTO", line => m_Consumer.OnAuto(line)},
                    { "CUSTOM_PROP", BlockReaderFor(ReadCustomProp) },
                    { "CUSTOM", BlockReaderFor(m_Consumer.OnCustom)},
                    { "CSNONE", m_Consumer.OnCsNone },
                    { "SYNC_JOBS", MarkerReaderFor(m_Consumer.OnSyncJobs) },
                    { "END", _ => cancellationToken.Cancel() }
                });
        }
    }
}
