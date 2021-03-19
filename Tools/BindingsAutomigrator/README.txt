INTRODUCTION:
BindingsAutomigrator is a tool we that use to migrate bindings to the new bindings system.

It is a standalone tool, and does not tie into anything else.

We will remove it when the bindings migration effort is complete, so it is temporary in nature.

USAGE:
1) Build editor
2) Build tool manually (whatever tool can build the .sln)
3) cd to root of unity repository
4) Tools/BindingsAutomigrator/BindingsAutomigrator/bin/Debug/BindingsAutomigrator.exe RELATIVE_PATH_TO_EXISTING_BINDINGS_FILE [OPTIONAL_COMMA_SEPERATED_DEFINE_LIST]

USAGE EXAMPLE:
Tools/BindingsAutomigrator/BindingsAutomigrator/bin/Debug/BindingsAutomigrator.exe Editor/Mono/Hardware.bindings ENABLE_MONO,UNITY_PS4


- rb (Ralph Brorsen), Scripting Integration Team, 2016/11/10