This is a tool to help you work with crash dumps (*.dmp files) from Windows players.

Unlike dumps from the Editor, you cannot simply load these dump files into VS and go because 
the executable name is different for every user project. This tool parses the crash dump to
figure out which actual Unity Player executable the dump refers to, and retrieves it from the
symbol server; after that's done then you *can* open the dump in VS and just hit go.

Run DumpPrep.exe with no arguments for usage information.
