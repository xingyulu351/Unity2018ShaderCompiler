#include "UnityPrefix.h"
#include "ShaderImportErrors.h"
#include "Runtime/Utilities/Word.h"

void ShaderImportErrors::AddImportError(const core::string& err, int line, bool warning, ShaderCompilerPlatform api)
{
    m_Errors.insert(ShaderImportError(err, "", line, warning, api));
}

void ShaderImportErrors::AddImportError(const core::string& err, const core::string& file, int line, bool warning, ShaderCompilerPlatform api)
{
    m_Errors.insert(ShaderImportError(err, file, line, warning, api));
}

void ShaderImportErrors::MakeErrorSetSaner()
{
    // If we have any errors of one API, then just remove errors from all other APIs.
    // Usually it's because of some syntax error, and we don't want dozens of errors
    // all saying the same from different compilers.

    ShaderCompilerPlatform minAPIWithErrors = kShaderCompPlatformCount;
    for (ErrorContainer::const_iterator it = m_Errors.begin(); it != m_Errors.end(); ++it)
    {
        const ShaderImportError& err = *it;
        if (!err.warning)
        {
            if (err.api < minAPIWithErrors)
                minAPIWithErrors = err.api;
        }
    }

    if (minAPIWithErrors == kShaderCompPlatformCount)
        return;

    // leave only errors from that one API
    ErrorContainer newErrors;
    for (ErrorContainer::const_iterator it = m_Errors.begin(); it != m_Errors.end(); ++it)
    {
        const ShaderImportError& err = *it;
        if (err.api == minAPIWithErrors)
            newErrors.insert(err);
    }

    m_Errors.swap(newErrors);
}

static void DumpKeywordList(core::string* dst, const char* header, const core::string* keywords, int start, int count)
{
    *dst += header;
    for (int i = 0, ki = start, n = count; i < n; ++i, ++ki)
    {
        if (i != 0)
            *dst += ' ';
        *dst += keywords[ki];
    }
}

void ShaderImportErrors::AddSnippetInfoToErrorMessages(const char* programType, int platformKeywordCount, int userKeywordCount, const core::string* keywords)
{
    if (m_Errors.empty())
        return;

    // add indication for which shader type and keyword variant we were compiling
    // do this as "details" message part, after the special "\\n" delimiter

    core::string typeMsg = Format("\\nCompiling %s program", programType);

    core::string definesMsg;
    if (userKeywordCount > 0)
        DumpKeywordList(&definesMsg, " with ", keywords, platformKeywordCount, userKeywordCount);
    if (platformKeywordCount > 0)
        DumpKeywordList(&definesMsg, "\nPlatform defines: ", keywords, 0, platformKeywordCount);

    ErrorContainer newErrors;
    for (ErrorContainer::const_iterator it = m_Errors.begin(); it != m_Errors.end(); ++it)
    {
        ShaderImportError err = *it;
        err.message += typeMsg;
        err.message += definesMsg;
        newErrors.insert(err);
    }

    m_Errors.swap(newErrors);
}

static void LogErrorsCB(
    void* userData,
    CgBatchErrorType type,
    int platform,
    int line,
    const char* fileStr,
    const char* messageStr
)
{
    ShaderCompilerPreprocessCallbacks* callbacks = reinterpret_cast<ShaderCompilerPreprocessCallbacks*>(userData);
    if (callbacks)
        callbacks->OnError(type, platform, line, fileStr, messageStr);
}

void ShaderImportErrors::LogErrors(ShaderCompilerPreprocessCallbacks* callbacks) const
{
    LogErrors(LogErrorsCB, callbacks);
}

void ShaderImportErrors::LogErrors(CgBatchErrorCallback callback, void* callbackData) const
{
    for (ErrorContainer::const_iterator it = m_Errors.begin(); it != m_Errors.end(); ++it)
    {
        const ShaderImportError& err = *it;
        Assert(!err.message.empty());

        callback(callbackData, err.warning ? kCgBatchErrorWarning : kCgBatchErrorError, err.api, err.line, err.file.c_str(), err.message.c_str());
    }
}

bool ShaderImportErrors::HasErrors() const
{
    for (ErrorContainer::const_iterator it = m_Errors.begin(); it != m_Errors.end(); ++it)
    {
        if (!it->warning)
            return true;
    }
    return false;
}

void ShaderImportErrors::AddErrorsFrom(const ShaderImportErrors& o)
{
    m_Errors.insert(o.m_Errors.begin(), o.m_Errors.end());
}
