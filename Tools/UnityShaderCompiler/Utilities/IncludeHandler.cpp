#include "UnityPrefix.h"
#include "Tools/UnityShaderCompiler/ShaderCompiler.h"
#include "Tools/UnityShaderCompiler/PlatformPlugin.h"
#include "ProcessIncludes.h"

#include "External/ShaderCompilers/hlsl2glslfork/include/hlsl2glsl.h"

#include "Runtime/Utilities/HashStringFunctions.h"
#include "Runtime/Utilities/PathNameUtility.h"
#include "Runtime/Utilities/Word.h"

#include "Editor/Src/Utility/TextUtilities.h"

// ----------------------------------------------------------------------

// this function massages the filenames in the error list if they have been seen to be opened,
// using whichever node is at the head of the list (most recently opened file by that name)
static void UpdateErrorsFilenames(const OpenedIncludeFileMap& openedFilesMap, ShaderImportErrors& errors)
{
    // since we can't modify std::set in place, we copy the errors into a new local set and swap.
    ShaderImportErrors::ErrorContainer &outErrors = errors.GetErrors();
    ShaderImportErrors::ErrorContainer localErrors;

    for (ShaderImportErrors::ErrorContainer::iterator it = outErrors.begin(); it != outErrors.end(); ++it)
    {
        const ShaderImportError& error = *it;

        OpenedIncludeFileMap::const_iterator openedFile = openedFilesMap.find(error.file);

        // if we have opened this filename, update to the full pathname we used
        if (openedFile != openedFilesMap.end())
        {
            localErrors.insert(ShaderImportError(error.message, openedFile->second, error.line, error.warning, error.api));
        }
        else
        {
            localErrors.insert(ShaderImportError(error.message, error.file, error.line, error.warning, error.api));
        }
    }

    outErrors.swap(localErrors);
}

HLSLIncludeHandler::HLSLIncludeHandler(const IncludeSearchContext& includeContext)
    : m_IncludeContext(includeContext)
{}

HLSLIncludeHandler::HLSLIncludeHandler(const ShaderCompileInputInterface* input)
    : m_IncludeContext(*input->GetIncludeSearchContext())
{
}

HRESULT HLSLIncludeHandler::Open(const core::string& filename, const void* *ppData, UINT* pBytes)
{
    core::string str;

    if (!ReadTextFile(filename, str))
        return E_FAIL_IMPL;

    if (str.empty() || str[str.length() - 1] != '\n')
        str += '\n'; // make sure it ends with a newline, in case last thing was a preprocessor directive

    size_t length = str.size();
    char* data = new char[length + 1];
    if (data == NULL)
        return E_OUTOFMEMORY_IMPL;

    memcpy(data, str.c_str(), length + 1);

    *ppData = data;
    *pBytes = static_cast<UINT>(length);

    {
        Mutex::AutoLock lock(m_Mutex);
        m_PathMap.insert(std::make_pair(*ppData, filename));
    }

    return S_OK_IMPL;
}

HRESULT WINAPI_IMPL HLSLIncludeHandler::Open(D3D10_INCLUDE_TYPE IncludeType, const char* pFileName, const void* pParentData, const void* *ppData, UINT *pBytes)
{
    Mutex::AutoLock lock(m_Mutex);

    ParentPathMap::iterator parent = m_PathMap.find(pParentData);
    core::string convertedFileName = pFileName;
    ConvertSeparatorsToUnity(convertedFileName);

    if (parent != m_PathMap.end())
    {
        core::string convertedParent = parent->second;
        ConvertSeparatorsToUnity(convertedParent);
        core::string name = AppendPathName(DeleteLastPathNameComponentWithAnySlash(convertedParent), convertedFileName);
        HRESULT hr = Open(name, ppData, pBytes);
        if (hr != E_FAIL_IMPL)
        {
            m_OpenedFiles[pFileName] = name;
            return hr;
        }
    }

    for (size_t i = 0; i < m_IncludeContext.includePaths.size(); ++i)
    {
        core::string name = AppendPathName(m_IncludeContext.includePaths[i], convertedFileName);
        HRESULT hr = Open(name, ppData, pBytes);
        if (hr != E_FAIL_IMPL)
        {
            m_OpenedFiles[pFileName] = name;
            return hr;
        }
    }

    core::string redirectedName = ResolveRedirectedPath(convertedFileName, m_IncludeContext.redirectedFolders);
    if (!redirectedName.empty())
    {
        HRESULT hr = Open(redirectedName, ppData, pBytes);
        if (hr != E_FAIL_IMPL)
        {
            m_OpenedFiles[pFileName] = redirectedName;
            return hr;
        }
    }

    return E_FAIL_IMPL;
}

HRESULT WINAPI_IMPL HLSLIncludeHandler::Close(const void* pData)
{
    Mutex::AutoLock lock(m_Mutex);

    m_PathMap.erase(pData);

    delete[] (char*)pData;
    return S_OK_IMPL;
}

void HLSLIncludeHandler::UpdateErrorsWithFullPathnames(ShaderImportErrors &errors)
{
    UpdateErrorsFilenames(m_OpenedFiles, errors);
}

std::vector<core::string> HLSLIncludeHandler::GetOpenedIncludeFiles() const
{
    std::vector<core::string> fileList;
    fileList.reserve(m_OpenedFiles.size());
    for (OpenedIncludeFileMap::const_iterator it = m_OpenedFiles.begin(); it != m_OpenedFiles.end(); ++it)
    {
        fileList.push_back(FlattenRelativePath(it->second, false));
    }
    return fileList;
}

// ----------------------------------------------------------------------

static bool FindReadInclude(const core::string& fileName, const core::string& parentPath, bool suppressErrors,
    const IncludeSearchContext& includeContext, ShaderImportErrors* outErrors,
    ShaderCompilerPlatform api, core::string& outText, core::string& outPath)
{
    if (!parentPath.empty())
    {
        core::string name = AppendPathName(DeleteLastPathNameComponent(parentPath), fileName);
        bool success = ReadTextFile(name, outText);
        if (success)
        {
            outPath = name;
            return true;
        }
    }

    for (std::size_t i = 0; i < includeContext.includePaths.size(); ++i)
    {
        core::string fullFileName = AppendPathName(includeContext.includePaths[i], fileName);
        if (ReadTextFile(fullFileName, outText))
        {
            outPath = fullFileName;
            return true;
        }
    }

    core::string convertedFileName = fileName;
    ConvertSeparatorsToUnity(convertedFileName);
    core::string redirectedName = ResolveRedirectedPath(convertedFileName, includeContext.redirectedFolders);
    if (!redirectedName.empty())
    {
        if (ReadTextFile(redirectedName, outText))
        {
            outPath = redirectedName;
            return true;
        }
    }

    // our up to date mojoshader that does surface shader preprocessing can produce better error messages
    // with accurate line numbers. This is kept for the legacy mojoshader preprocessor embedded in hlsl2glsl
    if (!suppressErrors)
        outErrors->AddImportError(Format("failed to open source file: '%s'", fileName.c_str()), 0, false, api);
    return false;
}

static bool IncludeOpenCallback(bool isSystem, const char* fname, const char* parentfname, const char* parent, std::string& output, size_t& outHash, void* d)
{
    GLSLIncludeHandler* handler = reinterpret_cast<GLSLIncludeHandler*>(d);

    core::string parentFilename = parentfname;

    if (!parentFilename.empty())
    {
        UInt32 parentHash = ComputeStringHash32(parent);

        GLSLIncludeHandler::ParentPathMap::iterator it = handler->m_PathMap.find(parentHash);

        if (it != handler->m_PathMap.end())
            parentFilename = it->second;
    }

    core::string outText;
    core::string outFilename;
    if (!FindReadInclude(fname, parentFilename, handler->m_SuppressErrors, handler->m_IncludeContext, handler->m_Errors, handler->m_API, outText, outFilename))
        return false;

    output = outText;
    outHash = ComputeStringHash32(output);

    handler->m_OpenedFiles[fname] = outFilename;
    handler->m_PathMap.insert(std::make_pair(outHash, outFilename));

    return true;
}

static void IncludeCloseCallback(const char* data, size_t dataHash, void* d)
{
    GLSLIncludeHandler* handler = reinterpret_cast<GLSLIncludeHandler*>(d);

    GLSLIncludeHandler::ParentPathMap::iterator it = handler->m_PathMap.find(dataHash);

    if (it != handler->m_PathMap.end())
        handler->m_PathMap.erase(it);
}

GLSLIncludeHandler::GLSLIncludeHandler()
    : m_IncludeContext()
    , m_Errors(NULL)
    , m_API(kShaderCompPlatformCount)
    , m_SuppressErrors(false)
{
}

GLSLIncludeHandler::GLSLIncludeHandler(const IncludeSearchContext& includeContext, ShaderImportErrors& outErrors, ShaderCompilerPlatform api)
    : m_IncludeContext(includeContext)
    , m_Errors(&outErrors)
    , m_API(api)
    , m_SuppressErrors(false)
{}

GLSLIncludeHandler::GLSLIncludeHandler(const ShaderCompileInputInterface* input, ShaderImportErrors& outErrors, ShaderCompilerPlatform api)
    : m_Errors(&outErrors)
    , m_API(api)
    , m_SuppressErrors(false)
{
    m_IncludeContext = *input->GetIncludeSearchContext();
}

void GLSLIncludeHandler::SetupHlsl2GlslCallbacks(Hlsl2Glsl_ParseCallbacks &callbacks)
{
    callbacks.includeOpenCallback = &IncludeOpenCallback;
    callbacks.includeCloseCallback = &IncludeCloseCallback;
    callbacks.data = (void*)this;
}

void GLSLIncludeHandler::UpdateErrorsWithFullPathnames(ShaderImportErrors& errors)
{
    UpdateErrorsFilenames(m_OpenedFiles, errors);
}
