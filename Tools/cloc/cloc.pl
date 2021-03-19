#!/usr/bin/env perl
use FindBin qw($Bin);
system(
    "perl $Bin/cloc-1.52.pl --exclude-ext=meta --read-lang-def=$Bin/lang_defs.txt --exclude-list-file=$Bin/clocexclude.txt --skip-uniqueness --list-file=$Bin/clocruntime.txt >loc_runtime.txt"
);
system(
    "perl $Bin/cloc-1.52.pl --exclude-ext=meta --read-lang-def=$Bin/lang_defs.txt --exclude-list-file=$Bin/clocexclude.txt --skip-uniqueness --list-file=$Bin/cloceditor.txt >loc_editor.txt"
);
