// Unity Crash Handler, somewhat based on XCrashReport by Hans Dietrich:
//   http://www.codeproject.com/debug/XCrashReportPt4.asp

#pragma once


#define INI_SECTION         L"UnityCrashHandler"
#define INI_KEY_APPNAME     L"AppName"
#define INI_KEY_PROJECTFOLDER   L"UserFolder"
#define INI_KEY_EDITORMODE  L"EditorMode"
#define INI_KEY_INFO        L"Info"
#define INI_KEY_TIME        L"Time"

#define INI_KEY_FILES_PATH  L"FilesPath"

// each file entry: path|description|type
#define INI_FILES_SEP '|'
#define INI_KEY_FILE_TEMPL  L"File%02d"
#define INI_MAX_FILE_ITEMS  999

// each directory to delete entry: path
#define INI_KEY_DELETE_TEMPL    L"Delete%02d"
#define INI_MAX_DELETE_ITEMS    999
