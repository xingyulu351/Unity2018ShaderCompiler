import lldb
import os
import sys
import datetime
import subprocess

REPO_PATH = "<INSERT_REPO_PATH>"

def GetFormatters(repo_root):
    print "Searching for Unity formatters"

    if not os.path.exists(repo_root):
        print "Failed to find repo root path " + repo_root
        return []

    lldb_files = []
    lldb_files += search_dir_recursively_for_formatter(repo_root)

    print str(len(lldb_files)) + " formatters found"
    for lldb_file in lldb_files:
        print lldb_file

    return lldb_files

def search_dir_recursively_for_formatter(dir):
    output = subprocess.check_output(['mdfind', '-onlyin', dir, 'kMDItemFSName == *lldb_formatters.py']).rstrip('\n\t')
    return output.split("\n")

def __lldb_init_module(debugger, dict):

    # Create and enable Unity catagory
    debugger.HandleCommand('type category define -e "Unity"')

    for formatter in GetFormatters(REPO_PATH):
        debugger.HandleCommand('command script import ' + formatter)

    # Allow breakpoints within header files (https://lldb.llvm.org/troubleshooting.html)
    debugger.HandleCommand('settings set target.inline-breakpoint-strategy always')
