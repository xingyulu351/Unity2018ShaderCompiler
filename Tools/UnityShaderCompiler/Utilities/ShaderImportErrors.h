#pragma once
#include <string>
#include <set>
#include "ShaderImportUtils.h"

struct ShaderImportError
{
    ShaderImportError(const core::string& msg_, const core::string& file_, int line_, bool warning_, ShaderCompilerPlatform api_)
        : message(msg_), file(file_), line(line_), warning(warning_), api(api_) {}

    core::string    message;
    core::string file;
    int     line;
    bool    warning;
    ShaderCompilerPlatform api;

    bool operator<(const ShaderImportError& o) const
    {
        if (file != o.file)
            return file < o.file;
        if (line != o.line)
            return line < o.line;
        if (api != o.api)
            return api < o.api;
        if (warning != o.warning)
            return (int)warning < (int)o.warning;
        return message < o.message;
    }
};


class ShaderImportErrors
{
public:
    typedef std::set<ShaderImportError> ErrorContainer;
public:
    void AddImportError(const core::string& err, int line, bool warning, ShaderCompilerPlatform api = kShaderCompPlatformCount);
    void AddImportError(const core::string& err, const core::string& file, int line, bool warning, ShaderCompilerPlatform api = kShaderCompPlatformCount);
    void AddErrorsFrom(const ShaderImportErrors& o);
    void Clear() { m_Errors.clear(); }
    void LogErrors(CgBatchErrorCallback callback, void* callbackData) const;
    void LogErrors(ShaderCompilerPreprocessCallbacks* callbacks) const;
    void MakeErrorSetSaner();
    void AddSnippetInfoToErrorMessages(const char* programType, int platformKeywordCount, int userKeywordCount, const core::string* keywords);
    bool HasErrors() const;
    bool HasErrorsOrWarnings() const { return !m_Errors.empty(); }

    ErrorContainer& GetErrors() { return m_Errors; }
    const ErrorContainer& GetErrors() const { return m_Errors; }

private:
    ErrorContainer  m_Errors;
};
