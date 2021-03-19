import lldb

# WARNING: LLDB FORMATTERS MUCH USE UNDERSCORES IN THEIR NAMES: Using hyphens will result in this hard to track down error messages "error: module importing failed: ('invalid syntax', ('temp.py', 1, 12, 'import lldb_formatters\n'))"
# Formatters MUST also end with "lldb_formatters.py" in order to be picked up by the scanner, after adding your formatter run 'lldb' on the terminal to confirm your formatter is listed

# Useful links:
# https://opensource.apple.com/source/lldb/lldb-112/www/varformats.html
# https://lldb.llvm.org/python_reference/

# Each type should have its own python function
# value is an 'SBValue' type (https://lldb.llvm.org/python_reference/lldb.SBValue-class.html) representing an instance of the registered type
# dict can be ignored, it stores internal python state for LLDB
# The return value of the function is the string used as the types summary

# Example
# class MyUnityClass
# {
#     int m_ID;
#     const char* m_Name;
# };

def unity_type_summary(value, dict):
    m_id = value.GetChildMemberWithName("m_ID")
    m_name = value.GetChildMemberWithName("m_Name")

    result = "ID: " + m_id.GetValue()

    # As m_Name is a pointer, get value will be the address pointed to, get Summary will provide a string
    result += "| Name: " + m_name.GetSummary()

    return result

# Registration of the summary function happens here
def __lldb_init_module(debugger, dict):

    # -w = Catagory name
    # -F = python function name
    # Last argument is the type name
    debugger.HandleCommand('type summary add -w "Unity" -F basic_lldb_formatters.unity_type_summary MyUnityClass')

    # Regex can also be used for the type names, useful for adding a summary for all instances of a templated class
    # -x = Regex
    debugger.HandleCommand('type summary add -w "Unity" -F basic_lldb_formatters.unity_type_summary -x "^MyUnityClass<.*>$"')
