namespace APIDocumentationGenerator
{
    public static class HugeTemplate
    {
        internal static string GetTemplate()
        {
            return
@"<!-- #BeginTemplate ""/Templates/manual-scriptref-page.dwt"" --><!DOCTYPE html PUBLIC ""-//W3C//DTD XHTML 1.0 Transitional//EN"" ""http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd"">
<html xmlns=""http://www.w3.org/1999/xhtml"">
<head>
    <!-- #TemplateBeginEditable name=""doctitle"" -->
            <title>Unity Script Reference &ndash; [TYPENAME].[MEMBERNAME]</title>
    <!-- #TemplateEndEditable -->
<link rel=""StyleSheet""  href=""docs.css"" type=""text/css"" />
<script type=""text/javascript"" src=""../Images/jquery-1.4.2.js""></script>
<script type=""text/javascript"" src=""../Images/jquery.cookie.js""></script>
<meta http-equiv=""Content-Type"" content=""text/html;charset=UTF-8"">
<style>
/*
Unity standard fonts
*/
body{ font-family:Helvetica, Arial, sans-serif; font-size:.9em; }

/*
simple format for code, using fixed width
and a nice border
*/
pre { font-family:""Courier New"", Courier, monospace; font-size:.8em; }
pre { padding:10px; margin:10px; border:1px #d8d8d8 solid; }
pre { -moz-border-radius:10px; -webkit-border-radius:10px; }
pre { background:#f8f8f8; }

/*
aligns the selection dialog on the top right
*/
.selectLang { margin-right:10px; text-align:right; float:right; }


.cSelect,
.cSelect ul,
.cSelect li { font-size:12px; }

/*.cSelect { border:1px solid #d0d0d0; }
.cSelect { background:#ddf; }*/
/*
.cSelect { -moz-border-radius:10px; -webkit-border-radius:10px; }*/

.cSelect { display:inline-block; position:relative; float:right; margin-right:20px;}
.cSelect { padding-top:.4em; padding-bottom:.2em;}
.cSelect-Selected,
.cSelect-ArrowDown,
.cSelect li { cursor:pointer; }

.cSelect,
.roundTop { -webkit-border-top-left-radius:10px; -webkit-border-top-right-radius:10px; }
.cSelect,
.roundTop { -moz-border-radius-topleft:10px; -moz-border-radius-topright:10px; }

.roundBottom,
.cSelect ul { -webkit-border-bottom-left-radius:10px; -webkit-border-bottom-right-radius:10px; }
.roundBottom,
.cSelect ul { -moz-border-radius-bottomleft:10px; -moz-border-radius-bottomright:10px; }


.cSelectWidth { width:10em; display:block;}

.cSelect-wrapper { position:relative; }

.cSelect ul { background:#fff; padding:0px; margin:0px; display:inline-block; }
.cSelect ul { z-index:10; position:absolute; top:1.3em; }
.cSelect ul { padding-bottom:.5em; padding-top:.5em; padding-left:0px; padding-right:0px; }
/*.cSelect ul { border:1px solid #d0d0d0; }*/


.cSelect li { padding:.1em; margin:0px; text-align:right; list-style-type:none; }
.cSelect li { padding-left:.7em; padding-right:24px; }

.cSelect-highlight { background:#f0f0f0; }

.cSelect-Selected { margin-left:.5em; margin-right:.5em; text-align:right; float:right; width:75%}

.cSelect-ArrowDown { width:18px; height:18px; display:inline-block; float:right}
.cSelect-ArrowDown { background:url('../Images/down-arrow.png'); text-align:center; }
.cSelect-ArrowDown { -moz-border-radius:9px; -webkit-border-radius:9px; }

.monoCompatibility-roweven { background-color:#DCDDDF; }
.monoCompatibility-rowodd { background-color:#F3F3F4; }
.monoCompatibility-cell { border:1px solid #ffffff;}


</style>
</head>

<body>


<table class=""titlebar"" cellpadding=0 cellspacing=0 border=0><tr>
    <td class=""titleleft""><img src=""../images/top/left.png""></td>
    <td><a href=""../../Documentation.html""><img src=""../images/top/logo.png""></a></td>
    <td class=""titlemid"">
        <table><tr>
            <td class=""doctitle""><a href='[TYPENAME].html' class='classlink' title='Go to [TYPENAME] scripting documentation'>[TYPENAME]</a>.[MEMBERNAME]</td>
            <!-- #TemplateBeginEditable name=""sections-nav"" -->
            <td class=""Scripting""><a href=""../Manual/index.html"" title=""Go to Unity manual"" class=""scripting-anchor""><img src=""../images/spacer.gif"" class=""manual"" border=""0""><span class=""manual-text"">Manual</span></a><span class=""docs-navigation"">&nbsp;&nbsp;&nbsp;&nbsp;</span></td>
            <td class=""Scripting""><a href=""../Components/index.html"" title=""Go to Reference"" class=""scripting-anchor""><img src=""../images/spacer.gif"" class=""reference"" border=""0""><span class=""components-text"">Reference</span></a><span class=""docs-navigation"">&nbsp;&nbsp;&nbsp;&nbsp;</span></td>
            <td class=""Scripting""><a href=""../ScriptReference/index.html"" title=""Go to Scripting Reference"" class=""scripting-anchor""><img src=""../images/spacer.gif"" class=""scripting"" border=""0""><span class=""scripting-text"">Scripting&nbsp;&nbsp;</span></a></td>
            <!-- #TemplateEndEditable -->
        </tr><tr>
            <td colspan=""4"">
                <table width=""100%"" class=""docpath""><tr><td >
                    <!-- #TemplateBeginEditable name=""path"" -->
                        <a href=""index.html"">Scripting</a> &gt <a href='20_class_hierarchy.html'>Runtime Classes</a> &gt <a href='[TYPENAME].html'>[TYPENAME]</a>
                    <!-- #TemplateEndEditable -->
                </td>
                </td><td class=""switchlink"">
                    <!-- #TemplateBeginEditable name=""switchLink"" -->
                    <!--BeginSwitchLink--><!--EndSwitchLink-->
                    <!-- #TemplateEndEditable -->
                </td></tr></table>
            </td>
        </tr></table>
    </td>
    <td class=""titleright"" width=""9""><img src=""../images/top/right.png""></td>
</tr></table>
<div class=""scriprefmain"">
        <span class=""heading"">
            <!-- #TemplateBeginEditable name=""title"" -->
                <a href='[TYPENAME].html' class='classlink' title='Go to [TYPENAME] scripting documentation'>[TYPENAME]</a>.[MEMBERNAME]
            <!-- #TemplateEndEditable -->
        </span>
<!-- #TemplateBeginEditable name=""details"" -->
    <span class=""text""></span>
<!-- #TemplateEndEditable -->

    <!-- #TemplateBeginEditable name=""manual-top-nav"" --><div class=""left-menu-container""><ul class=""left-menu""><li>
<form class=""apisearch"" action=""30_search.html"" method=""GET"">
    <input type=""search"" name=""q"" placeholder=""Search"" autosave=""Unity Reference"" results=""5"" class=""sbox"" id=""q"" />

    <script type=""text/javascript"" language=""JavaScript"">
    <!--
    function detect() {
        // simplify things
        var agent   = navigator.userAgent.toLowerCase();

        // detect platform
        this.isMac      = (agent.indexOf('mac') != -1);
        this.isWin      = (agent.indexOf('win') != -1);
        this.isWin2k    = (this.isWin && (
                agent.indexOf('nt 5') != -1));
        this.isWinSP2   = (this.isWin && (
                agent.indexOf('xp') != -1 ||
                agent.indexOf('sv1') != -1));
        this.isOther    = (
                agent.indexOf('unix') != -1 ||
                agent.indexOf('sunos') != -1 ||
                agent.indexOf('bsd') != -1 ||
                agent.indexOf('x11') != -1 ||
                agent.indexOf('linux') != -1);

        // detect browser
        this.isSafari   = (agent.indexOf('safari') != -1);
        this.isSafari2 = (this.isSafari && (parseFloat(agent.substring(agent.indexOf(""applewebkit/"")+""applewebkit/"".length,agent.length).substring(0,agent.substring(agent.indexOf(""applewebkit/"")+""applewebkit/"".length,agent.length).indexOf(' '))) >=  300));
        this.isOpera    = (agent.indexOf('opera') != -1);
        this.isNN       = (agent.indexOf('netscape') != -1);
        this.isIE       = (agent.indexOf('msie') != -1);
    }
    var browser = new detect();
    var str = ""Search"";
    var q=document.getElementById('q');
    if (!browser.isSafari2) {q.value=str;q.onfocus=function(){q.value="""";q.style.color=""#000"";};}
    //-->
    </script>
</form>
</li><li>[MENUBAR]
    <!-- #TemplateEndEditable -->
            <!-- #TemplateBeginEditable name=""body"" -->
            <div class=""manual-entry""><h3>[SIGNATURE]
</h3>[PARAMETERS]<h3 class=""soft"">Description</h3>
   [DESCRIPTION]
<div class=""entry-sep""></div>
</div>
    </div><!-- #TemplateEndEditable -->
    <script type=""text/javascript"">

    //prevent an error on older versions of IE(x) < IE7 who did not have a javascript console.
    if (typeof console == ""undefined"" || typeof console.log == ""undefined"") var console = { log: function() {} };


    var $j = jQuery.noConflict();
    $j(document).ready(function($){

        var languages = [];
        function getCookie(cookieName){
    //a wrapper, because google chrome does not allow for security reasons cookies on local files.
    //an option can be set while loading chrome to allow cookies on local files
            try {
                return $.cookie(cookieName);
            }catch(e){
                console.log(e);
            }
        }

        function setCookie(cookieName, cookieValue){
    //a wrapper, because google chrome does not allow for security reasons cookies on local files.
    //an option can be set while loading chrome to allow cookies on local files
            try{
                return $.cookie(cookieName, cookieValue);
            }catch(e){
                console.log(e);
            }
        }

    /*
    decectLangauges looks at all of the div(s) containing a class of ""code""
    it then creates a list of languages in that list to generate the drop down.
    to ad a language, ad an example with that language defined
    in the attribute ""code_lang_name"" for that div
    */
        function detectLanguages(){
            languages = [];
            $(""pre.code"").each(
                function(){
                    var isThere = false;
    //set the value of the attribute
                    var isNewLang = $(this).attr(""code_lang_name"");
                    for ( i in languages ){
                        if( isNewLang == languages[i] ){
                            isThere = true;
                        }
                    }
                    if( !isThere ){
    //adds a new language to the stack/ array
                       languages.push(isNewLang);
                    }
                });
            console.log(""languages detected"", languages);
        }

    //execute the function detectLanguages just after we defined it, this should only happen once and on page load after the document object model is ready
        detectLanguages();

        function buildListCSelect(){
    //finds the doc element with an id of selectedLangauge, base on css3 rules, then removes all children
            $("".cSelect ul"").empty();
            $("".cSelect .cSelect-Selected"").text("""");
            var langCookie = getCookie(""codeLanguage"");
            for( i in languages ){
    //inserts a new child with the included html
                $("".cSelect ul"").append(""<li>"" + languages[i] + ""</li>"");
            }

    //if the cookie is there, set that item to selected.
            for( i in languages ){
                if(langCookie !== null && languages[i] == langCookie){
                    $("".cSelect .cSelect-Selected"").text(languages[i]);
                }
            }

            if( $("".cSelect .cSelect-Selected"").text().length == 0){
                var first = $("".cSelect li:first"").text();
                $("".cSelect .cSelect-Selected"").text(first);
            }

        }
        buildListCSelect();

        function showCSelectedLanguage(){
            var selected = $("".cSelect .cSelect-Selected:first"").text();
            var langAvail = false;
            var langCookie = getCookie(""codeLanguage"");
            for( i in languages ){
                if(langCookie == languages[i]){
                    langAvail = true;
                }
            }
            if(langAvail) {
                selected = langCookie;
            } else {
                console.log(""the language stored in the cookie is not availible"", langCookie);
            }
            $(""pre.code[code_lang_name]"").hide();
            $(""pre.code[code_lang_name='""+ selected +""']"").show();
        }
        showCSelectedLanguage(); //set the default.
           $("".cSelect"").addClass(""roundBottom"");
        $("".cSelect ul"").hide();
    /*  $("".cSelect"").hover(
            function(){
                $(this).find(""ul"").show();
            },
            function(){
                $(this).find(""ul"").hide();
            }
        );
*/
        $("".cSelect"").hover(
            function(){
                $(document).unbind(""click.cSelect"");
            },
            function(){
                $(document).bind(""click.cSelect"", function(event){
                                   $("".cSelect"").addClass(""roundBottom"");
                    $("".cSelect ul"").hide();
                    $(this).unbind(event);
                });
            }
        );


        $("".cSelect li"").hover(
            function(){
                $(this).addClass(""cSelect-highlight"");
            },
            function(){
                $(this).removeClass(""cSelect-highlight"");
            }
        );

        var suppress = false;
        function enable(){
            suppress = false;
        }


        $("".cSelect li"").click(function(){
            var value = $(this).text();
            console.log( ""new selection "", value );
            $(this).parents("".cSelect"").find("".cSelect-Selected"").text(value);
            suppress = true;
                   $("".cSelect"").addClass(""roundBottom"");
            $("".cSelect ul"").hide();
            $(this).parents("".cSelect"").trigger(""change"");
            setTimeout(enable, 300 );
        });


           $("".cSelect-ArrowDown"")
            .add("".cSelect-Selected"")
            .add("".cSelect"").click(
            function(){
                var visible = $(this).parents("".cSelect"").find(""ul"").css(""display"") == ""block"" ? true : false;
                if( !visible ){
                    $(this).parents("".cSelect"").removeClass(""roundBottom"");
                    $(this).parents("".cSelect"").find(""ul"").show();
                }else{
                    $(this).parents("".cSelect"").addClass(""roundBottom"");
                    $("".cSelect ul"").hide();
                }
            });


        $("".cSelect"").click(function(){if(!suppress){}});

        $("".cSelect"").change(function(){
            console.log(""selected"",$(this).find("".cSelect-Selected"").text());
            selected = $(this).find("".cSelect-Selected"").text();
            $("".cSelect"").find("".cSelect-Selected"").text(selected);
            $(""pre.code[code_lang_name]"").hide();
            $(""pre.code[code_lang_name='""+ selected +""']"").show();
            setCookie(""codeLanguage"", selected);
        });
    });
    </script>
</body>
</html><!-- #EndTemplate -->

";
        }
    }
}
