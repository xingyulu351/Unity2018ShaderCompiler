This folder contains the setup for Unity's LLDB formatters.
LLDB formatters are small python functions that produce a string describing a type.

Install instructions:

1. Run python $<YOUR_REPO>/Tools/LLDBFormatters/generate-lldbinit.py
2. You should see "Set up script successfully written to /Users/<YOUR_USER>/.lldbinit"
3. Confirm installation by running "lldb" on your terminal, you should see "Searching for Unity formatters, N formatters found"

Once you've done this any new lldb formatters added to the repo you generated from will be automattically picked up whenever LLDB is run.
If you would like to have lldb search for printers from a different repo, simply repeat the above instructions within your desired repo.
For an example of how to write a basic lldb formatter see Tools/LLDBFormatters/examples

Happy Debugging!