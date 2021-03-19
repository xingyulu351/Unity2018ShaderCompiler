#pragma once

#include <string>
#include <vector>
#include "../ShaderCompilerClient.h"

class ShaderImportErrors;

void ConvertIncludePathSlashes(core::string &shaderSource);

bool WriteTextToFile(const char* fname, const char* text);

// Reads text file contents. Removes UTF8 BOM if present. Uses internal cache of already seen files.
bool ReadTextFile(const core::string& pathName, core::string& output);
void ClearReadTextFilesCache();

void CreateDirectoryAtPath(const char* name);
bool DeleteFileAtPath(const char* fname);
void DeleteDirectoryAtPath(const char* name);

core::string ResolveRedirectedPath(const core::string& path, const IncludeFolderMap& redirectedFolders);

void ProcessIncludeDirectives(core::string& text, const IncludeSearchContext& includeContext, ShaderImportErrors& outErrors, ShaderCompilerPlatform api);
void ComputeIncludesHash(const core::string& text, const IncludeSearchContext& includeContext, UInt32 outHash[4], std::vector<core::string> &outIncludedFiles);
