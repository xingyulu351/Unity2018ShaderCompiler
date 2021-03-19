<?xml version="1.0" encoding="utf-8"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
    <xsl:import href='classListing.xsl'/>
    <xsl:param name="Lang"/>
    <xsl:template match='/'>
        <xsl:text disable-output-escaping='yes'>&lt;!DOCTYPE html></xsl:text>
        <html lang="en" class="no-js">
            <head>
                <!-- Google Tag Manager -->
                <script>var offline=(location.href.indexOf('docs.unity3d.com')==-1)?true:false;if(!offline){(function(w,d,s,l,i){w[l]=w[l]||[];w[l].push({'gtm.start': new Date().getTime(),event:'gtm.js'});var f=d.getElementsByTagName(s)[0], j=d.createElement(s),dl=l!='dataLayer'?'&amp;l='+l:'';j.async=true;j.src= 'https://www.googletagmanager.com/gtm.js?id='+i+dl;f.parentNode.insertBefore(j,f); })(window,document,'script','dataLayer','GTM-5V25JL6');}</script>
                <!-- End Google Tag Manager -->

                <meta charset="utf-8" />
                <meta http-equiv="X-UA-Compatible" content="IE=edge,chrome=1" />
                <meta name="viewport" content="width=device-width, initial-scale=1.0" />
                <title>Unity - <xsl:value-of select="/Root/website_terms/scripting_api" />: <xsl:if test='/Root/Model/ParentClass'>
                        <xsl:value-of select='/Root/Model/ParentClass' />.</xsl:if>
                    <xsl:value-of select='/Root/Model/Section/Signature/Declaration/@name' />
                </title>
                <meta property="og:image" content="https://unity3d.com/files/images/ogimg.jpg" />
                <meta name="author" content="{/Root/website_terms/unity_tech}" />
                <link rel="shortcut icon" href="https://unity.com/themes/contrib/unity_base/images/favicons/favicon.ico" />
                <!-- <link rel="shortcut icon" href="../StaticFiles/images/favicons/favicon.ico" /> -->
                <link rel="icon" type="image/png" href="../StaticFiles/images/favicons/favicon.png" />
                <link rel="apple-touch-icon-precomposed" sizes="152x152" href="../StaticFiles/images/favicons/apple-touch-icon-152x152.png"/>
                <link rel="apple-touch-icon-precomposed" sizes="144x144" href="../StaticFiles/images/favicons/apple-touch-icon-144x144.png"/>
                <link rel="apple-touch-icon-precomposed" sizes="120x120" href="../StaticFiles/images/favicons/apple-touch-icon-120x120.png"/>
                <link rel="apple-touch-icon-precomposed" sizes="114x114" href="../StaticFiles/images/favicons/apple-touch-icon-114x114.png"/>
                <link rel="apple-touch-icon-precomposed" sizes="72x72" href="../StaticFiles/images/favicons/apple-touch-icon-72x72.png"/>
                <link rel="apple-touch-icon-precomposed" href="../StaticFiles/images/favicons/apple-touch-icon.png"/>
                <meta name="msapplication-TileColor" content="#222c37"/>
                <meta name="msapplication-TileImage" content="../StaticFiles/images/favicons/tileicon-144x144.png"/>

                <script type="text/javascript" src="https://docs.unity3d.com/StaticFilesConfig/UnityVersionsInfo.js"/>

                <script type="text/javascript" src="../StaticFiles/js/jquery.js"/>
                <script type="text/javascript" src="docdata/toc.js">//toc</script>
                <xsl:comment>local TOC</xsl:comment>
                <script type="text/javascript" src="docdata/global_toc.js">//toc</script>
                <xsl:comment>global TOC, including other platforms</xsl:comment>
                <script type="text/javascript" src="../StaticFiles/js/core.js"/>

                <link rel="stylesheet" href="../StaticFiles/css/prism.css"/>
                <script type="text/javascript" src="../StaticFiles/js/prism.js"/>

                <xsl:if test="$Lang='ja'">
                    <script type="text/javascript" src="../StaticFiles/js/tinysegmenter.js"/>
                </xsl:if>
                <link rel="stylesheet" type="text/css" href="../StaticFiles/css/core.css"/>
  <link rel="stylesheet" href="../StaticFiles/js/feedback/five-star-rating-master/css/rating.min.css"/>
  <script src="../StaticFiles/js/feedback/five-star-rating-master/js/src/rating.js"></script>

                <script src="../StaticFiles/js/jquery.sidebar.min.js"/>

                <script type="text/javascript" src="../StaticFiles/js/mobileoptimisation.js"/>
                <link rel="stylesheet" href="../StaticFiles/css/mobileoptimisation.css"/>

            </head>
            <body>
                <!-- Google Tag Manager (noscript) -->
                <noscript>
                    <iframe src="https://www.googletagmanager.com/ns.html?id=GTM-5V25JL6" height="0" width="0" style="display:none;visibility:hidden"/>
                </noscript>
                <!-- End Google Tag Manager (noscript) -->
                <div class="header-wrapper">
                    <div id="header" class="header">
                        <div class="content">
                            <div class="spacer">
                                <div class="menu">
                                    <div id="nav-open" for="nav-input">
                                        <span></span>
                                    </div>
                                    <div class="logo">
                                        <xsl:element name="a">
                                            <xsl:attribute name="href">
                                                <xsl:value-of select="/Root/website_terms/docs_url" />
                                            </xsl:attribute>
                                        </xsl:element>
                                    </div>
                                    <div class="search-form">
                                        <form action="30_search.html" method="get" class="apisearch">
                                            <input type="text" name="q" placeholder="Search scripting..." autosave="Unity Reference" results="5" class="sbox field" id="q" />
                                            <input type="submit" class="submit" />
                                        </form>
                                    </div>
                                    <ul>
                                        <li>
                                            <a href="../Manual/index.html">
                                                <xsl:value-of select="/Root/website_terms/manual" />
                                            </a>
                                        </li>
                                        <li>
                                            <a href="../ScriptReference/index.html" class="selected">
                                                <xsl:value-of select="/Root/website_terms/scripting_api" />
                                            </a>
                                        </li>
                                    </ul>
                                </div>
                            </div>
                            <div class="more">
                                <div class="filler"/>
                                <ul>
                                    <li>
                                        <a href="https://unity.com/">unity.com</a>
                                    </li>
                                </ul>
                            </div>
                        </div>
                    </div>
                    <div class="toolbar">
                        <div class="content clear">
                            <div class="toggle version-number" id="VersionNumber" data-target=".otherversionscontent">
                                Version: <b>2018.4</b>
                                <div class="otherversionscontent" id="OtherVersionsContent" style="display: none;">
                                    <ul id="OtherVersionsContentUl">
                                    </ul>
                                    <div id="otherVersionsLegend">
                                        <ul>
                                            <li>
                                                <div id="supportedColour" class="legendBox"></div>Supported</li>
                                            <li>
                                                <div id="notFoundColour" class="legendBox"></div>Legacy</li>
                                        </ul>
                                    </div>
                                </div>
                                <div id="VersionSwitcherArrow" class="arrow versionSwitcherArrow"></div>
                            </div>
                            <div class="lang-switcher hide">
                                <div class="current toggle" data-target=".lang-list">
                                    <div class="lbl">
                                        <xsl:value-of select="/Root/website_terms/language" />
                                        <span class="b">English</span>
                                    </div>
                                    <div class="arrow"/>
                                </div>
                                <div class="lang-list" style="display:none;">
                                    <ul>
                                        <li>
                                            <a href="">English</a>
                                        </li>
                                    </ul>
                                </div>
                            </div>
                            <div class="script-lang">
                                <ul>
                                    <li class="selected" data-lang="CS">C#</li>
                                </ul>
                            </div>
                            <div class="mobileLogo">
                                <a href="https://docs.unity3d.com"></a>
                            </div>
                        </div>
                    </div>
                </div>
                <div id="master-wrapper" class="master-wrapper clear">
                    <div id="sidebar" class="sidebar">
                        <div class="sidebar-wrap">
                            <div class="content">
                                <div class="sidebar-menu">
                                    <div class="toc">
                                        <h2>
                                            <xsl:value-of select="/Root/website_terms/scripting_api" />
                                        </h2>
                                        <div class="search-form sidebar-search-form">
                                            <form action="30_search.html" method="get" class="apisearch">
                                                <input type="text" name="q" placeholder="Search manual..." autosave="Unity Reference" results="5" class="sbox field" id="q"/>
                                                <input type="submit" id="mobileSearchBtn" class="submit" value="Search"/>
                                            </form>
                                        </div>
                                        <div class="toggle version-number sidebar-version-switcher" id="VersionNumber" data-target=".otherversionscontent">
                                            <form id="otherVersionsContentMobileForm">
                                                <div class="ui-field-contain">
                                                    <label for="select-native-4">Version: 2018.4</label>
                                                    <select name="select-native-4" id="versionsSelectMobile">
                                                        <option>Select a different version</option>
                                                        <optgroup id="versionsWithThisPageMobile" label="Versions with this page">

                                                        </optgroup>
                                                        <optgroup id="versionsWithoutThisPageMobile" label="Versions without this page">

                                                        </optgroup>
                                                    </select>
                                                </div>
                                            </form>
                                        </div>
                                        <div class="lang-switcher hide">
                                            <div class="current toggle" data-target=".lang-list">
                                                <div class="lbl">
                                                    <xsl:value-of select="/Root/website_terms/language" />
                                                    <span class="b">English</span>
                                                </div>
                                                <div class="arrow"/>
                                            </div>
                                            <div class="lang-list" style="display:none;">
                                                <ul>
                                                    <li>
                                                        <a href="">English</a>
                                                    </li>
                                                </ul>
                                            </div>
                                        </div>
                                    </div>
                                </div>
                            </div>
                        </div>
                    </div>
                    <div id="content-wrap" class="content-wrap">
                        <div class="content-block">
                            <div class="content">
                                <div class="section">
                                    <xsl:apply-templates select="Root/*[not(self::website_terms)]"/>
                                    <xsl:apply-templates select="items"/>
                                    <xsl:apply-templates select="TOC"/>
                                </div>
                                <div class="section">
                                    <!-- start of feedback box -->
                                    <div class="feedbackbox" id="feedbackbox">
                                        <div id="rating"><p>
                                            <xsl:value-of select="/Root/website_terms/rate_content" />
                                            <br/>
                                            <div id="ratecontent" class="c-rating"/>
                                        </p></div>
                                        <div id="ratingThanks" style="display:none" ><p>
                                            <xsl:value-of select="/Root/website_terms/rate_thanks" />
                                        </p></div>
                                        <div id="problem"><p><a name="problem"><xsl:value-of select="/Root/website_terms/report_problem" /></a></p></div>
                                        <div id="problemType" style="display:none"><p>
                                            <xsl:value-of select="/Root/website_terms/problem_type" />
                                            <ul type="problems">
                                                <li><a name="needcode" id="problemneedcode"><xsl:value-of select="/Root/website_terms/problem_needcode" /></a></li>
                                                <li><a name="code" id="problemcode"><xsl:value-of select="/Root/website_terms/problem_code" /></a></li>
                                                <li><a name="missing"  id="problemmissing"><xsl:value-of select="/Root/website_terms/problem_missing" /></a></li>
                                                <li><a name="incorrect" id="problemincorrect"><xsl:value-of select="/Root/website_terms/problem_incorrect" /></a></li>
                                                <li><a name="unclear" id="problemunclear"><xsl:value-of select="/Root/website_terms/problem_unclear" /></a></li>
                                                <li><a name="language" id="problemlanguage"><xsl:value-of select="/Root/website_terms/problem_language" /></a></li>
                                                <li><a name="other" id="problemother"><xsl:value-of select="/Root/website_terms/problem_other" /></a></li>
                                            </ul><p><xsl:copy-of select="/Root/website_terms/known_issues" /></p>
                                        </p></div>
                                        <div id="problemThanks" style="display:none"><p>
                                            <xsl:value-of select="/Root/website_terms/problem_thanks" /><br/><br/>
                                            <xsl:value-of select="/Root/website_terms/problem_more_info" /><br/><br/>
                                            <a id="problemThanksMoreInfoButton"><xsl:value-of select="/Root/website_terms/problem_more_info_link" /></a><br/>
                                        </p></div>
                                        <div id="problemMoreInfo" style="display:none">
                                            <p id="problemNeedCodeForm" style="display:none"><xsl:value-of select="/Root/website_terms/problem_needcode_form_description" /></p>
                                            <p id="problemCodeForm" style="display:none"><xsl:value-of select="/Root/website_terms/problem_code_form_description" /></p>
                                            <p id="problemMissingForm" style="display:none"><xsl:value-of select="/Root/website_terms/problem_missing_form_description" /></p>
                                            <p id="problemIncorrectForm" style="display:none"><xsl:value-of select="/Root/website_terms/problem_incorrect_form_description" /></p>
                                            <p id="problemUnclearForm" style="display:none"><xsl:value-of select="/Root/website_terms/problem_unclear_form_description" /></p>
                                            <p id="problemLanguageForm" style="display:none"><xsl:value-of select="/Root/website_terms/problem_language_form_description" /></p>
                                            <p id="problemOtherForm" style="display:none"><xsl:value-of select="/Root/website_terms/problem_other_form_description" /></p>
                                            <form>
                                            <textarea id="problemFormSuggestionField" cols="40" rows="5"></textarea>
                                            <input type="hidden" id="problemFormDescription" />
                                            <input type="submit" id="problemFormDescriptionSubmit" value="Submit"/>
                                            </form>
                                        </div>
                                        <div id="problemMoreInfoThanks" style="display:none"><p><xsl:value-of select="/Root/website_terms/problem_more_info_thanks" /></p></div>
                                        <script>InitialiseStarRating();</script>
                                    </div>
                                    <!-- end of feedback box -->
                                </div>
                                <div class="footer-wrapper">
                                    <div class="footer clear">
                                        <div class="copy">
                                            <p>
                                                <xsl:copy-of select="/Root/website_terms/known_issues" />
                                                <xsl:element name="a">
                                                    <xsl:attribute name="href">
                                                        <xsl:value-of select="/Root/website_terms/issuetracker_url" />
                                                    </xsl:attribute>
                                                    <xsl:value-of select="/Root/website_terms/issuetracker_title" />
                                                </xsl:element>.</p>
                                            <xsl:value-of select="/Root/website_terms/copyright" />. Publication: 2018.4-003D. Built: 2019-10-28.</div>
                                        <div class="menu">
                                            <xsl:element name="a">
                                                <xsl:attribute name="href">
                                                    <xsl:value-of select="/Root/website_terms/tutorials_url" />
                                                </xsl:attribute>
                                                <xsl:value-of select="/Root/website_terms/tutorials" />
                                            </xsl:element>
                                            <xsl:element name="a">
                                                <xsl:attribute name="href">
                                                    <xsl:value-of select="/Root/website_terms/community_answers_url" />
                                                </xsl:attribute>
                                                <xsl:value-of select="/Root/website_terms/community_answers" />
                                            </xsl:element>
                                            <xsl:element name="a">
                                                <xsl:attribute name="href">
                                                    <xsl:value-of select="/Root/website_terms/knowledgebase_url" />
                                                </xsl:attribute>
                                                <xsl:value-of select="/Root/website_terms/knowledgebase" />
                                            </xsl:element>
                                            <xsl:element name="a">
                                                <xsl:attribute name="href">
                                                    <xsl:value-of select="/Root/website_terms/forums_url" />
                                                </xsl:attribute>
                                                <xsl:value-of select="/Root/website_terms/forums" />
                                            </xsl:element>
                                            <xsl:element name="a">
                                                <xsl:attribute name="href">
                                                    <xsl:value-of select="/Root/website_terms/assetstore_url" />
                                                </xsl:attribute>
                                                <xsl:value-of select="/Root/website_terms/asset_store" />
                                            </xsl:element>
                                        </div>
                                    </div>
                                </div>
                            </div>
                        </div>
                    </div>
                </div>
            </body>
        </html>
    </xsl:template>
</xsl:stylesheet>
