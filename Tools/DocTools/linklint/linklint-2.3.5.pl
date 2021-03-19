#!/usr/bin/perl -- # -*- perl -*-

#==========================================================================
#  linklint  - a fast link checker and web site maintenance tool.
#  Copyright (C) 1997 -- 2001 James B. Bowlin.  All rights reserved.
#
#  This program is free software; you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation; either version 2 of the License, or
#  (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program; if not, write to
#
#      Free Software Foundation, Inc.
#      59 Temple Place - Suite 330
#      Boston, MA  02111-1307, USA
#
#  Notice, that ``free software'' addresses the fact that this program
#  is __distributed__ under the term of the GNU General Public License
#  and because of this, it can be redistributed and modified under the
#  conditions of this license, but the software remains __copyrghted__
#  by the author. Don't intermix this with the general meaning of
#  Public Domain software or such a derivated distribution label.
#
#  Linklint is a total rewrite of Rick Jansen's 4/15/96 version of webxref.
#
#  Thanks to Celeste Stokely, Scott Perry, Patrick Meyer, Brian Kaminer
#  David Hull, Stephan Petersen, Todd O'Boyle, Vittal Aithal (and
#  many others!) for many excellent suggestions.
#
#  Bugs, comments, suggestions welcome: bugs@linklint.org
#  Updates available at http://www.linklint.org
#
# RECENT CHANGES (see CHANGES.txt for full list):
#
# Version 2.3.5 August 13, 2001
# -----------------------------
#   o added -no_anchors tag (for larger sites)
#   o fixed bug that prevented site checks of
#     some non port 80 sites. (Thanks Rick Perry).
#
# Version 2.3.4 August 8, 2001
# ----------------------------
#   o keep query string for http site checks
#   o added no_query_string option to disable above
#   o added "php" and "jar" file types
#   o look for .jar files from applet tags
#   o s!//+!/!g inside of UniqueUrl()
#   o -http_header and -language options
#
# Version 2.3.3 July 6, 2001
# ---------------------------
#   o added 2nd argument to mkdir() on line 921
#   o for creating url doc directory
#
# Version 2.3.2 June 22, 2001
# ---------------------------
#   o -no_warn_index for missing index file warnings
#   o -concise_url flag to suppress output of valid remote links
#     on STDOUT
#
# Version 2.3.1 June 21, 2001
# ---------------------------
#   o unified -proxy support (no conflict w/ virtual hosts now)
#     and moved it to Request() so we should support proxies
#     for site checking
#
# Version 2.3.0 June 3, 2001
# --------------------------
#   o moved home site and email address
#   o added -help_all -version -license "@"
#   o updated to GPL
#
#========================================================================

$version = "2.3.5";
$date = "August 13, 2001";
$prog = "linklint";

$Usage1 = <<USAGE1;
This is $prog Version $version ($date)
Copyright (c) 1997-2001 James B. Bowlin
USAGE1

$Usage3 = qq~
Use $prog with no arguments for standard usage.
Use "$prog -help_all" for a list of ALL options.
~;

$Version_Usage = <<VERSION_USAGE;
$Usage1
This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

VERSION_USAGE

$LICENSE = <<LICENSE_TEXT;

            GNU GENERAL PUBLIC LICENSE
               Version 2, June 1991

 Copyright (C) 1989, 1991 Free Software Foundation, Inc.
      59 Temple Place, Suite 330,
      Boston, MA  02111-1307  USA
 Everyone is permitted to copy and distribute verbatim copies
 of this license document, but changing it is not allowed.

                Preamble

  The licenses for most software are designed to take away your
freedom to share and change it.  By contrast, the GNU General Public
License is intended to guarantee your freedom to share and change free
software--to make sure the software is free for all its users.  This
General Public License applies to most of the Free Software
Foundation's software and to any other program whose authors commit to
using it.  (Some other Free Software Foundation software is covered by
the GNU Library General Public License instead.)  You can apply it to
your programs, too.

  When we speak of free software, we are referring to freedom, not
price.  Our General Public Licenses are designed to make sure that you
have the freedom to distribute copies of free software (and charge for
this service if you wish), that you receive source code or can get it
if you want it, that you can change the software or use pieces of it
in new free programs; and that you know you can do these things.

  To protect your rights, we need to make restrictions that forbid
anyone to deny you these rights or to ask you to surrender the rights.
These restrictions translate to certain responsibilities for you if you
distribute copies of the software, or if you modify it.

  For example, if you distribute copies of such a program, whether
gratis or for a fee, you must give the recipients all the rights that
you have.  You must make sure that they, too, receive or can get the
source code.  And you must show them these terms so they know their
rights.

  We protect your rights with two steps: (1) copyright the software, and
(2) offer you this license which gives you legal permission to copy,
distribute and/or modify the software.

  Also, for each author's protection and ours, we want to make certain
that everyone understands that there is no warranty for this free
software.  If the software is modified by someone else and passed on, we
want its recipients to know that what they have is not the original, so
that any problems introduced by others will not reflect on the original
authors' reputations.

  Finally, any free program is threatened constantly by software
patents.  We wish to avoid the danger that redistributors of a free
program will individually obtain patent licenses, in effect making the
program proprietary.  To prevent this, we have made it clear that any
patent must be licensed for everyone's free use or not licensed at all.

  The precise terms and conditions for copying, distribution and
modification follow.


            GNU GENERAL PUBLIC LICENSE
   TERMS AND CONDITIONS FOR COPYING, DISTRIBUTION AND MODIFICATION

  0. This License applies to any program or other work which contains
a notice placed by the copyright holder saying it may be distributed
under the terms of this General Public License.  The "Program", below,
refers to any such program or work, and a "work based on the Program"
means either the Program or any derivative work under copyright law:
that is to say, a work containing the Program or a portion of it,
either verbatim or with modifications and/or translated into another
language.  (Hereinafter, translation is included without limitation in
the term "modification".)  Each licensee is addressed as "you".

Activities other than copying, distribution and modification are not
covered by this License; they are outside its scope.  The act of
running the Program is not restricted, and the output from the Program
is covered only if its contents constitute a work based on the
Program (independent of having been made by running the Program).
Whether that is true depends on what the Program does.

  1. You may copy and distribute verbatim copies of the Program's
source code as you receive it, in any medium, provided that you
conspicuously and appropriately publish on each copy an appropriate
copyright notice and disclaimer of warranty; keep intact all the
notices that refer to this License and to the absence of any warranty;
and give any other recipients of the Program a copy of this License
along with the Program.

You may charge a fee for the physical act of transferring a copy, and
you may at your option offer warranty protection in exchange for a fee.

  2. You may modify your copy or copies of the Program or any portion
of it, thus forming a work based on the Program, and copy and
distribute such modifications or work under the terms of Section 1
above, provided that you also meet all of these conditions:

    a) You must cause the modified files to carry prominent notices
    stating that you changed the files and the date of any change.

    b) You must cause any work that you distribute or publish, that in
    whole or in part contains or is derived from the Program or any
    part thereof, to be licensed as a whole at no charge to all third
    parties under the terms of this License.

    c) If the modified program normally reads commands interactively
    when run, you must cause it, when started running for such
    interactive use in the most ordinary way, to print or display an
    announcement including an appropriate copyright notice and a
    notice that there is no warranty (or else, saying that you provide
    a warranty) and that users may redistribute the program under
    these conditions, and telling the user how to view a copy of this
    License.  (Exception: if the Program itself is interactive but
    does not normally print such an announcement, your work based on
    the Program is not required to print an announcement.)

These requirements apply to the modified work as a whole.  If
identifiable sections of that work are not derived from the Program,
and can be reasonably considered independent and separate works in
themselves, then this License, and its terms, do not apply to those
sections when you distribute them as separate works.  But when you
distribute the same sections as part of a whole which is a work based
on the Program, the distribution of the whole must be on the terms of
this License, whose permissions for other licensees extend to the
entire whole, and thus to each and every part regardless of who wrote it.

Thus, it is not the intent of this section to claim rights or contest
your rights to work written entirely by you; rather, the intent is to
exercise the right to control the distribution of derivative or
collective works based on the Program.

In addition, mere aggregation of another work not based on the Program
with the Program (or with a work based on the Program) on a volume of
a storage or distribution medium does not bring the other work under
the scope of this License.

  3. You may copy and distribute the Program (or a work based on it,
under Section 2) in object code or executable form under the terms of
Sections 1 and 2 above provided that you also do one of the following:

    a) Accompany it with the complete corresponding machine-readable
    source code, which must be distributed under the terms of Sections
    1 and 2 above on a medium customarily used for software interchange; or,

    b) Accompany it with a written offer, valid for at least three
    years, to give any third party, for a charge no more than your
    cost of physically performing source distribution, a complete
    machine-readable copy of the corresponding source code, to be
    distributed under the terms of Sections 1 and 2 above on a medium
    customarily used for software interchange; or,

    c) Accompany it with the information you received as to the offer
    to distribute corresponding source code.  (This alternative is
    allowed only for noncommercial distribution and only if you
    received the program in object code or executable form with such
    an offer, in accord with Subsection b above.)

The source code for a work means the preferred form of the work for
making modifications to it.  For an executable work, complete source
code means all the source code for all modules it contains, plus any
associated interface definition files, plus the scripts used to
control compilation and installation of the executable.  However, as a
special exception, the source code distributed need not include
anything that is normally distributed (in either source or binary
form) with the major components (compiler, kernel, and so on) of the
operating system on which the executable runs, unless that component
itself accompanies the executable.

If distribution of executable or object code is made by offering
access to copy from a designated place, then offering equivalent
access to copy the source code from the same place counts as
distribution of the source code, even though third parties are not
compelled to copy the source along with the object code.

  4. You may not copy, modify, sublicense, or distribute the Program
except as expressly provided under this License.  Any attempt
otherwise to copy, modify, sublicense or distribute the Program is
void, and will automatically terminate your rights under this License.
However, parties who have received copies, or rights, from you under
this License will not have their licenses terminated so long as such
parties remain in full compliance.

  5. You are not required to accept this License, since you have not
signed it.  However, nothing else grants you permission to modify or
distribute the Program or its derivative works.  These actions are
prohibited by law if you do not accept this License.  Therefore, by
modifying or distributing the Program (or any work based on the
Program), you indicate your acceptance of this License to do so, and
all its terms and conditions for copying, distributing or modifying
the Program or works based on it.

  6. Each time you redistribute the Program (or any work based on the
Program), the recipient automatically receives a license from the
original licensor to copy, distribute or modify the Program subject to
these terms and conditions.  You may not impose any further
restrictions on the recipients' exercise of the rights granted herein.
You are not responsible for enforcing compliance by third parties to
this License.

  7. If, as a consequence of a court judgment or allegation of patent
infringement or for any other reason (not limited to patent issues),
conditions are imposed on you (whether by court order, agreement or
otherwise) that contradict the conditions of this License, they do not
excuse you from the conditions of this License.  If you cannot
distribute so as to satisfy simultaneously your obligations under this
License and any other pertinent obligations, then as a consequence you
may not distribute the Program at all.  For example, if a patent
license would not permit royalty-free redistribution of the Program by
all those who receive copies directly or indirectly through you, then
the only way you could satisfy both it and this License would be to
refrain entirely from distribution of the Program.

If any portion of this section is held invalid or unenforceable under
any particular circumstance, the balance of the section is intended to
apply and the section as a whole is intended to apply in other
circumstances.

It is not the purpose of this section to induce you to infringe any
patents or other property right claims or to contest validity of any
such claims; this section has the sole purpose of protecting the
integrity of the free software distribution system, which is
implemented by public license practices.  Many people have made
generous contributions to the wide range of software distributed
through that system in reliance on consistent application of that
system; it is up to the author/donor to decide if he or she is willing
to distribute software through any other system and a licensee cannot
impose that choice.

This section is intended to make thoroughly clear what is believed to
be a consequence of the rest of this License.

  8. If the distribution and/or use of the Program is restricted in
certain countries either by patents or by copyrighted interfaces, the
original copyright holder who places the Program under this License
may add an explicit geographical distribution limitation excluding
those countries, so that distribution is permitted only in or among
countries not thus excluded.  In such case, this License incorporates
the limitation as if written in the body of this License.

  9. The Free Software Foundation may publish revised and/or new versions
of the General Public License from time to time.  Such new versions will
be similar in spirit to the present version, but may differ in detail to
address new problems or concerns.

Each version is given a distinguishing version number.  If the Program
specifies a version number of this License which applies to it and "any
later version", you have the option of following the terms and conditions
either of that version or of any later version published by the Free
Software Foundation.  If the Program does not specify a version number of
this License, you may choose any version ever published by the Free Software
Foundation.

  10. If you wish to incorporate parts of the Program into other free
programs whose distribution conditions are different, write to the author
to ask for permission.  For software which is copyrighted by the Free
Software Foundation, write to the Free Software Foundation; we sometimes
make exceptions for this.  Our decision will be guided by the two goals
of preserving the free status of all derivatives of our free software and
of promoting the sharing and reuse of software generally.

                NO WARRANTY

  11. BECAUSE THE PROGRAM IS LICENSED FREE OF CHARGE, THERE IS NO WARRANTY
FOR THE PROGRAM, TO THE EXTENT PERMITTED BY APPLICABLE LAW.  EXCEPT WHEN
OTHERWISE STATED IN WRITING THE COPYRIGHT HOLDERS AND/OR OTHER PARTIES
PROVIDE THE PROGRAM "AS IS" WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESSED
OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.  THE ENTIRE RISK AS
TO THE QUALITY AND PERFORMANCE OF THE PROGRAM IS WITH YOU.  SHOULD THE
PROGRAM PROVE DEFECTIVE, YOU ASSUME THE COST OF ALL NECESSARY SERVICING,
REPAIR OR CORRECTION.

  12. IN NO EVENT UNLESS REQUIRED BY APPLICABLE LAW OR AGREED TO IN WRITING
WILL ANY COPYRIGHT HOLDER, OR ANY OTHER PARTY WHO MAY MODIFY AND/OR
REDISTRIBUTE THE PROGRAM AS PERMITTED ABOVE, BE LIABLE TO YOU FOR DAMAGES,
INCLUDING ANY GENERAL, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES ARISING
OUT OF THE USE OR INABILITY TO USE THE PROGRAM (INCLUDING BUT NOT LIMITED
TO LOSS OF DATA OR DATA BEING RENDERED INACCURATE OR LOSSES SUSTAINED BY
YOU OR THIRD PARTIES OR A FAILURE OF THE PROGRAM TO OPERATE WITH ANY OTHER
PROGRAMS), EVEN IF SUCH HOLDER OR OTHER PARTY HAS BEEN ADVISED OF THE
POSSIBILITY OF SUCH DAMAGES.

             END OF TERMS AND CONDITIONS
LICENSE_TEXT

$Usage = <<USAGE;

Usage: linklint \@file \@\@file -flag [option] linkset linkset ...

  -root dir         Set server root to  "dir" (default ".").
  -host name[:port] Use "name" as domain name of site.
  -http             Check site remotely via http (requires -host).
  -net              Check status of remote http links found in site.
  -help             Show help. (Use "-help_all for complete usage).
  -version          Show version info.

Multi File Output:
  -doc dir          Write many output files in "dir" directory.

Single File Output:
  -error  print errors          -out file      direct output to "file"
  -warn   print warnings        -xref          cross reference
  -list   print files found     -forward       sort by referring file

linkset:
  home page only: /  (default)                   root dir only: /#
     entire site: /@                           and all subdirs: /#/#
  specific links: /link1 /link2 ...        "sub" dir and below: /sub/@
USAGE

$Help2 = <<'HELP2';
Site Checking:
  -case             Check filename case (Windows/Dos only, local only).
  -orphan           List all unused (orphan) files (local only).
  -index file       Use "file" as default index file (local only).
  -skip linkset     Don't check html files that match linkset.
  -ignore linkset   Ignore all files matching linkset.
  -limit n          Only check up to n html files (default 500).
  -map /a[=/b]      Subsitute leading /a with /b (for serverside image maps).
  -local linkset    Always get files that match linkset locally.
  -no_warn_index    Don't create "index file not found" warnings
  -http_header xxx  Specifically add header "xxx" to all HTTP requests
  -language zz      Add "Accept-Language: zz" header to all HTTP requests

Network:
  -timeout t        Timeout remote links after t seconds (default 15).
  -delay d          Delay d seconds between requests to same host (default 0).
  -redirect         Follow <meta> redirects in remote urls.
  -proxy host[:port] Send remote http requests through a proxy server.
  -password realm user:password  Authorize access to "realm".
  -concise_url      Suppress STDOUT output of valid remote links.

Remote Status Cache:
  -netmod           Put urls in cache and report modified status.
  -netset           ... and update last modified status in the cache.
  -retry            Only check urls that had host failures.
  -flush            Remove urls from cache that aren't currently being checked.
  -checksum         Exaustive check of modified status.
  -cache dir        Read/save "linklint.url" cache file in this directory.

Output:
  -quiet            Don't print progress on screen.
  -silent           Don't print summary on screen.
  -docbase exp      Overrides defaults for linking html output back to site.
  -textonly         Only write .txt files in -doc directory.
  -htmlonly         Erase .txt files after .html output files are written.
  -output_frames    Have index.html open a new window when viewing data files
  -url_doc_prefix p Prefix Remote URL output files with "p" instead of "url".
  -output_index xxx Output index files will be "xxx.txt" and "xxx.html".
  -dont_output xxx  Don't create output files that contain xxx.

Debug Flags:
  -db1              Debug input, linkset expressions.
  -db2              Show every file that gets checked (not just html).
  -db3              Debug parser. Print tags and links found.
  -db4              Debug socket connections
  -db5              not used
  -db6              Detail last-modified status for remote urls.
  -db7              Print brief debug information checking remote urls.
  -db8              Print headers while checking remote urls.
  -db9              Generate random http errors.

  @cmndfile         Read command line options from "cmndfile".
  @@file            Check status of remote http links in "file".
HELP2

$Examples = << 'EXAMPLES';
Examples:

  1) linklint -doc linkdoc -root dir
     Checks home page.  Output files go in "linkdoc" directory.

  2) linklint -doc linkdoc -root dir /@
     Checks all files under "dir".  Output files go in "linkdoc" directory.

  3) linklint -doc linkdoc -root dir /@ -net
     Checks site as (2).  Then checks all http links found in site.

  4) linklint -doc linkdoc -root dir /#
     Like (2) but only checks files in the root directory.

  5) linklint -doc linkdoc -host host /@ -http
     Same as (2) but checks site using http instead of file system.

  6) linklint -doc linkdoc @@linkdoc/remote.txt
     Check remote link status without rechecking entire site.
EXAMPLES

$ErrUsage = <<ERR_USAGE;
Use $prog with no arguments for simple usage.
Use "$prog -help_all" for a list of ALL options.
ERR_USAGE

#------------------ Start up values ----------------------------------

$URL_PREFIX = "url";
$OUTPUT_INDEX = "index";

#----- Files to try in case of a directory reference like "/" --------

@DefIndex = ('home.html', 'index.html', 'index.shtml', 'index.htm', 'index.cgi', 'wwwhome.html', 'welcome.html',);

$DefCaseSens = '';

#----- File extensions and floormats ---------------

$HtmlExts = 'html|shtml|htm';

$AnyCgi = '\?|^/cgi-bin/|(\.(cgi|pl)(/|$))';

$StandardMaps = '(/imagemap|/cgi-(b|w)in/(imagemap.exe|htimage))';

@fileformat = (
    'php::\.php(\?|$)', "cgi::$AnyCgi",
    'default index::\(default\)', "html::\.($HtmlExts)\$",
    'java archive::\.jar$', 'map::\.map$',
    'image::\.(gif|jpg|jpeg|tif|tiff|pic|pict|hdf|ras|xbm)$', 'text::\.txt$',
    'audio::\.(au|snd|wav|aif|aiff|midi|mid)$', 'video::\.(mpg|mpeg|avi|qt|mov)$',
    'shockwave::\.dcr$', 'applet::\.class$',
    'other::unknown',
);

@filesplit = ('found %3d file%s::.', 'found %3d default index%es::\/$',);

@lostsplit = ('ERROR %3d missing file%s::.', 'ERROR %3d missing director%y::\/$',);

foreach (@fileformat)
{
    ($name, $data) = split(/::/);
    push(@filesplit, "found %3d $name file%s::$data");
    push(@lostsplit, "ERROR %3d missing $name file%s::$data");
}

@schema = ("http", "https", 'ftp', 'javascript', 'mailto', 'gopher', 'file', 'news', 'view-source', 'about', 'unknown');

@schemesplit = ("found %3d other link%s::.");

foreach (@schema)
{
    push(@schemesplit, "found %3d $_ link%s::^$_:");
}

#--------------- Output Floormats ----------------------------------

#---- flags used in the printout lists below ---------------

$LIST = 001;
$NOXR = 010;
$URLDOC = 0100;
$WARN = 002;
$XREF = 020;    # $URLDOC  = 0200;
$ERR = 004;
$FWD = 040;

$PrnFlag = 0;
$SumFlag = $LIST | $ERR | $WARN | $NOXR;
$DocFlag = $LIST | $ERR | $WARN | $NOXR | $XREF | $FWD | $URLDOC;

@SumPrint = (0, 0, 1, $SumFlag);

@SiteForm = (

# file; %Data; $mask; program; @parameters

    "dir;      DirList;  001; 1; found %3d director%y with files; 0",
    "file;     FileList; 011; 2; filesplit; 0",
    "fileX;    FileList; 021; 2; filesplit; 2",
    "fileF;    Forward;  041; 1; found %3d file%s with forward links; 1; contains %d link%s:",
    "remote;   ExtLink;  011; 2; schemesplit; 0",
    "remoteX;  ExtLink;  021; 2; schemesplit; 2",
    "anchor;   Anchor;   011; 1; found %3d named anchor%s; 0",
    "anchorX;  Anchor;   021; 1; found %3d named anchor%s; 2",
    "imgmap;   ImgMap;   011; 1; found %3d named image map%s; 0",
    "imgmapX;  ImgMap;   021; 1; found %3d named image map%s; 2",
    "ignore;   Ignored;  011; 1; ----- %3d ignored file%s; 0",
    "ignoreX;  Ignored;  021; 1; ----- %3d ignored file%s; 2",
    "action;   Action;   011; 1; ----- %3d action%s skipped; 0",
    "actionX;  Action;   021; 1; ----- %3d action%s skipped; 2",
    "skipped;  Skipped;  012; 1; ----- %3d file%s skipped; 0",
    "skipX;    Skipped;  022; 1; ----- %3d file%s skipped; 2",
    "orphan;   OrphList; 001; 1; ----- %3d orphan%s; 0",
    "warn;     WarnList; 012; 1; warn  %3d warning%s; 0",
    "warnX;    WarnList; 022; 1; warn  %3d warning%s; 2; occurred in",
    "warnF;    WarnF;    042; 1; warn  %3d file%s with warnings; 1; had %d warning%s",
    "case;     BadCase;  014; 1; ERROR %3d file%s mismatched case; 0",
    "caseX;    BadCase;  024; 1; ERROR %3d file%s mismatched case; 2",
    "caseF;    CaseF;    044; 1; ERROR %3d file%s with mismatched link(s); 1; had %d mismatched link%s",
    "error;    LostFile; 014; 2; lostsplit; 0",
    "errorX;   LostFile; 024; 2; lostsplit; 2",
    "errorF;   ErrF;     044; 1; ERROR %3d file%s had broken links; 1; had %d broken link%s",
    "errorA;   LostAnch; 014; 1; ERROR %3d missing named anchor%s; 0",
    "errorAX;  LostAnch; 024; 1; ERROR %3d missing named anchor%s; 2",
    "errorM;   LostMap;  014; 1; ERROR %3d missing named image map%s; 0",
    "errorMX;  LostMap;  024; 1; ERROR %3d missing named image maps%s; 2",
    "httpfail; HttpFail; 022; 3; ----- %3d link%s; 0; failed via http",
    "httpok;   HttpOk;   022; 3; ----- %3d link%s; 0; ok via http",
    "mapped;   Mapped;   022; 1; ----- %3d link%s %w mapped; 1; mapped to",

);

@UrlForm = (

    "${URL_PREFIX}ok;    UrlOk;    0001; 3; found %3d url%s;  0; %w ok",
    "${URL_PREFIX}moved; UrlMoved; 0002; 3; ----- %3d url%s;  0; moved",
    "${URL_PREFIX}host;  HostFail; 0104; 3; ERROR %3d host%s; 0; failed; %d host%s:",
    "${URL_PREFIX}fail;  UrlFail;  0014; 3; ERROR %3d url%s;  0; failed",
    "${URL_PREFIX}failX; UrlFailX; 0024; 3; ERROR %3d url%s;  2; failed",
    "${URL_PREFIX}failF; UrlFailF; 0044; 1; ERROR %3d file%s with failed urls; 1; had %d failed url%s",
    "${URL_PREFIX}skip;  UrlWarn;  0012; 3; warn  %3d url%s;  0; not checked",
    "${URL_PREFIX}mod;   UrlMod;   0000; 3; found %3d url%s;  0; %w modified",
    "${URL_PREFIX}warn;  WarnList; 0012; 1; warn  %3d warning%s; 0",
    "${URL_PREFIX}warnX; WarnList; 0022; 1; warn  %3d warning%s; 2; in %d url%s",
    "${URL_PREFIX}warnF; WarnF;     042; 1; warn  %3d url%s with warnings; 1; had %d warning%s",
);

#----- Command Line Input ---------------

$MiscFlags = join "|", qw/

    case
    concise_url
    error
    flush
    forward
    help
    help_all
    htmlonly
    http
    license
    list
    net
    netmod
    netset
    no_anchors
    no_warn_index
    no_query_string
    orphan
    output_frames
    quiet
    redirect
    retry
    silent
    textonly
    version
    warn
    xref

    /;

$HashOpts = join "|", qw/

    dont_output
    http_header
    ignore
    index
    javascript
    language
    local
    map
    skip

    /;

$FullOpts = join "|", qw/

    cache
    delay
    doc
    docbase
    host
    limit
    out
    output_index
    proxy
    root
    timeout
    url_doc_prefix

    /;

%CheckedIn = ();

#---------------------------------------------------------------------------
# Misc. startup values
#---------------------------------------------------------------------------

$Arg{'timeout'} = 15;    # how long to wait for a response
$Arg{'delay'} = 0;       # how long to delay between requests

$Arg{'VERSION'} = $version;

$Headline = "#" . "-" x 60 . "\n";    # used to print lists

$ATTRIB = q~\s*=\s*("\s*([^"]*[^"\s])\s*"|'\s*([^']*[^"\s])\s*'|([^\s"']+))~;

#----- prevent warnings ---------------

%HttpOk = %HttpFail = %CaseF = %WarnF = ();
%UrlWarn = %Ignored = %UrlFailX = %UrlFailF = %LostMap = ();

$UrlOk = $Abort = 0;

###########################################################################
#
# Main code starts here
#
###########################################################################

@ARGV or exit_with($Usage1, $Usage);
@infiles = &ReadArgs(@ARGV);

$OUTPUT_INDEX = $Arg{output_index} if $Arg{output_index};

@UrlHtml = ("${URL_PREFIX}sum", "${URL_PREFIX}${OUTPUT_INDEX}", "${URL_PREFIX}log", "Index of Linklint results for remote urls",);

@SiteHtml = ('summary', $OUTPUT_INDEX, 'log', "Index of Linklint results ",);

$Arg{'DOS'} = $Dos =
    $^O
    ? ($^O =~ /win32/i ? 1 : '')
    : ($ENV{'windir'} ? 1 : '');

$pwdprog = $Dos ? 'cd' : 'pwd';    # program to pwd

$HOME = &GetCwd;                   # this is done twice.

#---------------- Command Line Arguments -------------------------

foreach (@HttpFiles)
{
    push(@infiles, &ReadHttp($_));
}

$Arg{'doc'} && !$DB{1} && do
{
    ($DocDir = $Arg{'doc'}) =~ s#\\#\/#g;
    $DocDir =~ m#^/# || ($DocDir = "$HOME/$DocDir");

    -d $DocDir
        || mkdir($DocDir, 0777)
        || &Error(qq~invalid output directory: "$DocDir"~, 'sys');
    $SiteLog = "$DocDir/log.txt";
    $UrlLog = "$DocDir/${URL_PREFIX}log.txt";
    $LogProgress++;
};

$Arg{'out'} && !$DB{1} && do
{
    $OutLog = $Arg{'out'};
    $OutLog =~ m#^/# || ($OutLog = "$HOME/$OutLog");
};

$Arg{version} and exit_with($Version_Usage);
$Arg{license} and exit_with($Usage1, $LICENSE);
$Arg{'help_all'} and exit_with($Version_Usage, $Usage, $Help2, $Examples);
$Arg{'help'} and exit_with($Usage1, $Examples, $Usage3);

($LogDir = $Arg{'cache'} || $ENV{'LINKLINT'} || $ENV{'HOME'} || $HOME) =~ s#/$##;
$StatFile = $LogDir . "/linklint.url";

#----- Link Spec's ---------------

$Arg{'case'} && do
{
    $Dos || &Error("can only check case under Windows/Dos");
    $CheckCase++;
};

$IgnoreCase = ($Dos && !$CheckCase && !$Arg{'http'}) ? 1 : 0;

foreach (@infiles)
{
    s#^http:#http:#i && (push(@CheckUrls, $_), next);
    $_ = "/$_" unless m#^/#;
    $IgnoreCase && tr/A-Z/a-z/;
    $LinkSet{$_}++;
}

%LinkSet || do
{
    %LinkSet = ('/', '1');
    $Hints{'Use a linkset of /@ to check entire site.'}++;
};

foreach (keys %LinkSet)
{
    m%[\@\#]% || ($Seeds{$_}++, next);    # literals
    m%^([^\@\#]*/)% && $Seeds{$1}++;      # directories
}

%LinkSet && !%Seeds and die("\nError: Could not form any seeds from the linksets.\n");

$LinkSet = &LinkSet("linkset", *LinkSet);

#----- Flags and Options ---------------

&WantNumber($Arg{'limit'}, "-limit");
&WantNumber($Arg{'timeout'}, "-timeout");
&WantNumber($Arg{'delay'}, "-delay");

$Arg{'error'} && ($PrnFlag |= ($NOXR | $ERR));
$Arg{'warn'} && ($PrnFlag |= ($NOXR | $WARN));
$Arg{'list'} && ($PrnFlag |= ($NOXR | $LIST));
$Arg{'xref'} && ($PrnFlag |= $XREF, $PrnFlag &= ~$NOXR);
$Arg{'forward'} && ($PrnFlag |= $FWD, $PrnFlag &= ~($NOXR | $XREF));

$Arg{'quiet'} && $Quiet++;
$Arg{'silent'} && $Silent++;
$DB{2} && $DbLink++;
$DB{3} && $DbP++;

$CacheNet =
       $Arg{'retry'}
    || $Arg{'flush'}
    || $Arg{'netmod'}
    || $Arg{'netset'};

$CheckNet = $Arg{'netmod'} || $Arg{'netset'} || $Arg{'net'};

$CacheNet && !$CheckNet && !@CheckUrls && do
{

    $Arg{'flush'}
        && print STDERR "ERROR: -flush requires -net or -netmod or -netset\n";

    $Arg{'retry'}
        && print STDERR "ERROR: -retry requires -net or -netmod or -netset\n";

    die($ErrUsage);
};

$Limit = $Arg{'limit'} = $Arg{'limit'} || 500;    # check at most 500 files

$Arg{'root'} && do
{
    $DB{1}
        || chdir($Arg{'root'})
        || &Error(qq~invalid root directory: "$Arg{'root'}"~, 'sys');
    &GetCwd;
    $HeaderRoot = $CWD;
};
$ServerRoot = $CWD;

$IgnoreCase && $ServerRoot =~ tr/A-Z/a-z/;

$ServerHost = $Arg{'host'} || '';

#---- get base for links in html files in -doc directory

$DocDir && do
{
    $DocBase =
          defined $Arg{'docbase'} ? $Arg{'docbase'}
        : $Arg{'http'} ? "http://$ServerHost"
        : $Dos ? "file:///$DosDrive|$ServerRoot"
        : "file://$ServerRoot";
};

$Arg{'tilde'} && do
{
    ($ServerTilde = $Arg{'tilde'}) =~ s/#/\$1/g;
    $ServerTilde = '"' . $ServerTilde . '"';
};

%IGNORE && do
{
    $Ignore = &LinkSet('ignore', *IGNORE);
};

%INDEX && do
{
    grep(m/[A-Z]/, @DefIndex = keys %INDEX) && $DefCaseSens++;
    $Arg{'index'} = join(" ", @DefIndex);
};

%JAVASCRIPT && do
{
    foreach (keys %JAVASCRIPT)
    {
        &SetProtoJS($_);
    }
};

%MAP && do
{
    local (@maps);
    foreach (keys %MAP)
    {
        $NewMap{$_} = s/=([^=]*)$// ? $1 : '';
        push(@maps, &Regular($_));
    }
    $Arg{'map'} = $ServerMap = join('|', @maps);

};

!%LOCAL && $LOCAL{'@.map'}++;    # default to *.map

%LOCAL && ($Local = &LinkSet("local", *LOCAL));

%DONT_OUTPUT and ($Dont_Output = join "|", keys %DONT_OUTPUT);

%SKIP && ($Skip = &LinkSet("skip", *SKIP));

$Use_QS = ($Arg{'http'} and not $Arg{no_query_string}) ? 1 : 0;

%LANGUAGE
    and $HTTP_HEADER{ "Accept-Language: " . join ", ", sort keys %LANGUAGE } = 1;

$Arg{'http'} && do
{
    $ServerHost || &Error("must set -host when using -http");
    &HttpInit(*Arg, *DB, *PASSWORD, *HTTP_HEADER, 'spider');
    $Http++;
    ($Arg{'case'} || $Arg{'orphan'})
        && $Hints{"-case and -orphan don't work in -http mode."}++;
};

$DB{1} && do
{
    foreach (sort keys %Arg)
    {
        printf "%10s %s\n", $_, &Abbrev(60, $Arg{$_});
    }
    %PASSWORD || exit;
    print "Passwords:\n";
    foreach (sort keys %PASSWORD)
    {
        print qq~"$_" $PASSWORD{$_}\n~;
    }
    exit;
};

#--------------- Local/Remote Site Checking ---------------------

!@CheckUrls && do
{

    &LogFile($OutLog || $SiteLog);

    &Progress(
        $Http
        ? "\nChecking links via http://$ServerHost"
        : "\nChecking links locally in $ServerRoot"
    );
    &Progress("that match: " . join(" ", keys %LinkSet));
    &Progress(&Plural(scalar keys %Seeds, "%d seed%s: ") . join(" ", sort keys %Seeds));

    $Time = -time;
    &SetBase("/");
    foreach (sort keys %Seeds)
    {
        $_ = &UniqueUrl($_);
        &Progress("\nSeed:    $_");
        &WasCached($_, "\n") || &LinkLint(0, $_, "\n");
    }
    $Time += time;

    $DirList = keys %DirList;

    &CheckOrphan(*DirList, *OrphList, *BadCase, $Arg{'orphan'}, $CheckCase);

    &ProcessLocal;

    &PrintOutput(*SiteForm, *SiteSummary, $DocDir, @SiteHtml);
};

$CheckNet && push(@CheckUrls, grep(m#^http://#i, keys %ExtLink));

@CheckUrls || exit $ErrTot;

#--------------- Remote Url Checking ---------------------

$URL_PREFIX &&= $Arg{url_doc_prefix};
$DocDir and $URL_PREFIX =~ m~/~ and do
{
    $url_doc_dir = "$DocDir/$URL_PREFIX";
    $url_doc_dir =~ s~/[^/]+$~~;
    -d $url_doc_dir
        or mkdir($url_doc_dir, 0777)
        or Error("Could not mkdir($url_doc_dir)");
};

%WarnList = ();

&LogFile($OutLog || $UrlLog);

&HttpInit(*Arg, *DB, *PASSWORD, *HTTP_HEADER, 'checkonly');

$CacheNet && &Http'OpenCache($StatFile);

$Arg{'flush'} && &Http'FlushCache(@CheckUrls);

$Arg{'retry'}
    && !(@CheckUrls = &Http'Recheck(@CheckUrls))
    && die("No urls need to be retried.\n");

%UrlStatus = &Http'CheckURLS(@CheckUrls);
&Http'WriteCaches;

($CheckedUrls = keys %UrlStatus) || exit;

&Http'StatusMsg(*UrlStatus, *UrlOk, *UrlFail, *UrlWarn);

$UrlRetry = &Http'RetryCount(*UrlStatus);

foreach (keys %ExtLink)
{
    $UrlFail{$_} || next;
    $UrlFailX{$_} = $UrlFail{$_};
    $UrlFailXX{"$_&cr;        $UrlFailX{$_}"} = $ExtLink{$_};
}

&HashUnique(*WarnList);
&InvertKeys(*WarnList, *WarnF);

$UrlFailedF = &InvertKeys(*UrlFailXX, *UrlFailF);

&Http'OtherStatus(*UrlMod, *HostFail, *UrlMoved, *UrlRedirect);

&PrintOutput(*UrlForm, *UrlSummary, $DocDir, @UrlHtml);

exit;

###########################################################################
#
# Site Checking Routines
#
###########################################################################

#--------------------------------------------------------------------------
# UniqueUrl($url)
#
#    Make a URL Unique.  Decode "/../" and "." in the path of full URL's
#    Do same for relative URL's But also completes the path
#    if it does not start with a "/".
#--------------------------------------------------------------------------

sub UniqueUrl
{
    local ($_) = @_;
    local ($scheme, $host, $local);

    $DbP && $ErrTag && print $ErrTag;

    s#^([\w\.\-\+]+):## && do
    {    # specified scheme
        ($scheme = $1) =~ tr/A-Z/a-z/;    # lower case scheme
        $scheme =~ /^https?$|^ftp$/ || do
        {
            $DbP && print "==> LINK $scheme:$_\n\n";
            return "$scheme:$_";
        };
    };

    s#^//([^/]*)## && ($host = $1);       # specified host

    ($scheme && $host) || ($scheme = $BaseScheme) && do
    {
        $host || $BasePath || m#^/# || ($host = $_, $_ = '');
        $host || ($host = $BaseHost) && m#^/# || ($_ = $BasePath . $_);
    };

    (($host && $host ne $ServerHost) || ($scheme && $scheme !~ /http/i))
        || do
    {
        m#^/# || ($_ = $CurPath . $_);
        s/\#.*$//;                        # strip local anchor
        $local++;
        $Use_QS or s#\?.*$##;             # strip query string
    };

    m/&/ && do
    {
        s/&amp;/&/g;                      # expand &amp; etc
        s/&lt;/</g;
        s/&gt;/>/g;
        s/&quot;/"/g;
        s/&nbsp;/ /g;
        s/&#(\d\d?\d?);/pack("c",$1)/ge;
    };

    # s/%([0-9a-zA-Z]{2})/pack("c",hex($1))/ge;

    s#\\#/#g && &Warn("\\ converted to / in $_[0]", $link);
    s#//+#/#g && &Warn("// converted to / in $_[0]", $link);

    #----- make path unique by expanding /.. and /.

    m#/\.# && do
    {
        while (s#/\./#/#) { ; }    #  /./        ->  /
        while (s#/[^/]*/\.\./#/#) { ; }    #  /dir/../   ->  /
        s#/.$##;                           # trailing /.
        s#/[^/]*/\.\.$#/#;                 # trailing /dir/../   ->  /
    };

    $local || do
    {
        $scheme = $scheme || 'http';
        $host = $host ? "//$host" : '';
        $DbP && print "==> LINK $scheme:$host$_\n\n";
        return "$scheme:$host$_";
    };

    $IgnoreCase && tr/A-Z/a-z/;
    $DbP && $ErrTag && print "==> LOCAL LINK $_\n\n";
    $_ || '/';
}

#--------------------------------------------------------------------------
# WasCached($link, $referer)
#
#     Does some quick checks on $link.  Return 1 if we are done with it.
#     Return 0 if it should be checked further.  Also bails out early
#     If we know we will not have to process any further.
#--------------------------------------------------------------------------

sub WasCached
{
    local ($link, $referer) = @_;

    $referer ne "\n" && &AppendList(*Forward, $referer, $link);

    $LostFile{$link}
        && (($LostFile{$link} .= "\n$referer"), return '1');

    $FileList{$link} && do
    {
        $Skipped{$link} && ($Skipped{$link} .= "\n$referer");
        $FileList{$link} .= "\n$referer";
        return '1';
    };

    $Action{$link} && return '1';

    $link =~ m#^(\w+):# && do
    {
        &AppendList(*ExtLink, $link, $referer);
        return '1';
    };

    $Ignore && $link =~ m/$Ignore/o && do
    {
        &AppendList(*Ignored, $link, $referer);
        return '1';
    };

    '';
}

#--------------------------------------------------------------------------
# LinkLint($level, $link, $referer)
#
#     $level    keeps track of depth of recursion.
#     $link     is the URL or file to check
#     $referer  is the file that referenced $link.
#     Recursively get all referenced files from a file.
#     NOTE: $link is assumed to be anchored at the server root.
#--------------------------------------------------------------------------

sub LinkLint
{
    local ($level, $link, $referer) = @_;
    local (%newlinks);

    $DbLink && &Progress("getting  $link");

    ($ServerMap || $ServerTilde) && do
    {
        ($link = &MapLink($link, $referer)) || return '';
    };

    $link =
          $Http && $link !~ m/$Local/o
        ? &LinkRemote($link, $referer)
        : &LinkLocal($link, $referer);

    $link || return;

    $Forward{$link} = "\n";    # this primes forward

    #----- recurse into all links found in this file

    foreach $new (keys %newlinks)
    {
        &WasCached($new, $link) || &LinkLint($level + 1, $new, $link);
    }
}

#--------------------------------------------------------------------------
# LinkLocal($link, $referer)
#
#     Does the local equivalent of what the server does.
#--------------------------------------------------------------------------

sub LinkLocal
{
    local ($link, $referer) = @_;
    $link =~ s/\?.*$//;    # strip local queries

    local ($lastdir);      # for directory listings

    -d "$ServerRoot$link" && $link !~ m#/$# && ($link .= '/');

    local ($path) = $link;    # for index files

    if ($link =~ m#/$#)
    {

        if (&LookupDir($link))
        {
            $path = &LookupDir($link);
            $PrintAddenda{$link} = "[file: $path]";
        }
        else
        {
            $LASTDIR || do
            {
                &AppendList(*LostFile, $link, $referer);
                return '';
            };
            $lastdir = '1';
            $Arg{no_warn_index} or &Warn("index file not found", $link);
            $PrintAddenda{$link} = "[directory listing]";
        }
    }
    elsif (-f _ )
    {
        ((stat(_))[2] & 4 == 0) && &Warn("not world readable", $link);
    }
    else
    {
        &AppendList(*LostFile, $link, $referer);

        $ServerTilde
            || $link =~ m#(^/~[^/]*)# && $Hints{qq~use -http to resolve "$1" links.~}++;

        $ServerMap
            || $link =~ /^($StandardMaps)/o && $Hints{qq~use "-map $1" to resolve imagemaps.~}++;

        return '';
    }

    &AppendList(*FileList, $link, $referer);
    &CacheDir($link);

    $lastdir && do
    {
        &StopRecursion($link, $referer) && return '';
        &Progress("checking $link");
        %newlinks = %LASTDIR;
        return $link;
    };

    $path =~ /\.($HtmlExts|map)$/io || return '';    # only parse html & .map

    &StopRecursion($link, $referer) && return '';

    &Progress("checking $link");

    open($path, "$ServerRoot$path") || do
    {
        &Warn(qq~could not open file: "$ServerRoot$path"\n~, 'sys');
        return '';
    };

    $path =~ /\.map$/i
        ? &ParseMap($path, *newlinks)
        : &ParseHtml($path, $link, *newlinks);

    close($path);
    $link;
}

#--------------------------------------------------------------------------
# MapLink($link, $referer)
#
#     Resolves my server maps for $link.  Returns new $link or '' if
#     the new link was already cached.
#--------------------------------------------------------------------------

sub MapLink
{
    local ($link, $referer) = @_;
    local (%checked, $old);
    $old = $link;
    while (($ServerMap && $link =~ s#^($ServerMap)#$NewMap{$1}#o)
        || ($ServerTilde && $link =~ s#^/~([^/]*)#$ServerTilde#oee))
    {

        $checked{$link}++ || next;
        &Warn("infinite mapping loop", $link);
        return $link;
    }

    ($old eq $link || "$old/" eq $link) && return $link;

    $PrintAddenda{$link} = "($old)";
    $Mapped{$old} = $link;
    $DbLink && &Progress("mapped $old\n    => $link");
    &WasCached($link, $referer) && return '';
    $link;
}

#--------------------------------------------------------------------------
# StopRecursion($link, $referer)
#
#      Stops recursion as needed.  Also records skipped files.
#--------------------------------------------------------------------------

sub StopRecursion
{
    local ($link, $referer) = @_;

    $Parsed{$link} && return '1';

           $Abort
        || $link !~ /$LinkSet/o
        || ($Skip && $link =~ /$Skip/o)
        || ++$Parsed > $Limit
        || do
    {
        $Parsed{$link}++;
        return '';
    };

    &AppendList(*Skipped, $link, $referer);
    push(@Skipped, $link);
    &Progress("-----    $link");
    return '1';
}

#--------------------------------------------------------------------------
# LinkRemote($link, $referer)
#
#     Checks $link via http.  If it is an html file it is parsed and
#     the results go into local lists maintained by LinkLink().
#--------------------------------------------------------------------------

sub LinkRemote
{
    local ($oldlink, $referer) = @_;

    #---- check url and parse into local arrays in LinkLint().

    $Fetched{$oldlink}++ && return '';

    ($flag, $link) = &Http'Parse($ServerHost, $oldlink, $referer, *newlinks);

    $flag == -5000 && return '';    # user interrupt

    $flag == -4000 && do
    {                               # moved to different host
        &AppendList(*ExtLink, $link, $referer);
        return '';
    };

    $link ne $oldlink && $link ne "$oldlink/" && do
    {
        $PrintAddenda{$link} = "($oldlink)";
        $Mapped{$oldlink} = $link;
    };

    $flag || return '';             # new url was already cached

    local ($msg) = &Http'ErrorMsg($flag);

    &Http'FlagWarn($flag) && do
    {
        &AppendList(*Ignored, $link, $referer);
        &AppendList(*HttpFail, $link, $msg);
        &Warn($msg, $link);
        return '';
    };

    &Http'FlagOk($flag) || do
    {
        &AppendList(*LostFile, $link, $referer);
        &AppendList(*HttpFail, $link, $msg);
        return '';
    };

    &AppendList(*HttpOk, $link, $msg);

    $flag == -2000 || return '';

    $link;
}

#--------------------------------------------------------------------------
# CacheDir($link)
#
#    Save a list of directories for orphan and case checking.
#--------------------------------------------------------------------------

sub CacheDir
{
    local ($dir) = @_;
    $dir =~ s#/[^/]*$##;
    local ($absdir) = $ServerRoot . $dir;
    $dir = $dir || '(root)';
    ($DirList{$dir} || $LostDir{$dir}) && return;
    &AppendList(-d $absdir ? *DirList : *LostDir, $dir, "\n");
}

#--------------------------------------------------------------------------
# ParseHtml(*HANDLE, $link, *list)
#
#     Extracts all (?) links from the file by setting %list{link} = "1".
#     Links are expanded to full unique URL's or paths.
#     %Anchor    named anchors found
#     %ImgMap    named image maps found
#     %WantAnch  named anchors to find
#     %WantMap   named image maps to find
#--------------------------------------------------------------------------

sub ParseHtml
{
    local (*HANDLE, $link, *list) = @_;
    local ($code, $tag, $temp, $url, $att, $term, $anch);

    &SetBase($link);

    $DbP && print "\n", '=' x 60, "\nFILE     $link\n\n";

    $/ = "<";    # use "<" as newline separator

TAG:
    while (<HANDLE>)
    {

        /^\!\-\-/ && do
        {
            while ($_ !~ /\-\-\!?>/)
            {    # ignore tags inside comments
                defined($_ = <HANDLE>) && next;
                &Warn(q~missing end comment "-->"~, $link);
                last TAG;
            }
            next TAG;
        };

        m/^(\w+)(\s*("[^"]*"|'[^']*'|[^>"'])*)(>?)/ || next;

        $tag = $1;
        $att = $2;
        $term = $4;

        while (!$term)
        {
            $att .= $_;
            ($term, $att) = &FixTag(*HANDLE, $att);
            $term eq 'next' && next TAG;
            $term eq 'last' && last TAG;
        }

        ($_ = $tag) =~ tr/A-Z/a-z/;    # convert tag to lower case

        if (/^script$/)
        {
            $att =~ /\ssrc${ATTRIB}/io && $list{ &UniqueUrl($+) }++;
            while ($_ = <HANDLE>) { m#^/script#i && next TAG; }
            &Warn("missing </script>", $link);
            last TAG;
        }

        $att || next TAG;

        $DbP && $att && do
        {
            ($ErrTag = "<$tag$att>") =~ s/\s+/ /;
            $ErrTag .= "\n";
        };

        if (/^a$/)
        {
            $att =~ s/\sname${ATTRIB}//io
                && !$Arg{no_anchors}
                && ($Anchor{"$CurFile#$+"} = "\n");

            $att =~ /\shref${ATTRIB}/io || next;

            $temp = $+;
            $temp =~ /^#/ && ($temp = "$BaseFile$temp");

            $anch = $temp =~ /(#.*)$/ ? $1 : '';
            $url = &UniqueUrl($temp);
            $list{$url}++;

            %ProtoJS && $url =~ /^javascript:/ && &ProtoJS($url, $referer);

                   $anch
                && $url =~ m#^/#
                && !$Arg{no_anchors}
                && &AppendList(*WantAnch, "$url$anch", $link);
        }
        elsif (/^base$/)
        {
            $att =~ /\shref${ATTRIB}/io || next;
            $DbP && print $ErrTag;
            &BaseTag($+);
        }
        elsif (/^bgsound$|^frame$|^input$|^embed$/)
        {
            $att =~ /\ssrc${ATTRIB}/io && $list{ &UniqueUrl($+) }++;
        }
        elsif (/^area$/)
        {
            $att =~ /\shref${ATTRIB}/io && $list{ &UniqueUrl($+) }++;
        }
        elsif (/^body$/)
        {
            $att =~ /\sbackground${ATTRIB}/io && $list{ &UniqueUrl($+) }++;
        }
        elsif (/^img$/)
        {
            $att =~ s/\ssrc${ATTRIB}//io && $list{ &UniqueUrl($+) }++;
            $att =~ s/\slowsrc${ATTRIB}//io && $list{ &UniqueUrl($+) }++;
            $att =~ /\sdynsrc${ATTRIB}/io && $list{ &UniqueUrl($+) }++;
            $att =~ /\susemap${ATTRIB}/io || next;

            $temp = $+;
            $temp =~ /^#/ && ($temp = "$BaseFile$temp");

            $anch = $temp =~ /(#.*)$/ ? $1 : '';
            $url = &UniqueUrl($temp);
            $list{$url}++;

            $anch
                && $url =~ m#^/#
                && &AppendList(*WantMap, "$url$anch", $link);
        }
        elsif (/^map$/)
        {
            $att =~ s/\sname${ATTRIB}//io
                && ($ImgMap{"$CurFile#$+"} = "\n");
        }
        elsif (/^form$/)
        {
            $att =~ /\saction${ATTRIB}/io || next;
            $temp = &UniqueUrl($+);
            &AppendList(*Action, $temp, $link);
            $list{$temp}++;
        }
        elsif (/^applet$/)
        {
            $att =~ /\scode${ATTRIB}/io or next;
            $code = $+;
            $code =~ /\.class$/i || ($code .= ".class");

            my ($jar, $base);
            $att =~ /\sarchive${ATTRIB}/io and $jar = $+;
            my $file = $jar || $code || next;

            $att =~ /\scodebase${ATTRIB}/io and do
            {
                ($base = $+) =~ s#/$##;
                $file = "$base/$file";
            };
            $list{ &UniqueUrl($file) }++;
        }
        elsif (/^meta$/)
        {
            $att =~ /\shttp-equiv${ATTRIB}/io || next;
            $+ =~ /^refresh$/i || next;
            $att =~ /\scontent\s*=\s*"([^"]*)"/i || next;
            $1 =~ /url=([^"\s]+)/i || next;
            $temp = $1;
            $temp =~ m#^\w+://#
                || &Warn("re-direct should be absolute", $link);
            $url = &UniqueUrl($temp);
            $list{$url}++;
        }
    }

    $/ = "\n";    # reset line seperator to "\n"
    $DbP or return;
    print '=' x 60, "\n\n";
    $ErrTag = '';
}

#--------------------------------------------------------------------------
# SetBase($link)
#
#    Clears globals: BaseScheme, BaseFile, BasePath,
#    Sets CurFile and CurPath
#--------------------------------------------------------------------------

sub SetBase
{
    ($CurFile) = @_;
    $BaseScheme = '';
    $BaseHost = '';
    $BasePath = '';
    $BaseFile = $CurFile;
    ($CurPath = $CurFile) =~ s#(\?.*)$##;    # strip query off of path
    $CurPath =~ s#([^/]+)$##;                # strip file off of path
    $CurPath = $CurPath || "/";              # default to root
}

sub ProtoJS
{
    local ($link, $referer) = @_;
    local ($place, %place, $cnt);
    $link =~ s#javascript:([^\(\)]+)\(## || return;
    ($place = $ProtoJS{$1}) || return;
    grep($place{$_}++, split("\n", $place));
    while ($link =~ s/^\s*("([^"]*)"|'([^']*)'|([^"'\)]+))\s*[,\)]//)
    {
        $place{ ++$cnt } && $list{ &UniqueUrl($+) }++;
    }
}

sub SetProtoJS
{
    local ($proto) = @_;
    local (@place, $name, $cnt);
    $proto =~ s#^([^\(\)]+)\(## || return;
    $name = $1;
    while ($proto =~ s#^([^\),]*)[,\)]##)
    {
        $cnt++;
        $+ eq 'url' && push(@place, $cnt);
    }
    $ProtoJS{$name} = join("\n", @place);
    $DB{1} && print "javascript:$name $ProtoJS{$name}\n";
}

#--------------------------------------------------------------------------
# FixTag(*HANDLE, $att)
#
#    Reads in the remainder of a tag if there was a  bare "<" inside the
#    the tag.  Only works if "<" was inside of single or double quotes.
#    This is slow but we only get here on rare occasions.
#--------------------------------------------------------------------------

sub FixTag
{
    local (*HANDLE, $att) = @_;
    local ($erratt, $temp, $quot);

    $DbP && print "attrb = [$att]\n";

    ($erratt = substr($att, 0, 20)) =~ s/\s+/ /;    # for error msg
    $temp = $att;
    $temp =~ s/"[^"]*"|'[^']*'//g;                  # strip leading ".."

    $DbP && print "tail  = [$temp]\n";

    $temp = m/(['"])/ || do
    {                                               # should have ' or "

        &Warn(qq~unquoted "<" in <$tag$erratt~, $link);
        return 'next';
    };

    $quot = $1;                                     # last ' or "
    $DbP && print "quot  = [$quot]\n";

    $_ = '';                                        # prime the pump
    do
    {
        $att .= $_;
        $DbP && print "append1 [$_]\n";             # add new lines ...
        defined($_ = <HANDLE>) || do
        {
            &Warn("unterminated <$tag$erratt", $link);
            return 'last';
        };

        } until (
        ($quot eq '"' && s/^([^"]*")//) ||          # until we close quote
            ($quot eq "'" && s/^([^']*')//)
        );

    $att .= $1;                                     # got it
    $DbP && print "append2 [$1]\n";

    m/^(("[^"]*"|'[^']*'|[^>])*)(>?)/ && ($att .= $1);

    $DbP && print "append3 [$1]\n";

    $term = $3;
    $DbP && print "term  = [$term]\n";
    !$term && ($att .= $_);
    ($term, $att);
}

#--------------------------------------------------------------------------
# BaseTag($url)
#
#     Set Global $BaseHost, $BasePath and $BaseFile defined in $url
#     Only set BaseHost if a scheme is given!
#     $BasePath will always start and end with "/"
#--------------------------------------------------------------------------

sub BaseTag
{
    local ($_) = @_;

    $BaseFile = $_;

    s#^([\w\-]+):## && do
    {
        ($BaseScheme = $1) =~ tr/A-Z/a-z/;
        $BaseFile = "$BaseScheme:$_";
        $BaseHost = $1 if s#^//([^/]*)##;    # can't have host without scheme
    };
    s#([^/]+)$##;                            # strip file first
    $BasePath = $_ if m#^/#;                 # only if absolute

#    $ServerHost && $BaseFile =~ s#^http://$ServerHost##o;

    $DbP || return;
    print "\nBaseScheme $BaseScheme\n";
    print "BaseHost   $BaseHost\n";
    print "BasePath   $BasePath\n";
    print "BaseFile   $BaseFile\n\n";
}

#--------------------------------------------------------------------------
# ParseMap(*HANDLE, *list)
#     Reads a map file and tries to extract all http links.
#--------------------------------------------------------------------------

sub ParseMap
{
    local (*HANDLE, *list) = @_;
    &SetBase("/");
    while (<HANDLE>)
    {
        next unless m#(http://[^\s"]+)#i;    # strip any junk around an http://
        $list{ &UniqueUrl($1) }++;
    }
}

#--------------------------------------------------------------------------
#  $url = ParseRedirect(*HANDLE, $link)
#
#     Reads text from FILE until end of <head> element.  Uses $link for
#     error messages.  Returns $url with redirected $url if given otherwise
#     returns ''.
#--------------------------------------------------------------------------

sub ParseRedirect
{
    local (*HANDLE, $link) = @_;

    &SetBase($link);

    local ($url) = '';
    $/ = "<";    # use "<" as newline separator

REDIR:
    while (<HANDLE>)
    {

        $DbP && print;

        /^\!\-\-/ && do
        {
            while ($_ !~ /\-\-\!?>/)
            {    # ignore tags inside comments
                defined($_ = <HANDLE>) && next;
                &Warn(q~missing end comment "-->"~, $link);
                last REDIR;
            }
            next REDIR;
        };

        last if m#^(/head|body|h\d|font)#i;
        s/^meta//i || next;
        /\shttp-equiv\s*=\s*"?refresh/i || next;
        /\scontent\s*=\s*"([^"]*)"/i || next;
        $1 =~ /url\s*=\s*([^"\s]+)/i || next;
        $url = &UniqueUrl($1);
        $url =~ m#^\w+://#
            || &Warn("re-direct $url should be absolute", $link);
        last;
    }
    $/ = "\n";    # use "\n" as newline separator
    return $url;  # return value
}

#--------------------------------------------------------------------------
# $path = LookupDir($dir)
#
#     $dir and $path are both relative to server root.
#     Find a default index file in $dir. Returns $path of default file
#     on success or return 0 on failure. Caches results in $DefDir.
#     Fills %LASTDIR with last successful directory listing.
#--------------------------------------------------------------------------

sub LookupDir
{
    local ($absdir) = @_;
    $absdir =~ s#/$##;
    local ($dir) = $absdir . '/';

    defined $DefDir{$dir} && return $DefDir{$dir};    # was cached

    local (%file, $lc);

    &Progress("looking for $dir(default)");

    opendir(DIR, "$ServerRoot$absdir")
        || return $DefDir{$dir} = $LASTDIR = '';

    %LASTDIR = ();
    foreach (grep(!/^\./, readdir(DIR)))
    {                                                 # all files in directory

        ($lc = $_) =~ tr/A-Z/a-z/;                    # lower case version
        $IgnoreCase && ($_ = $lc);
        $LASTDIR{"$dir$_"}++;
        $file{ $DefCaseSens ? $_ : $lc } = $_;
    }

    closedir(DIR);
    $LASTDIR = '1';

    foreach (@DefIndex)
    {
        next unless $file{$_} && -f "$ServerRoot$dir$file{$_}";
        return $DefDir{$dir} = "$dir$file{$_}";
    }
    return $DefDir{$dir} = '';
}

#--------------------------------------------------------------------------
# CheckOrphan(*dirlist, *orphlist, *badcase, $checkorphan, $checkcase)
#
#     Checks every directory in dirlist and creates a list of
#     all files that have not been checked by linklint.
#
#     If CheckCase is set we first check the case of all found files
#     in that directory.
#--------------------------------------------------------------------------

sub CheckOrphan
{
    local (*dirlist, *orphlist, *badcase, $checkorphan, $checkcase) = @_;

    %dirlist || return;
    $checkorphan || $checkcase || return;

    local ($msg) = '    for';
    $msg .= " orphans" if $checkorphan;
    $msg .= " and" if $checkorphan && $checkcase;
    $msg .= " case mismatch" if $checkcase;

    local (@files, %files, $file, $link, $absdir, $reldir);

    foreach $dir (sort keys %dirlist)
    {

        &Progress("checking $dir");
        &Progress($msg);

        $reldir = $dir eq '(root)' ? '' : $dir;
        $absdir = $ServerRoot . $reldir;

        &PushDir($absdir) || next;
        opendir(DIR, '.') || do
        {
            &Warn(qq~could not read directory "$absdir"~, 'sys');
            next;
        };

        @files = grep(!/^\./, readdir(DIR));
        $IgnoreCase && grep(tr/A-Z/a-z/ && 0, @files);
        closedir(DIR);

        $checkcase && do
        {

            %files = ();
            grep($files{"$reldir/$_"}++, @files);

            foreach $link (grep(m#^$reldir/[^/]+$#i, keys %FileList))
            {
                next if $files{$link};
                ($file) = grep(/$link/i, keys %files);    # get "real" filename
                $file = $file || $link;                   # just in case
                $badcase{$file} = $FileList{$link};       # add to list
            }
        };
        next unless $checkorphan;

        foreach $file (@files)
        {
            $link = "$reldir/$file";
            next
                if $FileList{$link}
                || $dirlist{$link}
                || $badcase{$link};

            -d $file && ($link .= "/");    # let them know it's a dir
            $orphlist{$link} = "\n";

            next unless $link =~ /\.($HtmlExts)$/io;

            #----- parse html files for possible redirects

            open($file, $file) || do
            {
                &Warn(qq~could not open orphan "$link"~, 'sys');
                next;
            };
            local ($equiv) = &ParseRedirect($file, $link);
            close($file);
            $equiv && ($PrintAddenda{$link} = "  => $equiv");
        }
        &PopDir;
    }
}

#--------------------------------------------------------------------------
# ProcessLocal()
#
#     Does the work needed between gathering links and printing.
#--------------------------------------------------------------------------

sub ProcessLocal
{
    local ($file, $anch, $ref);

    &Progress("\nProcessing ...");

    #---- Resolve named anchors

    $Arg{no_anchors} or do
    {
        &HashUnique(*WantAnch);
        &ResolveAnch(*WantAnch, *Anchor, *LostAnch);
    };

    #---- resolve named image maps

    &HashUnique(*WantMap);
    &ResolveAnch(*WantMap, *ImgMap, *LostMap);

    &HashUnique(*FileList);    # pathinfo and dirlookup can cause extras.
    &HashUnique(*LostFile);    # pathinfo and dirlookup can cause extras.
    &HashUnique(*Action);
    &HashUnique(*Forward);

    &InvertKeys(*LostFile, *ErrF);
    &InvertKeys(*BadCase, *CaseF);
    &InvertKeys(*WarnList, *WarnF);

    $Parsed = keys %Parsed;

    $Parsed >= $Limit
        && !$Arg{'limit'}
        && $Hints{"use -limit to check more than $Limit files."}++;

    @PRINTFORM = @SiteForm;
    &PrintFiles(0, 0, 0, 0);    # count elements in local arrays

    $ErrTot = $LostFile + $LostAnch + $BadCase;
    $Fetched = keys %Fetched;
}

###########################################################################
#
#                          PRINTING ROUTINES
#
# Entry through PrintOutput() function.
#
###########################################################################

#--------------------------------------------------------------------------
# PrintOutput(*form, *summary, $docdir, $sumfile, $indexfile, $title)
#
#     A wrapper for a printing routines. Computes counts of # of elements
#     in all hashes (need scalars).  Inverts some hashes for forward
#     results. Prints output to files in $DocDir or to "std" output.
#--------------------------------------------------------------------------

sub PrintOutput
{
    local (*printform) = shift;
    @PRINTFORM = @printform;    # used globally below
    local (*SUMMARY) = @_;      # Summary routine: used and passed on.

    &PrintFiles(0, 0, 0, 0);    # count elements in all hashes

    &PrintDocDir(@_);           # All -doc files

    $LogFile && do
    {
        print "\n";
        &PrintFiles(@SumPrint);    # summary list of results
        print "\n";
        &SUMMARY;                  # text summary at top of file zz ?
        print "\n";
    };

    &PrintFiles(0, 0, 0, $PrnFlag);    # print as user requested

    return if $Silent;                 # -silent: no summary on screen

    $lastsel = select(STDERR);
    print "\n";
    &PrintFiles(@SumPrint);            # list summary
    print "\n";
    &SUMMARY;                          # text summary at bottom of screen
    select($lastsel);
}

#--------------------------------------------------------------------------
# PrintFiles($DocDir, $DOCHEAD, $SUM, $flags)
#
#     Prints all or part of the output according to the flags supplied.
#     If $DocDir is supplied output goes to the files specified in the
#     OpenDoc() calls, otherwise output goes to currently selected output.
#     PrintFile is called several times to print lists to STDOUT,
#     doc files, summary, and summary file. Passes globals to OpenDoc(),
#     PrintLISTS() and PrintList().
# where to print:
#     $DocDir    send output to seperate files in $DocDir.
# summarize:
#     $DOCHEAD   add filenames to headers.
#     $SUM       1: summary form, 2: summarize printLISTS to one line
#
#--------------------------------------------------------------------------

sub PrintFiles
{
    local ($docdir, $DOCHEAD, $SUM, $flags) = @_;
    local ($file, $data, $mask, $prog, @params);
    foreach (@PRINTFORM)
    {
        ($file, $data, $mask, $prog, @params) = split(/;\s*/, $_);
        $Dont_Output and $file =~ m/$Dont_Output/o and next;
        $mask = oct($mask);
        $flags || (eval("\$$data = \$$data || keys \%$data"), next);
        &OpenDoc($docdir, $file, $data) || next;
        next unless ($flags & $mask) == $mask;
        $prog == 1 && &PrintList($data, @params);
        $prog == 2 && &PrintLISTS($data, @params);
        $prog == 3 && &PrintUrl($data, @params);
    }
}

#--------------------------------------------------------------------------
# $flag = OpenDoc($docdir, $file, *list)
#
#     A co-conspirator with PrintFiles().
#     $file is the name of a file (sans extension) to be written in
#     $DocDir.  $anydata tells us if there is any data to be written
#     to the file.  Always erase old copies of all the files but save
#     a backup copy if the file might be theirs.
#     If $DocDir is 0 then create the filename $DOCFILE and return
#     $anydata. If $anydata is 0 don't create a new file and return ''.
#--------------------------------------------------------------------------

sub OpenDoc
{
    local ($docdir, $name, *data) = @_;
    local ($anydata) = $data || ($data = scalar keys %data);
    $DOCFILE = "$name.txt";    # global used in PrintList()
    return $anydata unless $docdir;    # and output will to current select()

    local ($htmlfile) = $name;
    $htmlfile .= '.html';

#    $Clean && do {
    -e $DOCFILE && unlink($DOCFILE);
    -e $htmlfile && unlink($htmlfile);

#    };

    return '' unless $anydata;         # don't create new file w/ no data

##    print STDERR ">>>$DOCFILE\n";

    open(DOC, ">$DOCFILE") || do
    {
        &Warn(qq~could not open "$DOCFILE" for output~, 'sys');
        return '';
    };

    $DOCFILES++;                       # count files created
    $DOCFILES{$DOCFILE} = $htmlfile;   # keep a list of .txt files
    select(DOC);                       # print's will default to DOC
    print "file: $DOCFILE\n";          # indentify filename
    print &Preamble;                   # print a file header
}

sub Preamble
{
    $TimeStr = $TimeStr || &TimeStr('(local)');

    join('', $HeaderRoot ? "root: $HeaderRoot\n" : '', $ServerHost ? "host: $ServerHost\n" : '', "date: $TimeStr\n", "Linklint version: $version\n", "\n");
}

#--------------------------------------------------------------------------
# SiteSummary
#
#     Prints the textual summary of what has happened.
#--------------------------------------------------------------------------

sub SiteSummary
{
    %Hints && print "hint: ", join("\nhint: ", keys %Hints), "\n\n";

    $FileList && print
        "Linklint found ",
        &Plural($FileList, "%d file%s"),
        $DirList ? &Plural($DirList, " in %d director%y") : '',
        $Parsed ? &Plural($Parsed, " and checked %d html file%s") : '',
        ".\n";

    $CheckCase
        && $BadCase == 0
        && print &Plural($BadCase, "There %w %n file%s with mismatched case.\n");

    $CheckOrphan
        && $OrphList == 0
        && print &Plural($OrphList, "There %w %n director%y with orphans.\n");

    print &Plural($LostFile, "There %w %n missing file%s.");
    print &Plural($ErrF, " %N file%s had broken links.\n");

    print &Plural($ErrTot, "%N error%s, ");
    print &Plural($WarnList, "%n warning%s.");

    $Http
        || $Time && $Parsed && $Time > 4 && printf("  Parsed ~ %1.1f files/second.", $Parsed / $Time);

    print "\n";
}

#--------------------------------------------------------------------------
# PrintDocDir(*SUMMARY, $docdir, $sumfile, $indexfile, $title)
#
#     Prints complete documentation in $DocDir directory.
#--------------------------------------------------------------------------

sub PrintDocDir
{
    local (*SUMMARY, $docdir, $sumfile, $indexfile, $logfile, $title) = @_;
    $docdir && &PushDir($docdir) || return;

    local ($indexhtml) = $indexfile;
    $indexhtml .= ".html";

    $DOCFILES = 0;
    %DOCFILES = ();

    &Progress("\nwriting files to $Arg{'doc'}");    # abbreviated $DocDir

    local ($lastselect) = select;                   # save "std" output

    &PrintFiles($docdir, 0, 0, $DocFlag);           # print almost all doc files

    local ($dum) = '1';                             # used to fool OpenDoc()

    &OpenDoc($docdir, $sumfile, *dum) && do
    {
        &SUMMARY;                                   # text summary at top of file
        print "\n";
        &PrintFiles(@SumPrint);                     # list summary
    };

    &OpenDoc($docdir, $indexfile, *dum) && do
    {
        printf("%12s: %s\n", "$sumfile.txt", 'summary of results');
        $Arg{'out'} || printf("%12s: %s\n", "$logfile.txt", 'log of progress');
        &PrintFiles('', 1, 2, $DocFlag);            # list index to all files
    };

    close(DOC);                                     # close the last one
    select($lastselect);                            # restore "std" output

    &Progress(&Plural($DOCFILES, "wrote %n txt file%s"));

    $Arg{'textonly'} || do
    {

        delete $DOCFILES{"$sumfile.txt"};
        delete $DOCFILES{"$indexfile.txt"};
        delete $DOCFILES{"dir.txt"};

        foreach $txt (keys %DOCFILES)
        {
            &HtmlDoc($txt, $DOCFILES{$txt}, *DefDir, *Action, $DocBase);
            $Arg{'htmlonly'} && $txt !~ /^remote/ && unlink($txt);
        }

        &Progress(&Plural(scalar keys %DOCFILES, "wrote %n html file%s"));

        &HtmlSummary("${indexfile}.txt", $indexhtml, $title, *DOCFILES)
            && &Progress("wrote index file $indexhtml");
        $Arg{'htmlonly'} && unlink("${indexfile}.txt");
    };

    &PopDir;
}

#--------------------------------------------------------------------------
# HtmlDoc($in, $out, *map, *skip, $base)
#
#     Reads txt file $in, writes html file $out, adding anchors
#     to lines that start with "/" or "scheme:".  Sets the base
#     to $base if provided, maps links found in %map, does not
#     add anchors to links found in *skip.
#--------------------------------------------------------------------------

sub HtmlDoc
{
    local ($in, $out, *map, *skip, $base) = @_;
    local ($title) = "Linklint - $out";

    #---- Open files

    open(IN, $in) || do
    {
        &Warn(qq~\ncould not read back "$infile"~, 'sys');
        return '';
    };

###    print STDERR ">> HTML >> $out\n";

    open(OUT, ">$out") || do
    {
        &Warn(qq~\ncould not open "$outfile" for output~, 'sys');
        close(IN);
        return '';
    };

    my $base_tag = $Arg{output_frames} ? "<base target=LinkLint_site>" : "";

    #----- print html header

    print OUT "<html><head><title>\n$title\n</title>\n";
    print OUT $base_tag;
    print OUT "</head><body>\n\n<pre>\n";

    #---- read/print text file header

    $_ = <IN>;    # skip "file: filename.txt"

    while (<IN>)
    {
        print OUT $_;
        last unless /\S/;
    }

    #---- read/print out the list with anchor tags

    local ($file);

    while (<IN>)
    {
        s/^(\s+)// && print OUT $1;
        s/^(=>\s*)// && print OUT $1;
        m#^(/|\w+:).*# || do { print OUT $_; next; };
        s/\s+$//;
        $file = $map{$_} || $_;
        $skip{$file} && do { print OUT "$_\n"; next; };
        $file =~ m#^/# && ($file = $base . $file);
        print OUT qq~<a href="$file">$_</a>\n~;
    }

    close(IN);

    print OUT @tab, "</pre></body></html>\n";
    close(OUT);

    return 1;
}

#--------------------------------------------------------------------------
# TimeStr($gmt, $time)
#
#     Returns a string formated "weekday, month day year hh:mm:ss"
#--------------------------------------------------------------------------

sub TimeStr
{
    local ($gmt, $time) = @_;
    defined $time || ($time = time);
    local ($sec, $min, $hour, $mday, $mon, $year, $wday) =
        ($gmt =~ m/GMT/) ? gmtime($time) : localtime($time);

    local (@mon) = ('Jan', 'Feb', 'Mar', 'Apr', 'May', 'Jun', 'Jul', 'Aug', 'Sep', 'Oct', 'Nov', 'Dec');
    local (@wday) = ('Sun', 'Mon', 'Tue', 'Wed', 'Thu', 'Fri', 'Sat');

    $year < 50 && ($year += 2000);
    $year < 1900 && ($year += 1900);

    sprintf("$wday[$wday], %02d $mon[$mon] $year %02d:%02d:%02d %s", $mday, $hour, $min, $sec, $gmt);
}

#--------------------------------------------------------------------------
# HtmlSummary($infile, $outfile, $title, $docfiles)
#
#     Creates a simple html summary file by reading back the linklint.txt
#     file that was already created.
#--------------------------------------------------------------------------

sub HtmlSummary
{
    local ($infile, $outfile, $title, *docfiles) = @_;

    local (%head);
    open(IN, $infile) || do
    {
        &Warn(qq~\ncould not read back "$infile"~, 'sys');
        return '';
    };

    open(OUT, ">$outfile") || do
    {
        &Warn(qq~\ncould not open "$outfile" for output~, 'sys');
        close(IN);
        return '';
    };

    while (<IN>)
    {
        last unless /\S/;
        /\s*(\S+):\s*(\S+.*\S+)\s*$/ && ($head{$1} = $2);
    }

    local ($title2) = $title;
    $head{'root'} && ($title2 .= "for $head{'root'}");
    my $target = $Arg{output_frames} ? "<base target=LinkLint>" : "";

    print OUT qq~<html><head><title>\n
$title\n
</title>

$target

</head><body>
<font size=4>\n
$title2<br>\n
</font>\n
$head{'date'}<br>
Linklint version: $version
<pre>~;

    local ($file);
    while (<IN>)
    {
        last unless /\S/;
        s|(ERROR )|<font color=#cc0000>$1</font>|;
        s|^\s*(\S+):||;
        $file = $docfiles{$1} || $1;
        $file =~ s~[^/]+/~~;

        print OUT " " x (13 - length($file));
        print OUT "<a href=$file>$file</a>:", $_;
    }

    close(IN);

    print OUT @tab, "\n\n</pre></body></html>\n";

    close(OUT);
    return 1;
}

#--------------------------------------------------------------------------
# Plural($cnt,$msg)
#
#     Returns a pluralized version of $msg.
#
#     %w  -> was : were           %d ->   $cnt
#     %s  ->     :    s           %n ->  no : $cnt
#     %y  ->   y :  ies           %N ->  No : $cnt
#     %es ->     :   es
#--------------------------------------------------------------------------

sub Plural
{
    local ($cnt, $_) = @_;
    $cnt == 1 ? s/\%w/was/g : s/\%w/were/g;    # %w -> 'was' or 'were'
    $cnt == 1 ? s/\%s//g : s/\%s/s/g;          # %s -> ''    or 's'
    $cnt == 1 ? s/\%y/y/g : s/\%y/ies/g;       # %y -> 'y'   or 'ies'
    $cnt == 1 ? s/\%es//g : s/\%es/es/g;       # %y -> 'y'   or 'ies'
    s/\%n/\%d/ && ($cnt = $cnt || 'no');       # %n -> "no"  or $cnt
    s/\%N/\%d/ && ($cnt = $cnt || 'No');       # %N -> "No"  or $cnt
    s/\%d/$cnt/;                               # %d -> $cnt
    s/(\%\d+d)/sprintf($1,$cnt)/e;             # %3d -> $cnt
    return $_;
}

#--------------------------------------------------------------------------
# PrintList(*list, $header, $xref, $subhead)
#
#     Print out keys (and values if $xref) of %list.  Prepend "$DOCFILE: "
#     to header if $DOCHEAD is set.  Append "(cross referenced)" to header
#     if $xref == 2.  Only print header if $SUM is set.
#--------------------------------------------------------------------------

sub PrintList
{
    local (*list, $header, $xref, $subhead) = @_;
    $subhead = $subhead || "used in %d file%s:";
    return unless %list;
    local (@major) = sort keys %list;

    local ($headtext) = &Plural(scalar @major, $header);
    $xref == 2 && ($headtext .= " (cross referenced)");
    $DOCHEAD && ($headtext = sprintf("%13s: ", $DOCFILE) . $headtext);

    $SUM && do { print "$headtext\n"; return; };

    print "$Headline# $headtext\n$Headline";

    foreach (@major)
    {
        s/&cr;/\n/g;
        print "$_\n";
        $PrintAddenda{$_} && print "$PrintAddenda{$_}\n";
        $xref && &PrintSubList($list{$_}, $subhead, 4);
    }
    print "\n" unless $xref;
}

#--------------------------------------------------------------------------
# PrintSubList($sublist, $subhead, $indent)
#
#     Prints out all elements of $sublist split by "\n".  Prints out the
#     number of elements in pluralized $subhead.
#--------------------------------------------------------------------------

sub PrintSubList
{
    local ($sublist, $subhead, $indent) = @_;
    $indent = " " x $indent;
    $sublist =~ s/^\n+//;
    $sublist || return;
    (local (@items) = sort split(/\n+/, $sublist)) || return;
    grep(s/&cr;/\n/g, @items);
    print $indent , &Plural(scalar @items, $subhead), "\n", $indent;
    print join("\n$indent", @items), "\n\n";
}

#--------------------------------------------------------------------------
# PrintLISTS(*list, *heads, $xref)
#
#     Split %list into sublists and then prints each sublist.  The
#     splitting is controlled by @heads. Each line of @heads must be in the
#     form "$heading::$regexp"  All keys of %list that match %regexp are
#     printed out under the heading $heading.  If regexp contains 'unknown'
#     then all remaining items in %list are printed out under its $heading.
#     If $SUM == 2 then a summary of all keys %list is printed using the
#     first heading format which is otherwise ignored.
#--------------------------------------------------------------------------

sub PrintLISTS
{
    local (*listname, *heads, $xref) = @_;
    local (%sublist, @items, $heading, $regexp);
    local (%list) = %listname;

    foreach ($SUM == 2 ? @heads[0 .. 0] : @heads[1 .. $#heads])
    {
        ($heading, $regexp) = split(/::/, $_);

        (
            @items =
                $regexp =~ /unknown/
            ? keys %list
            : grep(/$regexp/i, keys %list)
            )
            || next;

        %sublist = ();
        foreach (@items)
        {
            $sublist{$_} = $list{$_};    # transfer to %temp
            delete $list{$_};            # and delete from %list
        }
        &PrintList(*sublist, $heading, $xref);
    }
}

#--------------------------------------------------------------------------
# PrintUrl(*list, $header, $xref, $posthead, $subhead)
#
#     Prints out URLs (and references if $xref) in %list.
#--------------------------------------------------------------------------

sub PrintUrl
{
    local (*list, $header, $xref, $posthead, $subhead) = @_;
    ($list = $list || scalar keys %list) || return;
    $header .= ':';
    local (@items, %invert);
    local ($headtext) = &Plural($list, $header);
    $DOCHEAD && ($headtext = sprintf("%13s: ", $DOCFILE) . $headtext);
    $headtext .= " " . &Plural($list, $posthead);

    $subhead = $subhead || "%d url%s:";
    $xref == 2 && ($headtext .= " (cross referenced)");

    $SUM == 2 && ((print "$headtext\n"), return);

    &InvertKeys(*list, *invert);

    $SUM || print "$Headline# $headtext\n$Headline";

    foreach (sort keys %invert)
    {
        @items = sort split(/\n/, $invert{$_});
        print &Plural(scalar @items, $SUM ? $header : $subhead), " $_\n";
        $SUM && next;
        &PrintSubUrl(*items, $xref);
    }
    print "\n" unless $SUM;
}

#--------------------------------------------------------------------------
# PrintSubUrl
#
#     Prints out all elements of $sublist split by "\n".  Prints out the
#     number of elements in pluralized $subhead.
#--------------------------------------------------------------------------

sub PrintSubUrl
{
    local (*items, $xref) = @_;
    foreach (@items)
    {
        print "    ", $_;
        &PrintUrlRedir($_);
        print "\n";
        ($xref && defined $ExtLink{$_}) || next;
        &PrintSubList($ExtLink{$_}, "used in %d file%s:", 8);
    }
    print "\n" unless $xref && defined $ExtLink{$_};
}

#---------------------------------------------------------------------------
# PrintUrlRedir($url)
#
#     Prints "linked list" of where $url was moved to due to 3XX status
#     codes.  Always returns if an infinite loop ( a -> b -> a ...)
#     occurs.
#---------------------------------------------------------------------------

sub PrintUrlRedir
{
    local ($url) = @_;
    local (%checked);
    while ($url = $UrlRedirect{$url})
    {
        $checked{$url}++ && return;
        print "\n      => ", $url;
    }
}

#---------------------------------------------------------------------------
# UrlSummary()
#
#     Print Textual summary of checking remote url status.
#---------------------------------------------------------------------------

sub UrlSummary
{
    $TotFail = $UrlFail;

    $CheckedUrls && print
        &Plural($CheckedUrls, "Linklink checked %d url%s:\n"),
        &Plural($UrlOk, "    %d %w ok, "),
        $TotFail, " failed",
        $UrlMoved ? &Plural($UrlMoved, ". %N url%s moved") : '',
        ".\n";

    $HostFail && print
        &Plural($HostFail, "    %n host%s failed:"),
        &Plural($UrlRetry, " %N url%s could be retried.\n");

    %ExtLink && print &Plural($UrlFailedF, "%N file%s had failed urls.\n");

    $ErrF && $UrlFailF && print &Plural($ErrF + $UrlFailedF, "There were %n file%s with broken links.\n");

    $CacheNet && print &Plural($UrlMod, "    %N url%s %w modified since last reset.\n");
}

#---------------------------------------------------------------------------
# Abbrev($max, $str)
#
#---------------------------------------------------------------------------

sub Abbrev
{
    local ($max, $str) = @_;
    length($str) > $max && ($str = substr($str, 0, $max - 4) . " ...");
    $str;
}

#---------------------------------------------------------------------------
# LogFile($filename)
#
#     Changes log file to filename for logging progress.
#---------------------------------------------------------------------------

sub LogFile
{
    local ($name) = @_;
    $name || return;

    $LogFile && $LogFile eq $name && return;    # don't reopen same file.

    select(STDERR);
    $LogFile && close($LogFile);
    $LogFile = $name;

    open($LogFile, ">$LogFile")
        || &Error(qq~could not open file "$LogFile" for output~, 'sys');
    select($LogFile);
}

###########################################################################
#
#    Utilities
#
###########################################################################

#---------------------------------------------------------------------------
# $outcnt = InvertKeys(*in, *out)
#
#     Assumes %in is filled with values like "file1\nfile2\n..."
#     Fills %out with keys file1, file2, ... and each key has values
#     of the keys from %in that refer to it.
#---------------------------------------------------------------------------

sub InvertKeys
{
    local (*in, *out) = @_;
    %in || return %out ? scalar keys %out : 0;

    local (%temp);
    foreach $in (keys %in)
    {
        %temp = ();
        grep($temp{$_}++, split(/\n+/, $in{$in}));
        foreach (keys %temp)
        {
            /\S/ || next;
            $out{$_} ? ($out{$_} .= "\n$in") : ($out{$_} = $in);
        }
    }
    scalar keys %out;
}

#--------------------------------------------------------------------------
# ResolveAnch(*want, *found, *lost)
#
#     named anchors wanted: %want{link#frag} = "ref1 \n ref2 \n ..."
#     named anchors found:  %found{link#frag} = 1
#     Fills *found and *lost as appropriate for found and lost named
#     anchors.  Works for named maps too.
#--------------------------------------------------------------------------

sub ResolveAnch
{
    local (*want, *found, *lost) = @_;

    foreach (keys %want)
    {
        /^([^#]*)(#.*)/;    # $1 will contain filename
        $file = $1;
        $anch = $2;
        $Ignore && $url =~ /$Ignore/o && next;    # ignore ignored files
        $Skipped{$file} && next;                  # skip anchors in skipped files
        $ref = $want{$_};
        $Mapped{$file} && do
        {
            $file = $Mapped{$file};
            $Ignore && $url =~ /$Ignore/o && next;    # ignore ignored files
            $Skipped{$file} && next;                  # skip skipped files
            $_ = "$file$anch";
        };
        $found{$_} ? ($found{$_} = $ref) : ($lost{$_} = $ref);
    }
}

#--------------------------------------------------------------------------
# HashUnique(*hash)
#
#     Assumes values in hash are "val1 \n val2 \n ..."
#     Ensures that each val occurs at most once in each hash value.
#--------------------------------------------------------------------------

sub HashUnique
{
    local (*hash) = @_;
    local (%temp, $key, $val);
    while (($key, $val) = each %hash)
    {
        %temp = ();
        grep($temp{$_}++, split(/\n+/, $val));
        $hash{$key} = join("\n", keys %temp);
    }
}

#--------------------------------------------------------------------------
# Warn(@msg)
#
#     Registers this warning in %Warn.  If $link is supplied we cross
#     reference the warning to $link.
#--------------------------------------------------------------------------

sub Warn
{
    local ($msg, $link) = @_;
    (!$link || $link eq 'sys') && do
    {
        $WarnList{$msg} = "\n";
        &Progress("WARNING: $msg");
        $link && $! && print STDERR "    System message: $!\n";
        return;
    };
    &AppendList(*WarnList, $msg, $link);
}

#--------------------------------------------------------------------------
# WantNumber($val, $flag)
#--------------------------------------------------------------------------

sub WantNumber
{
    local ($val, $flag) = @_;
    $val || return;
    $val =~ /^\d+$/ && return;
    &Error("$flag must be followed by an integer");
}

#--------------------------------------------------------------------------
# exit_with(@msg)
#--------------------------------------------------------------------------

sub exit_with { print @_; exit; }

#--------------------------------------------------------------------------
# Error(@msg)
#      Print @msg and exit.
#--------------------------------------------------------------------------

sub Error
{
    local ($msg, $flag) = @_;
    print STDERR "\n$prog error: $msg\n";
    @_ > 1 && $flag eq 'sys' && $! && (print STDERR "\nSystem message: $!\n");
    exit;
}

#--------------------------------------------------------------------------
# Progress(@msg)
#--------------------------------------------------------------------------

sub Progress
{
    local ($msg) = @_;
    $LogProgress && print $msg, "\n";
    $Quiet && return;
    print STDERR &Abbrev(75, $msg), "\n";
}

#--------------------------------------------------------------------------
# AppendList(*list, $key, $value)
#
#     Adds $value to $list{$key} separates values with "\n" as needed.
#--------------------------------------------------------------------------

sub AppendList
{
    local (*list, $key, $value) = @_;
    $list{$key} ? ($list{$key} .= "\n$value") : ($list{$key} = $value);
}

#--------------------------------------------------------------------------
# PushDir($newdir)
#
#     Pushes current directory onto a stack @DIRS and then
#     chdir's to $newdir.  We Assume that $newdir is a full path!
#     Returns O if there is an error.
#--------------------------------------------------------------------------

sub PushDir
{
    local ($new) = @_;
    push(@DIRS, $CWD);
    return $CWD if $new eq $CWD;
    chdir($new) || do
    {
        &Warn(qq~(pushdir) could not chdir to "$new"~, 'sys');
        return '';
    };
    return ($CWD = $new);
}

#--------------------------------------------------------------------------
# PopDir
#
#     Pops most recent directory off of stack @DIRS and
#     changes $CWD and current directory accordingly.
#--------------------------------------------------------------------------

sub PopDir
{
    local ($new) = pop(@DIRS);
    return $CWD if $new eq $CWD;
    chdir($new) || do
    {
        &Warn(qq~(popdir) could not chdir to "$new"~, 'sys');
        return '';
    };
    $CWD = $new;
}

#--------------------------------------------------------------------------
# GetCwd
#
#     Returns a string containing the current working directory
#     "\" is changed to "/" for consistency if $DOS.
#     Sets $CWD to the current working directory.
#--------------------------------------------------------------------------

sub GetCwd
{
    local ($_) = `$pwdprog`;    # different prog's for Dos/Unix
    $Dos && do
    {
        s|\\|\/|g;              # replace \ with /
        s/^([a-zA-Z])://;       # remove drive:
        $DosDrive = $1;         # save drive letter for dochtml printout
    };

    s/\n$//;                    # remove trailing \n
    $CWD = $_;
}

#--------------------------------------------------------------------------
# Regular($exp, $flag)
#
#--------------------------------------------------------------------------

sub Regular
{
    local ($exp) = @_;
    $exp =~ s#([^\w/])#\\$1#g;
    $exp;
}

#--------------------------------------------------------------------------
# LinkSet($flag, *in)
#
#     Fills $out with regular exppression made out that is the or'ed
#     of the the specs in @in.  Fills %out with all of the keys
#     that were literal expresions.
#--------------------------------------------------------------------------

sub LinkSet
{
    local ($flag, *in) = @_;

    %in = ('/*', '1') if $in{'/*'};    # keep it simple
    local ($out) = join('|', grep($_ = &LinkSet1($_, $flag), keys %in));
    $Arg{$flag} = $out;
    $out;
}

#--------------------------------------------------------------------------
# LinkSet1($exp)
#
#    If $exp does not contain ()[]^| or $  then it is converted to a
#    literal expression.  If this expression contains ? or * then these
#    are converted to [^/]* and .* and the whole thing is anchored to the
#    start and end. Otherwise check $exp  to be a valid regular expression.
#--------------------------------------------------------------------------

sub LinkSet1
{
    local ($_, $flag) = @_;

    s!([^\w\@#/])!\\$1!g;    # protect special characters

    s!\@!.*!g;               # @ to .* (match any)
    s!#/!#/?!g;              # make / behind # optional
    s!#![^/]*!g;             # '#' to [^/]* (match any but /)

    $_ = "^$_\$";
}

###########################################################################
#
# User Input Routines
#
###########################################################################

#--------------------------------------------------------------------------
# @linkset = ReadFile($file)
#
#     Every infile that starts with "@" is read and its contents are
#     are added to the end of the @infiles list. If an @file contains
#     lines starting with - then the entire line is read in as commands.
#--------------------------------------------------------------------------

sub ReadFile
{
    local ($file) = @_;
    local (@argv, @out);
    local ($lastarg, $lastflag, $arg);

    $CheckedIn{$file}++ && next;
    if ($file eq "STDIN")
    {
        $file = \*STDIN;
    }
    elsif ($file)
    {
        open($file, $file) || &Error(qq~could not open file-list "$file"~, 'sys');
    }
    else
    {
        $file = \*STDIN;
    }

    while (<$file>)
    {
        s/^#! ?// && do { print STDERR $_; next; };    # print comments
        m/^#/ && next;
        s/\s+$//;                                      # trailing whitespace
        s/^\s+//;                                      # leading whitespace

        s/^@@// && (push(@HttpFiles, $_), next);
        s/^@// && (push(@out, &ReadFile($_)), next);

        m/^-/ || (push(@out, split(/\s+/, $_)), next);

        while (s/("([^"]*)"|(\S+))\s*//)
        {
            $arg = $+;
            $arg =~ /^-/ && (push(@argv, $lastflag = $lastarg = $arg), next);
                   $lastflag
                && $lastarg !~ /^-/
                && $lastflag =~ /-($HashOpts)/o
                && push(@argv, $lastflag);
            push(@argv, $lastarg = $arg);
        }
    }
    close($file);

    @argv && &ReadArgs(@argv);
    @out;
}

#--------------------------------------------------------------------------
# ReadHttp($file)
#
#     Reads $file and returns every "http://..." it can find.
#     returns the list of links in an array.
#--------------------------------------------------------------------------

sub ReadHttp
{
    local ($file) = @_;
    local (@out);

    $file || do
    {
        $Arg{'doc'} || &Error("@@ must be preceded by -doc linkdoc");
        $file = "$Arg{'doc'}/remoteX.txt";
    };

    $CheckedIn{$file}++ && return;

    open($file, $file) || &Error(qq~could not open http-list "$file"~, 'sys');

    while (<$file>)
    {
        /^file: remoteX.txt/ && $. == 1 && return &ReadBack($file);
        while (s#(http://[^\s"><']+)##i) { push(@out, $1); }
    }
    close($file);
    @out;
}

#--------------------------------------------------------------------------
# ReadBack($file)
#
#     Reads in $file as if it were the remoteX.txt file generated by
#     Linklint.  %ExtLink if filled with the cross references found
#     in the file.  Returns a list of all urls found.
#--------------------------------------------------------------------------

sub ReadBack
{
    local ($file) = @_;
    local ($url, @out);
    while (<$file>)
    {
        /^#/ && ($url = '', next);    # new section
        s/\s+$//;                     # trailing \n
        /^http:/ && (push(@out, $url = $_), next);    # a url to check
        s#^    ## || next;                            # sublist indent
        m#^/# && &AppendList(*ExtLink, $url, $_);     # local link
    }
    close($file);
    @out;
}

#--------------------------------------------------------------------------
# SetHash('name', $key, $value)
#
#     Sets $name{$key} = $value.
#--------------------------------------------------------------------------

sub SetHash
{
    local (*hash, $key, $val) = @_;
    $hash{$key} = $val;
}

#--------------------------------------------------------------------------
# ReadArgs(@args)
#
#     Reads arguments from @args (all start with "-") returns the
#     remainder of @args.   We first check for full flags and options.
#     If these don't match exactly we go through the argument looking
#     for short flags and options 'globbed' together.  Flags set
#     $Arg{'X'} to 1.  For short flags X is the flag, for full flags X
#     is the first character.  Short options set $Arg{'X'} to the next
#     argument.  Full options set $X to the next argument where X
#     is the value from %fullopts. ZZZ Has been modified.
#--------------------------------------------------------------------------

sub ReadArgs
{
    local ($name, @out);

    while (@_ && ($_ = shift))
    {
        s/^@@// && (push(@HttpFiles, $_), next);
        s/^@// && (push(@out, &ReadFile($_)), next);
        (m#^/# || m#\.html?$#i || s#^http://#http://#i)
            && (push(@out, $_), next);

        s/^-// || &Error(qq~at "$_"~ . qq~\nexpected: "-flag" or "/linkset" or "http: ..."\n~ . $ErrUsage);

        if (/^($MiscFlags)$/o)
        {
            $Arg{$1}++;
        }
        elsif (/^($FullOpts)$/o)
        {
            (@_ < 1 || $_[0] =~ /^-/)
                && &Error("expected parameter after -$_\n" . $ErrUsage);
            $Arg{$_} = shift;
        }
        elsif (/^($HashOpts)$/o)
        {
            (@_ < 1 || $_[0] =~ /^-/)
                && &Error("expected parameter after -$_\n" . $ErrUsage);
            ($name = $_) =~ tr/a-z/A-Z/;
            &SetHash($name, shift, 1);
        }
        elsif (/^password$/)
        {
            (@_ < 2 || $_[0] =~ /^-/ || $_[1] =~ /^-/)
                && &Error("expected 2 parameters after -$_\n" . $ErrUsage);
            $_[1] =~ /:/
                || &Error(qq~expected username:password at "$_[1]"\n~ . $ErrUsage);
            $PASSWORD{ $_[0] } = $_[1];
            shift;
            shift;
        }
        elsif (s/^db//)
        {
            while ($_)
            {
                s/^([\d])//
                    || &Error("expected a digit after -db\n" . $ErrUsage);
                $DB{$1}++;
            }
        }
        else
        {
            &Error("unknown flag -$_\n" . $ErrUsage);
        }
    }
    return (@out, @_);
}

sub HttpInit
{
    local ($arg, $db, $pw, $headers, $agent) = @_;
    defined &Http'Init || do
    {
        unshift(@INC, $HOME);
        require "linkhttp.pl";
    };
    &Http'Init($arg, $db, $pw, $headers, $agent);
}

#==========================================================================
# End of linklint
#==========================================================================

###########################################################################
#
# Http Package
#
# Init(*arg, *db, *pw, *headers, $agent);
#     'timeout'    abort request after timeout seconds
#     'delay'      delay between requests to same host
#     'netset'     resets last modified state in cache
#     'redirect'   search for redirects in files
#     'ignore'
#     'VERSION'
#     'DOS'
#
# OpenBot($botfile)
#
# OpenCache($cachefile)
#
# FlushCache(*validurls)
#
# UrlsFromCache($ok, $retry, $fail)
#     Returns @urls with urls from cache depending on status in cache.
#
# CheckURLS(*urls)
#     Check all http:// urls in @urls and returns %UrlStatus
#     filled with urls and status code.
#
# StatusMsg(*status, *ok, *fail, *warn)
#     Fills %ok, %fail %warn, with urls and error messages
#     from *status.
#
# OtherStatus(*urlmod, *urlmoved, *hostfail, *redirect)
#     Fills hashes with modified, moved, and host fail messeges
#     Fills *redirect with redirected urls and destinations.
#
# WriteCaches()
#
###########################################################################

package Http;

use Socket;

#--------------------------------------------------------------------------
# Init(*arg, *db, *pw, *headers, $agent)
#
#     Sets up global variables. Can be called more than once.
#--------------------------------------------------------------------------

sub Init
{
    local (*arg, *db, *pw, *headers, $agent) = @_;
    %Arg = %arg;
    %DB = %db;

    @Base64 = ('A' .. 'Z', 'a' .. 'z', '0' .. '9', '+', '/');

    foreach (keys %pw)
    {
        $PassWord{$_} = &Base64Encode($pw{$_});
    }

    $TimeOut = $Arg{'DOS'} ? 0 : $Arg{'timeout'} || 0;

    $Delay = $Arg{'delay'};
    $ResetCache = $Arg{'netset'};

    $UserAgent = "LinkLint-$agent";
    $Arg{'VERSION'} && ($UserAgent .= "/$Arg{'VERSION'}");

    $Proxy = $Arg{'proxy'} || '';
    $Proxy_Port = ($Proxy and $Proxy =~ s/:(\d*)$//) ? $1 : '';

    ($DB{7} || $DB{8}) && do
    {
        print "TimeOut: $TimeOut\n";
        print "User-Agent: $UserAgent\n";
        $Proxy && print "Proxy: $Proxy\n";
        $Proxy_Port && print "Proxy Port: $Proxy_Port\n";
    };

    $TimeOut && do
    {
        $SIG{'ALRM'} = *AlarmHandler;
        $SIG{'INT'} = *IntHandler;
    };

    $Init++ && return;

    $CRLF = $Arg{'DOS'} ? "\n" : pack("cc", 13, 10);

    for my $header_line (keys %headers)
    {
        my ($name, $value) = $header_line =~ m/^([\w\-]+):\s*(.*)/;
        $name && $value or do
        {
            &main'Warn("Could not parse -http_header $header_line");
            next;
        };
        $name = join "-", map ucfirst, map lc, split /-/, $name;
        $USER_REQ_HEAD{$name} = $value;
        $USER_HEADERS .= "${CRLF}$name: $value";
    }

    $StatType = "url last-modified cache";
    $BotType = "robot exclusion cache";

    $NOW = time;

    %FlagOk = (
        200, 'ok (200)', 201, 'ok created (201)', 202, 'ok accepted (202)',
        304, 'ok not modified (304)', -2000, 'ok parsed html', -2001, 'ok skipped',
        -3005, 'ok last-modified date unchanged', -3006, 'ok did not compute checksum', -3007, 'ok checksum matched',
    );

    %FlagNotMod = (304, 1, -3005, 1, -3006, 1, -3007, 1,);

    %FlagMoved = (301, 'moved permanently (301)', 302, 'moved temporarily (302)', -3003, 'redirected',);

    %FlagRetry = (
        -1, 'could not find ip address', -2, 'could not open socket', -3, 'could not bind socket',
        -4, 'could not connect to host', -5, 'timed out waiting for data', -8, 'malformed status line',
        -12, 'timed out before anything could happen', -14, 'timed out connecting to host', -15, 'timed out waiting for data',
        -16, 'timed out reading status', -17, 'timed out reading header', -18, 'timed out reading data',
        -19, 'timed out getting host ip address', 502, 'server temporarily overloaded (502)', 503, 'gateway timeout (503)',
    );

    %FlagWarn = (
        -6, 'not an http link', -7, 'no status. Will try GET method', -10, 'Disallowed by robots.txt',
        -11, 'infinite redirect loop', 401, 'access not authorized (401)', -4000, 'moved to non-local server',
        -4010, 'invalid username/password (401)', -4020, 'unknown authorization scheme (401)', -5000, 'user interrupt',
        -6000, 'unknown internal error',
    );

    %FlagFail = (
        204, 'had no content (204)', 301, 'moved permanently, no new URL (301)', 302, 'moved temporarily, no new URL (302)',
        400, 'bad request (400)', 403, 'access forbidden (403)', 404, 'not found (404)',
        500, 'internal server error (500)', 501, 'service not implemented on server (501)',
    );

    %FlagBad = (%FlagRetry, %FlagWarn, %FlagFail);
    @FlagDebug = (keys %FlagOk, keys %FlagMoved, keys %FlagRetry, keys %FlagFail, keys %FlagWarn,);
}

#--------------------------------------------------------------------------
# OpenBot($botfile)
#
#     Reads cache of robot exclusion info from $botfile filling %BotExclude.
#     Must be run after Init.
#--------------------------------------------------------------------------

sub OpenBot
{
    local ($botfile) = @_;
    &ReadCache($botfile, $BotType, *BotExclude);
}

#--------------------------------------------------------------------------
# OpenCache($cachefile)
#
#     Reads status/last-modified cache from $cachefile int %StatCache
#--------------------------------------------------------------------------

sub OpenCache
{
    local ($statfile) = @_;
    &ReadCache($statfile, $StatType, *StatCache);
}

#--------------------------------------------------------------------------
# FlushCache(@valid)
#
#     Removes all entrys in StatCache that do not also occur in *valid
#--------------------------------------------------------------------------

sub FlushCache
{
    @_ || do
    {
        &main'Warn("Won't flush entire cache.");
        return;
    };
    local (%valid);

    grep($valid{$_}++, @_);
    local (@delete) = grep(!$valid{$_}, keys %StatCache);
    foreach (@delete)
    {
        delete $StatCache{$_};
    }
    local ($cnt) = scalar @delete;
    &main'Progress("Deleted $cnt entries from cache.");
    @delete && $TaintCache{$StatType}++;
}

#--------------------------------------------------------------------------
# CheckURLS(@urls)
#
#    Checks all http links in @urls
#    Returns hash of urls and status codes.
#--------------------------------------------------------------------------

sub CheckURLS
{
    local ($flag, %http);
    grep(s#^http://#http://#i && $http{$_}++, @_);
    local (%checked);

    &main'Progress(&main'Plural(scalar keys %http, "\nchecking %d url%s ...\n"));

    foreach $url (sort keys %http)
    {
        next unless $url =~ m#^http://#i;
        $Arg{'ignore'} && $url =~ m/$Arg{'ignore'}/o && next;
        next if $checked{$url}++;
        $flag = &CheckUrl($url, 0);
        $flag = &UrlMoved($flag, $url);
        $UrlStatus{$url} = $flag;
        if ($Arg{concise_url})
        {
            $FlagBad{$flag} and do
            {
                &main'Progress($url);
                &main'Progress("  " . &ErrorMsg($flag));
            };
        }
        else
        {
            &main'Progress("  " . &ErrorMsg($flag));
        }
    }
    %UrlStatus;
}

#--------------------------------------------------------------------------
# $flag = UrlMoved($flag, $url)
#
#     Process 3XX status.  Recheck $url given back in Location field
#     of HEADER.  Continue until
#     a) non-3XX status,  b) infinite loop  c) already checked.
#--------------------------------------------------------------------------

sub UrlMoved
{
    local ($flag, $url) = @_;
    local (%checked, $next);

    while ($FlagMoved{$flag})
    {
        $checked{$url}++ && return -11;    # infinite loop
        ($next = $HEADER{'location'}) || return $flag;    # this is an error
        $UrlMoved{$url} = $FlagMoved{$flag};
        $Redirect{$url} = $next;
        $UrlStatus{$next} && return $UrlStatus{$next};
        $flag = &CheckUrl($url = $next, 1);
    }
    return $flag;
}

#--------------------------------------------------------------------------
# Parse($host, $link, $referer, *list, *anchlist, *wantanchor)
#
#
#--------------------------------------------------------------------------

sub Parse
{
    local ($thishost, $oldlink, $REFERER, @param) = @_;
    local ($port) = $thishost =~ s/:(\d+)$// ? $1 : 80;
    local ($host) = $thishost;
    local ($scheme);
    local ($LINK) = $oldlink;
    local ($url) = "http://$thishost:$port$LINK";    # for warning messages

#    $HostError{$host} && return ($HostError{$host}, $LINK);

    local (%checked);

    ($flag = &Disallowed($url, 0.5)) && return ($flag, $LINK);

    $flag = &Request($host, $port, $LINK, 'GET', *GetParse, @param);

    while ($FlagMoved{$flag})
    {
        $checked{$LINK}++ && return (-11, $LINK);    # infinite loop
        ($url = $HEADER{'location'}) || return ($flag, $LINK);

        ($scheme, $host, $port, $LINK) = &SplitUrl($url);
        ($host ne $thishost) && return (-4000, $url);    # non-local server

        &main'WasCached($LINK, $REFERER) && return (0, $LINK);

        $flag = &Request($host, $port, $LINK, 'GET', *GetParse, @param);
    }

    return ($flag, $LINK);
}

#--------------------------------------------------------------------------
# $flag = CheckUrl($url, $recheck)
#
#     Returns status of a URL.  Uses the robots.txt protocol.
#     If $Arg{'redirect'} uses "GET" with html files otherwise
#     tries a "HEAD" first and if that fails tries "GET".
#--------------------------------------------------------------------------

sub CheckUrl
{
    local ($url, $recheck) = @_;
    local ($scheme, $host, $port, $path) = &SplitUrlQ($url);

    return -6 if $scheme ne "http";

    $REQHEAD{'Host'} = $host;

    if ($Arg{concise_url})
    {
    }
    elsif ($recheck)
    {
        &main'Progress("  moved");
        &main'Progress("  $host$path");
    }
    else
    {
        &main'Progress("$host$path");
    }

    $OpenedCache{$StatType} && !$recheck && return &CheckModified($url);

    return $Arg{'redirect'}
        ? &Request($host, $port, $path, 'GET', "GetRedirect", $url)
        : &Request($host, $port, $path, "GET");
}

#--------------------------------------------------------------------------
# CheckModified($url)
#
#     Outer wrapper to see if $url has been modified.
#     Always update status in cache. Always update cache for new entries.
#     Update time, last-mod, checksum if new entry or reset.
#--------------------------------------------------------------------------

sub CheckModified
{
    local ($url) = @_;
    local ($flag);
    local ($_, $time, $mod, $csum) = $StatCache{$url} ? split(/\s+/, $StatCache{$url}) : ('0', '0', '0', '0');

    local ($nmod, $ncsum) = ($mod, $csum);

    $TIMESTR = &main'TimeStr('GMT', $time);    # for http header

    if ($mod ne '0')
    {                                          # only check last-mod date

        $flag = &Request($host, $port, $path, "GET");

        $FlagOk{$flag} && do
        {

            if ($HEADER{'last-modified'})
            {
                ($nmod = $HEADER{'last-modified'}) =~ tr/ /_/;
                $flag = -3005 if ($nmod eq $mod);
            }
            else { $mod = $nmod = '0'; }
        };
    }

    else
    {
        $REQHEAD{'if-Modified-Since'} = $TIMESTR;
        $flag = &Request($host, $port, $path, 'GET', "GetModified");
    }

    $csum = -1 if $flag == 304;    # -1: server obeys "if-mod"

    $FlagNotMod{$flag}
        || !$FlagOk{$flag}
        || ($UrlMod{$url} = "modified since " . &main'TimeStr('(local)', $time));

    #---- update cache if -reset or 1st time we get something new

    $FlagOk{$flag} && do
    {
        $time = $NOW if $ResetCache || $time eq '0';
        $csum = $ncsum if $ResetCache || $csum eq '0';
        $mod = $nmod if $ResetCache || $mod eq '0';
    };

    local ($temp) = join(" ", $flag, $time, $mod, $csum);    # create new entry

    #---- update entry in cache if needed

    (!$StatCache{$url} || $StatCache{$url} ne $temp) && do
    {
        $StatCache{$url} = $temp;
        $TaintCache{$StatType}++;
    };

    $flag;
}

#--------------------------------------------------------------------------
# GetModified($flag, *S)
#
#     Passed to request4().  Only get here if file was modified or
#     "If-Modified-Since" was ignored.  Check last-mod date 1st.
#     then check checksum only if they ask and if the $time was > 0.
#     If $csum == -1 then don't bother with checksum.
#--------------------------------------------------------------------------

sub GetModified
{
    local ($flag, *S) = @_;

    $HEADER{'last-modified'} && do
    {    # use this info
        ($nmod = $HEADER{'last-modified'}) =~ tr/ /_/;
        return $flag if $mod ne '0' && ($nmod ne $mod);    # modified
        return -3005;                                      # not modified
    };

    $csum == -1 && return $flag;                           # modified: obeyed "if-mod" before

    return -3006 unless $Arg{'checksum'} && $time != 0;    # don't checksum

    &main'Progress("    computing checksum");

    $ncsum = 0;

    while (<S>) { $ncsum += unpack("%32C*", $_); }         # compute csum

    return $flag if $csum ne '0' && ($ncsum != $csum);     # modified
    return -3007;                                          # not modified
}

#--------------------------------------------------------------------------
# UrlsFromCache($ok, $retry, $fail)
#
#    Returns @urls with urls from the cache depending on the last
#    status of the url in the cache.
#--------------------------------------------------------------------------

sub UrlsFromCache
{
    local ($ok, $retry, $fail) = @_;
    local (@urls);
    $OpenedCache{$StatType} || &main'Warn("No url cache to read from.");

    local ($url, $cache, $flag);
    while (($url, $cache) = each %StatCache)
    {
        ($flag) = split(" ", $cache);
        push(@urls, $url)
            if ($fail && $FlagWarn{$flag})
            || ($fail && $FlagFail{$flag})
            || ($ok && $FlagOk{$flag})
            || ($retry && $FlagRetry{$flag});
    }
    @urls;
}

#--------------------------------------------------------------------------
# Recheck(@urls)
#
#     Returns only those urls that need to be retried.
#--------------------------------------------------------------------------

sub Recheck
{
    local (%retry);
    grep($retry{$_}++, &UrlsFromCache(0, 1, 0));
    grep($retry{$_}, @_);
}

#--------------------------------------------------------------------------
# $flag = Disallowed($url)
#
#     Checks robots.txt file for $url.  Results are cached for each host.
#     Returns:
#        -10  if access is excluded by robots.txt
#          0  if access is allowed
#        < 0  if < 0 (non-http) error occured
#--------------------------------------------------------------------------

sub Disallowed
{
    local ($url, $expire) = @_;
    local ($scheme, $host, $port, $path) = &SplitUrl($url);
    local ($flag);

    $BotExclude{$host} && do
    {
        local ($time, $xpath) = split(/\s+/, $BotExclude{$host});
        local ($secs) = 60 * 60 * 24 * ($expire || 30);
        $time + $secs >= $NOW && do
        {
            $xpath eq 'ok' && return 0;
            return ($path =~ m/^$xpath/) ? -10 : 0;
        };
    };

    &main'Progress("    checking robots.txt for $host");
    $flag = &Request($host, $port, "/robots.txt", "GET", "GetText", 100);

    return $flag if $FlagRetry{$flag};

    $BotExclude{$host} = time . " ok";    # default value
    $TaintCache{$BotType}++;              # need to write a new file

    return 0 unless $FlagOk{$flag};

    $_ = join("", @DATA);
    s/\r\n?/\n/g;                         # end-of-line = \r | \n | \r\n
    @DATA = split(/\n/, $_);
    local (@agents, @disallow);
    push(@DATA, " ");                     # ensure last group gets proccessed
    foreach (@DATA)
    {
        next if /^\s*#/;
        s/\s+$//;
        if (/^$/)
        {
            if (@disallow && @agents)
            {
                $_ = join(" ", @disallow);    # prepare for use as regexp
                s#([^\w\s/])#\\$1#g;          # literal search (pretty)
                s/\s+/\|/g;
                $BotExclude{$host} = "$NOW $_";
                last if grep(/linklint/i, @agents);
            }
            @agents = @disallow = ();
            next;
        }

        s/\s*#.*$//;                          # trailing comments

        if (m/^\s*(User.?Agent|Robot)s?\s*:\s+(\S+.*\S?)\s*$/i)
        {
            push(@agents, $2);
        }
        elsif (m/^\s*Disallow\s*:\s+(\S+.*\S?)\s*$/i)
        {
            next unless grep(/(linklint|\*)/i, @agents);
            push(@disallow, $1);
        }
    }
    return &Disallowed(@_);                   # only recurse once. Relies on BotExclude{host}
}

#--------------------------------------------------------------------------
# GetParse($flag, *S, $link, *newlinks, *Anchor, *wantanch);
#
#     Parse html via remote http connection.
#--------------------------------------------------------------------------

sub GetParse
{
    local ($flag) = shift;
    local ($handle) = shift;

    &main'AppendList(*main'FileList, $LINK, $REFERER);

#    &main'CacheDir($LINK);

    $HEADER{'content-type'} =~ m#^text/html#i || return $flag;

    &main'StopRecursion($LINK, $REFERER) && return -2001;
    &main'Progress("Checking $LINK");
    &main'ParseHtml($handle, $LINK, @_);

    return -2000;    # parsed the file
}

#--------------------------------------------------------------------------
# GetRedirect($flag, 'S', $url)
#
#     Passed to request4() by CheckUrl() to parse for redirects in header.
#--------------------------------------------------------------------------

sub GetRedirect
{
    local ($flag) = shift;
    $HEADER{'content-type'} =~ m#^text/html#i || return $flag;

    local ($redir) = &main'ParseRedirect(@_);
    $redir || return $flag;
    $HEADER{'location'} = $redir;
    return -3003;
}

#--------------------------------------------------------------------------
# GetText($flag, *S, $lines)
#
#     Passed to request4() to read $lines of text into @DATA.
#     For now @DATA is a global.
#--------------------------------------------------------------------------

sub GetText
{
    local ($flag, *S, $lines) = @_;

    $HEADER{'content-type'} =~ /^text/ || return $flag;

    while (<S>)
    {    # read $lines into @DATA
        push(@DATA, $_);
        $lines && (--$lines || last);
    }
    $flag;
}

#--------------------------------------------------------------------------
# $flag = Request($host, $port, $path, $method, *getmethod, @params)
#
#     Handles host errors. Flags bad hosts. Caches host errors.
#     Calls Request2(). Will retry if more than one IP address is given
#     by gethostbyname() and we get StatRetry errors.
#
#     Use subroutine getmethod($flag, 'S', @params) if given
#     to read data after the header.  I don't have a great method for
#     sending data back at the moment.  Use globals for now.
#--------------------------------------------------------------------------

sub Request
{
    my ($host, $port, $path, @other) = @_;
    my $flag;

    $REQHEAD{Host} = $host;
    if ($Proxy)
    {
        my $v_url = "http://$host";
        $port and $v_url .= ":$port";
        $v_url .= $path;

        $flag = Request2($Proxy, $Proxy_Port, $v_url, @other);
    }
    else
    {
        $flag = &Request2(@_);
    }
    %REQHEAD = ();
    return $flag;
}

sub Request2
{
    local ($_, $flag, $realm, $pw, $auth);
    $flag = &Request3(@_);
    $flag == 401 || return $flag;

    ($auth = $HEADER{'www-authenticate'}) || do
    {
        &main'Warn(qq~missing authentication~, $url);
        return -4020;
    };

    $auth =~ m/\s*basic\s+realm\s*=\s*"([^"]*)"/i || do
    {
        &main'Warn(qq~unknown authorization scheme: $auth~, $url);
        return -4020;
    };

    $realm = $1;

    if ($pw = $PassWord{$realm} || $PassWord{'DEFAULT'})
    {
        $REQHEAD{'Authorization'} = "Basic $pw";
        $flag = &Request3(@_);
        $flag != 401 && return $flag;
        &main'Warn(qq~invalid password for "$realm"~);
        return -4010;
    }
    else
    {
        &main'Warn(qq~need password for "$realm"~);
        return $flag;
    }
}

sub Request3
{
    local ($host) = shift;

    local ($flag) = &Request4($host, @_);

    return $flag unless $FlagRetry{$flag};

    $IpAddr2{$host} || return &HostError($host, $flag);

    #----- try alternate ip addresses

    foreach (split(/\n/, $IpAddr2{$host}))
    {
        &main'Progress("Warning: $host $IpAddr{$host}");
        &main'Progress("    Error: $FlagRetry{$flag}");

        &main'Progress("    checking server $_");

        $flag = &Request4($_, @_);
        next if $FlagRetry{$flag};

        $IpAddr{$host} = $_;    # save good one as the default
        return $flag;
    }
    &HostError($host, $flag);    # all servers for this host are down
}

#--------------------------------------------------------------------------
# HostError($host, $flag)
#
#     Fills %HostError with $flag, fills %HostFail with error message.
#     Returns $flag.
#--------------------------------------------------------------------------

sub HostError
{
    local ($host, $flag) = @_;
    $HostFail{$host} = &ErrorMsg($flag);
    $HostError{$host} = $flag;
}

#--------------------------------------------------------------------------
# $flag = Request4($host, $port, $path, $method, 'getmethod', @params)
#
#     Fills %HEADER with header info,
#     Uses globals set by Init().
#     $flag is error (or success) flag see %Httpxxx for details.
#     Will use getmethod($flag, 'S', @params) to read data.
#--------------------------------------------------------------------------

sub Request4
{
    local ($host, $port, $path, $method, $getmethod, @params) = @_;
    local ($request, $ipaddr, $flag);
    $port = $port || 80;

    %HEADER = ();    # global %HEADER holds http header info.

    $DB{9} && do
    {
#        $HEADER{'location'} = 'http://dum/dum';
        return $flag = $FlagDebug[int(rand(@FlagDebug))];
    };

    (($ipaddr, $flag) = &GetIpAddress($host));
    $flag && return $flag;

    $Delay && sleep($Delay);

    $DB{7} && print
        "\n$method http://$host$path\n",
        "host ip: $IpAddr{$host}\n";

    $request = "$method $path HTTP/1.0";

    $USER_HEADERS and $request .= $USER_HEADERS;

    $REQHEAD{"User-Agent"} = $UserAgent;

    for my $name (sort keys %REQHEAD)
    {
        $USER_REQ_HEAD{$name} and next;
        $request .= "${CRLF}$name: $REQHEAD{$name}";
    }

    $ALARMFLAG = -12;

    $@ = '';

    $TimeOut && alarm($TimeOut);

    $SOCKETOPEN = 0;

    eval { $flag = &FragileRequest($ipaddr, $request, $getmethod || \NUL, @params); };

    $TimeOut && alarm(0);

    $SOCKETOPEN && close(S);
    $SOCKETOPEN = 0;
    $@ || return $flag;
    $@ =~ /^timeout/ && return $ALARMFLAG;
    $@ =~ /^user interrupt/ || return -6000;
    print STDERR "\nUser Interrupt.\n", "Interrupt again to abort or wait 2 seconds for linklint to resume\n";
    sleep(2);
    return -5000;
}

#--------------------------------------------------------------------------
# FragileRequest($ipaddr, $request, *getmethod, @params)
#
#    Actually sends $request, and reads status, header and data from socket.
#    Should be called from within an eval() to implement timeout.
#--------------------------------------------------------------------------

sub FragileRequest
{
    local ($ipaddr, $request, *getmethod, @params) = @_;

    my $iaddr = inet_aton($ipaddr);
    my $paddr = sockaddr_in($port, $iaddr);

    my $proto = getprotobyname('tcp');
    socket(S, PF_INET, SOCK_STREAM, $proto)
        or return -2;

    $SOCKETOPEN = 1;

    $ALARMFLAG = -14;    # "connecting to host"

    $DB{4} && &main'Progress("    Connecting");

    connect(S, $paddr) || return -4;    # -4: could not connect

    $DB{4} && &main'Progress("    Connect successful");

    $ALARMFLAG = -15;                   # "waiting for data";

    local ($lastsel) = select(S);
    $| = 1;
    select($lastsel);

    $DB{8} && print "\n$request\n\n";

    print S "$request$CRLF$CRLF";

#    $TimeOut && do {
#        local($rin) = '';
#        vec($rin, fileno(S), 1 ) = 1;
#        select($rin, undef, undef, $TimeOut ) || return -5;
#        $ALARMFLAG = -16;  # "reading status"
#    };

    $_ = <S>;    # read status line
    $_ || return -7;    # -7: no status  (will try GET)

    $TimeOut && alarm($TimeOut);

    s/\s*$//;

    $flag = /^\s*\S+\s+(\d+)/ ? $1 : -8;    # -8: Malformed status line

    ($DB{7} || $DB{8}) && print "$_\n";

    $flag == -8 && return $flag;

    $ALARMFLAG = -17;                       # "reading header";

    local ($name);
    while (<S>)
    {                                       # put header info into %HEADER
        s/\s*$//;
        $DB{8} && print "$_\n";
        ($DB{7} && $DB{8}) && next;
        last unless m/\w/;
        next unless m/(\S+):\s+(\S*.*\S*)\s*$/;
        ($name = $1) =~ tr/A-Z/a-z/;
        $HEADER{$name} = $2;
    }

    $flag == 200 && @_ > 2 && defined &getmethod && do
    {

        $ALARMFLAG = -18;    # "reading data"
        return &getmethod($flag, *S, @params);
    };

    return $flag;
}

sub AlarmHandler
{
    die "timeout";
}

sub IntHandler
{
    die "user interrupt";
}

#--------------------------------------------------------------------------
# GetIpAddress($host)
#
#     Returns UNpacked IP address for host. Caches the address in
#     $IpAddr{$host}, caches alternate IP addresses in $IpAddr2{$host}.
#--------------------------------------------------------------------------

sub GetIpAddress
{
    local ($host) = @_;

    local ($_, @addrlist);

    $host =~ m/(\d+)\.(\d+)\.(\d+)\.(\d+)/
        && ($IpAddr{$host} = $host);

    $IpAddr{$host} && return ($IpAddr{$host}, 0);    # use cached ip address

    $TimeOut && alarm($TimeOut);

    eval('($_,$_,$_,$_, @addrlist) = gethostbyname($host)');
    $TimeOut && alarm(0);
    $@ && $@ =~ /^timeout/ && return (0, -19);

    @addrlist || return (0, -1);                     # could not find host

    grep($_ = join(".", unpack("C4", $_)), @addrlist);

    $IpAddr{$host} = shift @addrlist;                # 1st one is default
    $IpAddr2{$host} = join("\n", @addrlist);         # save others just in case
    return ($IpAddr{$host}, 0);
}

#--------------------------------------------------------------------------
# SplitUrl($url)
#
#     Split the given URL into its component parts according to HTTP rules.
#     returns ($scheme, $host, $port, $path, $query, $frag)
#--------------------------------------------------------------------------

sub SplitUrl
{
    local ($_) = $_[0];
    local ($scheme) = s#^(\w+):## ? $1 : '';
    local ($host) = s#^//([^/]*)## ? $1 : '';
    local ($port) = $host =~ s/:(\d*)$// ? $1 : '';
    local ($query) = s/\?(.*)$// ? $1 : '';
    local ($frag) = s/#([^#]*)$// ? $1 : '';
    $scheme =~ tr/A-Z/a-z/;
    $_ = $_ || '/';
    return ($scheme, $host, $port, $_, $query, $frag);
}

#--------------------------------------------------------------------------
# SplitUrl($url)
#
#     Split the given URL into its component parts according to HTTP rules.
#     returns ($scheme, $host, $port, $path, $query, $frag)
#--------------------------------------------------------------------------

sub SplitUrlQ
{
    local ($scheme, $host, $port, $path, $query) = &SplitUrl(@_);
    $query && ($path .= "?" . $query);
    return ($scheme, $host, $port, $path);
}

#--------------------------------------------------------------------------
# StatusMsg(*all, *ok, *fail, *warn)
#
#    Fills %ok, %fail, %retry with urls and error MESSAGES from
#    urls and error FLAGS in %all.
#--------------------------------------------------------------------------

sub StatusMsg
{
    local (*all, *ok, *fail, *warn) = @_;
    while (($url, $flag) = each %all)
    {
        $msg = &ErrorMsg($flag);
        $FlagOk{$flag} && ($ok{$url} = $msg, next);
        $FlagWarn{$flag} && ($warn{$url} = $msg, next);
        $fail{$url} = $msg;
    }
}

sub RetryCount
{
    local (*flags) = @_;
    scalar grep($FlagRetry{$_}, values %flags);
}

#--------------------------------------------------------------------------
# ErrorMsg($flag)
#
#    Returns the error message associated with flag.
#--------------------------------------------------------------------------

sub ErrorMsg
{
    local ($flag) = @_;

    $DB{6} || ($FlagOk{$flag} && return 'ok');

           $FlagOk{$flag}
        || $FlagFail{$flag}
        || $FlagWarn{$flag}
        || $FlagRetry{$flag}
        || "unknown error ($flag)";
}

#--------------------------------------------------------------------------
# OtherStatus(*urlmod, *hostfail, *urlmoved, *redirect)
#
#     Fills hashes with modified, moved, and host fail messeges
#     Fills *redirect with redirected urls and destinations.
#--------------------------------------------------------------------------

sub OtherStatus
{
    local (*urlmod, *hostfail, *urlmoved, *redirect) = @_;
    %urlmod = %UrlMod;
    %urlmoved = %UrlMoved;
    %hostfail = %HostFail;
    %redirect = %Redirect;
}

#--------------------------------------------------------------------------
# ReadCache($file, $type, *hash)
#
#     Reads $file, looking for key val1 on each line.
#     Fills hash{key} = val1.  Skips entries that have expired.
#     Sets $OpenedCache{$type} to $file for writing later.
#     Also used to only read the cache file once.
#--------------------------------------------------------------------------

sub ReadCache
{
    local ($file, $type, *hash) = @_;
    $OpenedCache{$type} && return;
    $OpenedCache{$type} = $file;
    open(CACHE, $file) || return;

    ($DB{7} || $DB{8}) && &main'Progress(qq~reading $type from "$file"~);

    foreach (<CACHE>)
    {
        /^#/ && next;
        /^(\S+)\s+(\S+)\s+(\S.*\S)\s*$/ || next;
        $hash{$1} = "$2 $3";
    }
    close(CACHE);
}

#--------------------------------------------------------------------------
# WriteCaches()
#--------------------------------------------------------------------------

sub WriteCaches
{
    &WriteCache($BotType, *BotExclude);
    &WriteCache($StatType, *StatCache);
}

#--------------------------------------------------------------------------
# WriteCache($type, *hash)
#
#     Writes key(hash1) value(hash1) to file.  Preceded by $header.
#     Will only write if cache was opened via ReadCache() and if the
#     cache has been tainted via %TaintCache.
#--------------------------------------------------------------------------

sub WriteCache
{
    local ($type, *hash) = @_;
    local ($file) = $OpenedCache{$type} || return;
    ($TaintCache{$type} && %hash) || return;

    open(CACHE, ">$file") || do
    {
        &main'Warn(qq~Could not write $type to "$file"\n~, 'sys');
        return;
    };

    local ($header) =
        "# $type created by linklint\n" . "# Use -cache flag or set environment variable LINKLINT to\n" . "# to change this file's directory.\n\n";

    print CACHE $header;

    foreach (sort keys %hash)
    {
        print CACHE "$_ $hash{$_}\n";
    }

    close CACHE;
    &main'Progress(qq~\nWrote $type to "$file"~);
}

sub FlagOk
{
    $FlagOk{ $_[0] } ? 1 : 0;
}

sub FlagWarn
{
    $FlagWarn{ $_[0] } ? 1 : 0;
}

sub Base64Encode
{
    local ($plain) = @_;
    local ($out, @out);
    local (@bits) = split(//, unpack('B*', $plain));
    while (@bits > 0)
    {
        $out = join('', splice(@bits, 0, 6));
        while (length($out) < 6) { $out .= '0' }
        push(@out, $Base64[hex(unpack('H*', pack('B8', "00$out")))]);
    }
    while (@out % 4) { push(@out, "="); }
    join('', @out);
}

sub d_u_m_m_y { $main'FileList = 0 }

1;    # required packages must return true

#==========================================================================
# End of linkhttp.pl
#==========================================================================
