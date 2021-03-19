import re
import os
import os.path

def get_files_to_process(dirs_list, extensions_list, dir_exclusions, file_exclusions):
    files = []
    for dir in dirs_list:
        for dirpath, dirnames, filenames in os.walk(dir):
            dirnames[:] = [ dirname for dirname in dirnames if not (dirname in dir_exclusions) ]
            for filename in filenames:
                full_file_path = os.path.join(dirpath, filename)
                file_name, file_extension = os.path.splitext(filename)
                if (file_extension.lower() in extensions_list) and (not filename in file_exclusions):
                    files.append(full_file_path)
    return files

def replace_in_file(filename, replace_rules, special_replace_rules, special_replace_rules_exclusions):
    replaces = []
    file_name, file_extension = os.path.splitext(filename)
    file_name = os.path.basename(filename)
    tmp_filename = filename + '.tmp'
    file_replace_rules = replace_rules
    if (file_extension != '.bindings') and (not file_name in special_replace_rules_exclusions):
        file_replace_rules = file_replace_rules + special_replace_rules
    with open(filename, 'r') as fi, open(tmp_filename, 'w') as fo:
        for lineno, line in enumerate(fi, start=1):
            if line.find('string') != -1 or line.find('UnityStr') != -1: # Small optimization to skip regex evaluation on non-relevant lines
                for search, replace in file_replace_rules:
                    if replace is None:
                        if not re.match(search, line) is None:
                            break
                        continue
                    newline = re.sub(search, replace, line)
                    if newline != line:
                        replaces.append((lineno, line, newline))
                        line = newline
            fo.write(line)
    #for lineno, oldline, newline in replaces:
    #   print '%s:%d' % (filename, lineno)
    #   print '---%s+++%s' % (oldline, newline)
    if len(replaces) != 0:
        os.remove(filename)
        os.rename(tmp_filename, filename)
    else:
        os.remove(tmp_filename)
    return len(replaces)


start_path = os.getcwd()
start_dirs = [
    'Runtime',
    'Editor',
    'Editor/Tools/QuickTime',
    'External/ProphecySDK',
    'PlatformDependent/AndroidPlayer/Source',
    'PlatformDependent/AndroidPlayer/WebRequest',
    'PlatformDependent/CommonApple',
    'PlatformDependent/CommonWebPlugin',
    'PlatformDependent/iPhonePlayer',
    'PlatformDependent/Facebook',
    'PlatformDependent/Linux',
    'PlatformDependent/LinuxStandalone',
    'PlatformDependent/MetroPlayer',
    'PlatformDependent/OSX',
    'PlatformDependent/OSXPlayer',
    'PlatformDependent/PS4/CgBatch',
    'PlatformDependent/PS4/Editor/Native',
    'PlatformDependent/PS4/Source',
    'PlatformDependent/PSP2Player/Source',
    'PlatformDependent/PSP2Player/Editor/Native',
    'PlatformDependent/WebGL/Source',
    'PlatformDependent/Win',
    'PlatformDependent/WinPlayer',
    'PlatformDependent/WinRT',
    'PlatformDependent/XboxOne/Source',
    'PlatformDependent/XboxOne/CgBatch/PlatformPlugin',
    'Tools/UnityShaderCompiler',
]
extentions = ['.cpp', '.mm', '.h', '.bindings']
dir_exclusions = [
    'ScriptUpdater',
    'UnityYAMLMerge',
    'Extensions',
    'External',
    'Jam',
    'mecanim',
    'il2cpp',
    'UWP_IL2CPP_D3D',
    'UWP_IL2CPP_XAML',

    #Reversions because of external dependencies
    'Collab', # libart - which in fact has std::string as our own modifications...
]
file_exclusions = [
    'String.h', 'StringStorageDefault.h', 'StringTests.cpp', 'StringTests.inc.h',
    'CommonStrings.h',
    'WinUnicode.h', 'PathUnicodeConversion.h',
    'WinUnicode.cpp', 'PathUnicodeConversion.cpp',
    'BugReporterCrashReporter.cpp', 'ExternalCrashReporter.cpp', 'ExternalCrashReporter.h',
    'ScreenSelectorWrapper.cpp',

    #Reversions because of external dependencies
    'WinWebViewWindowPrivate.cpp', 'HomeWindow.cpp', 'WebViewWindow.cpp', 'WebViewWindowPrivate.cpp', # cef
    'CompilerHLSLcc.cpp', # hlcc
    'SurfaceAnalysis.cpp', 'IncludeHandler.cpp', # MOJOSHADER
    'ScriptingTypes.h', # std::string used in GameCenter bindings
]
special_files_to_process = [
    'External/sqlite/SQLite.h',
    'External/sqlite/SQLite.cpp',
]

replace_rules = [
    (re.compile(r'\bstd::string\b'), r'core::string'),
    (re.compile(r'\bconst string&'), r'const core::string&'),
    (re.compile(r'\bstd::wstring\b'), r'core::wstring'),
    (re.compile(r'\bUnityStr\b'), r'core::string'),
]
# Replacing 'string' to 'core::string'.
# Disabled since first pass is done
special_replace_rules = [
    #(re.compile(r'.*?//.*?\bstring\b.*'), None), # Ignoring comments
    #(re.compile(r'(^\s*|,\s*|\(\s*|const\s+|<\s*|\w\s+)(?<!include\s<)(?<!CommonString\()string(?![\.\w])'), r'\1core::string'),
]
special_replace_rules_exclusions = [
    'CommonScriptingClasses.h', 'MonoUtility.h', 'Il2CppUtility.h', 'WinRTMarshalers.h', 'WinRTMarshalers.cpp',
    'PathNameUtility.h',
    'ApiGLES.cpp',
    'Pipe.cpp',
    'MdFourGenerator.cpp',
    'UsbDevices.cpp',
    'UnicodeInputView.mm',
    'UnityAdsUnityWrapper.mm',
    'PlayerPrefsPSP2.cpp', 'PlayerPrefsPS4.cpp',
    'ScriptDomainTestFixtureTests.cpp',
]


print 'Starting in %s:' % start_path
for idx in range(len(start_dirs)):
    print '\t%s' % start_dirs[idx]
    start_dirs[idx] = os.path.join(start_path, start_dirs[idx])
print 'Scanning...'

files_to_process = special_files_to_process
files_to_process = files_to_process + get_files_to_process(start_dirs, extentions, dir_exclusions, file_exclusions)
print 'Processing...'

total_replace_count = 0
for f in files_to_process:
    total_replace_count += replace_in_file(f, replace_rules, special_replace_rules, special_replace_rules_exclusions)

print 'Replaced: %d' % total_replace_count
