<?xml version="1.0" encoding="utf-8"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:import href="styling.xsl"/>

<xsl:template match='Model'>
  <xsl:for-each select='Model'><xsl:apply-templates select='.' /></xsl:for-each>
</xsl:template>

<xsl:template match='Model'>
  <xsl:if test='Section'>
    <xsl:if test='(position() > 2) and (/Root/@LongForm="true")'><hr/></xsl:if>
    <div class="mb20 clear">
      <xsl:attribute name="id">
        <xsl:value-of select="@id"/>
      </xsl:attribute>
      <xsl:if test='(@status="obsolete" or @status="removed")'>
        <div class="message message-error mb20">
          <xsl:if test='@status="obsolete"'><b>Method group is Obsolete</b><br/></xsl:if>
          <xsl:if test='@status="removed"'><b>Removed<xsl:if test='../Model/@version'> in version <xsl:value-of select='../Model/@version'/></xsl:if></b><br/></xsl:if>
          <xsl:if test='(@status="obsolete" or @status="removed") and ../Model/@obsoleteText'>
            <xsl:value-of select='../Model/@obsoleteText'/>
          </xsl:if>
        </div>
      </xsl:if>
	  <!-- Add a note for experimental APIs. Depending on which page we're in (class overview itself, or class member), different things in the model contain the namespace: it's either the "namespace" or the "parent class". -->
	  <xsl:if test='(contains(../Model/@Namespace, "Experimental") or contains(ParentClass, "Experimental"))'>
		<div class="message message-error mb20"><b>Experimental</b>: this API is experimental and might be changed or removed in the future.</div>
	  </xsl:if>
      <h1 class="heading inherit">
		  <xsl:if test="ParentClass != '' and Section/Signature/Declaration/@type != 'constructor'">
			  <xsl:choose>
				  <xsl:when test="ParentClass/@DisplayName != ''">
					  <a href='{ParentClass}.html'>
						  <xsl:value-of select='ParentClass/@DisplayName' />
					  </a>.</xsl:when>
				  <xsl:otherwise>
					  <a href='{ParentClass}.html'>
						  <xsl:value-of select='ParentClass' />
					  </a>.</xsl:otherwise>
			  </xsl:choose>
		  </xsl:if>
		  <xsl:value-of select='Section/Signature/Declaration/@name' />
		<xsl:if test="Section/Signature/Declaration/@type = 'constructor'"> Constructor</xsl:if>
	  </h1>
      <div class="clear"></div>
      <xsl:if test='Section/Signature/Declaration/@type = "class"
      or Section/Signature/Declaration/@type = "struct"
      or Section/Signature/Declaration/@type = "interface"
      or Section/Signature/Declaration/@type = "enumeration"'>
        <p class="cl mb0 left mr10"><xsl:value-of select='Section/Signature/Declaration/@type'/>
        <xsl:if test='../Model/@Namespace'> in
          <xsl:value-of select='../Model/@Namespace'/>
        </xsl:if></p>
        <xsl:if test='BaseType/@id'>
           <p class="cl mb0 left mr10">/</p>
           <p class="cl mb0 left"><xsl:value-of select="/Root/website_terms/inherits_from" /> <a href='{BaseType/@id}.html' class='cl'><xsl:value-of select="BaseType/@id"/></a></p>
        </xsl:if>
        <xsl:if test='../Model/@Assembly'>
          <p class="cl mb0 left mr10">/</p>
          <p class="cl mb0 left"><xsl:value-of select="/Root/website_terms/implemented_in" /> <a href='{../Model/@Assembly}.html' class='cl'><xsl:value-of select='../Model/@Assembly'/></a></p>
        </xsl:if>
      </xsl:if>
      <div class="scrollToFeedback">
        <a id="scrollToFeedback"><xsl:value-of select="/Root/website_terms/leave_feedback" /></a>
      </div>

	  <xsl:if test='ImplementedInterfaces'>
		  <br/>
		  <p class="cl mb0 left">
			  <xsl:value-of select="/Root/website_terms/implements_interfaces" />
			  <xsl:for-each select='ImplementedInterfaces/member'>
				  <a href='{@id}.html' class='cl'>
					  <xsl:value-of select='name'/>
				  </a>
				  <xsl:if test="position() != last()">
					  <xsl:text>, </xsl:text>
				  </xsl:if>
			  </xsl:for-each>
		  </p>
	  </xsl:if>
	  <div class="clear"></div>
      <div class="suggest">
        <a class="blue-btn sbtn"><xsl:value-of select="/Root/website_terms/suggest_a_change" /></a>
        <div class="suggest-wrap rel hide">
          <div class="loading hide"><div></div><div></div><div></div></div>
          <div class="suggest-success hide">
            <h2><xsl:value-of select="/Root/website_terms/suggest_success" /></h2>
            <p><xsl:value-of select="/Root/website_terms/suggest_success_body" /></p>
            <a class="gray-btn sbtn close"><xsl:value-of select="/Root/website_terms/suggest_clone" /></a>
          </div>
          <div class="suggest-failed hide">
            <h2><xsl:value-of select="/Root/website_terms/suggest_failed" /></h2>
            <p><xsl:value-of select="/Root/website_terms/suggest_failed_body" /></p>
            <a class="gray-btn sbtn close"><xsl:value-of select="/Root/website_terms/suggest_clone" /></a>
          </div>
          <div class="suggest-form clear">
            <label for="suggest_name"><xsl:value-of select="/Root/website_terms/suggest_name" /></label>
            <input id="suggest_name" type="text" />
            <label for="suggest_email"><xsl:value-of select="/Root/website_terms/suggest_email" /></label>
            <input id="suggest_email" type="email" />
            <label for="suggest_body" class="clear"><xsl:value-of select="/Root/website_terms/suggest_body" /> <span class="r">*</span></label>
            <textarea id="suggest_body" class="req"></textarea>
            <button id="suggest_submit" class="blue-btn mr10"><xsl:value-of select="/Root/website_terms/suggest_submit" /></button>
            <p class="mb0"><a class="cancel left lh42 cn"><xsl:value-of select="/Root/website_terms/suggest_cancel" /></a></p>
           </div>
        </div>
      </div>
      <a href="" class="switch-link gray-btn sbtn left hide"><xsl:value-of select="/Root/website_terms/switch_to_manual" /></a>
      <div class="clear"></div>
    </div>
  </xsl:if>
  <xsl:apply-templates select='Intro' />
  <xsl:apply-templates select='Section' />
  <xsl:apply-templates select='Search' />
  <xsl:apply-templates select='StaticVars|Vars|ProtVars|StaticProtVars|Constructors|StaticFunctions|MemberFunctions|ProtectedFunctions|Operators|Messages|Delegates|Events|Classes|Structs|Enumerations' />
  <xsl:apply-templates select='BaseType'/>
  <xsl:apply-templates select='Feedback' />
</xsl:template>

<xsl:template match='Section'>
  <xsl:if test='position() > 1'><hr class='section'/></xsl:if>
  <div class="subsection">
    <div class="signature">
      <xsl:apply-templates select='Signature' />
    </div>
  </div>

  <xsl:if test='ParamWithDoc and ParamWithDoc[1]/doc/child::text()'>
    <div class="subsection">
      <h2><xsl:value-of select="/Root/website_terms/parameters" /></h2>
      <table class="list">
        <xsl:apply-templates select='ParamWithDoc' />
      </table>
    </div>
  </xsl:if>

  <xsl:if test='ReturnWithDoc'>
    <div class="subsection">
      <h2><xsl:value-of select="/Root/website_terms/returns" /></h2>
      <p><xsl:apply-templates select='ReturnWithDoc' /></p>
    </div>
  </xsl:if>

  <div class="subsection">
    <h2><xsl:value-of select="/Root/website_terms/description" /></h2>
    <xsl:apply-templates select='Summary' />
  </div>

  <xsl:apply-templates select='Description|Example' />

</xsl:template>

<!-- Signatures and parameters -->
<xsl:template match='ParamWithDoc'>
  <xsl:if test='doc != ""'>
    <tr>
      <td class="name lbl"><xsl:value-of select='name' /></td>
      <td class="desc"><xsl:apply-templates select='doc' /></td>
    </tr>
  </xsl:if>
</xsl:template>

<xsl:template match='doc'><xsl:apply-templates /></xsl:template>

<xsl:template match='ReturnWithDoc'>
  <strong><xsl:value-of select='@type' /></strong> <xsl:apply-templates />
</xsl:template>

<xsl:template match='Signature'>
  <div class='signature-CS sig-block'>
    <span style='color:red;'>
      <xsl:value-of select='Declaration/@Obsolete' />
    </span>
    <xsl:if test='Declaration/@type="function" or Declaration/@type="delegate" or Declaration/@type="constructor" or Declaration/@type="operator"'>
      <xsl:if test='Declaration/@modifiers'><xsl:value-of select='concat(Declaration/@modifiers, " ")' />
		  <xsl:if test='Declaration/@type="delegate"'>delegate </xsl:if>
	  </xsl:if>
      <xsl:if test='Declaration/@type != "constructor"'>
        <xsl:if test='ReturnType/@hasLink'><a href='{ReturnType}.html'><xsl:value-of select='ReturnType' /></a><xsl:text> </xsl:text></xsl:if>
        <xsl:if test='not(ReturnType/@hasLink)'><xsl:value-of select='concat(ReturnType, " ")' /></xsl:if>
      </xsl:if>
      <span class='sig-kw'><xsl:value-of select='Declaration/@name' /></span>(<xsl:apply-templates select='ParamElement' mode='CS' />);
    </xsl:if>
    <xsl:if test='Declaration/@type="var"'>
      <xsl:if test='Declaration/@modifiers'><xsl:value-of select='concat(Declaration/@modifiers, " ")' /></xsl:if>
      <xsl:if test='ReturnType/@hasLink'><a href='{ReturnType}.html'><xsl:value-of select='ReturnType' /></a></xsl:if>
      <xsl:if test='not(ReturnType/@hasLink)'><xsl:value-of select='ReturnType' /></xsl:if>
      <xsl:text> </xsl:text>
      <span class='sig-kw'><xsl:value-of select='Declaration/@name' /></span>;
    </xsl:if>
  </div>
</xsl:template>

<xsl:template match='ParamElement[1]' mode='JS'><xsl:if test='@modifier'><xsl:value-of select='concat(@modifier, " ")' />
 </xsl:if><span class='sig-kw'><xsl:value-of select='@name' /></span>:
<xsl:if test='@hasLink'><a href='{@type}.html'><xsl:value-of select='@type' /></a></xsl:if><xsl:if test='not(@hasLink)'><xsl:value-of select='@type' /></xsl:if>
<xsl:if test='@default'> = <xsl:value-of select='@default'/></xsl:if>
</xsl:template>

<xsl:template match='ParamElement' mode='JS'>,
<xsl:if test='@modifier'><xsl:value-of select='concat(@modifier, " ")' /></xsl:if>
<span class='sig-kw'><xsl:value-of select='@name' /></span>: <xsl:if test='@hasLink'><a href='{@type}.html'><xsl:value-of select='@type' /></a></xsl:if><xsl:if test='not(@hasLink)'><xsl:value-of select='@type' /></xsl:if>
<xsl:if test='@default'> = <xsl:value-of select='@default'/></xsl:if>
</xsl:template>

<xsl:template match='ParamElement[1]' mode='CS'><xsl:if test='@modifier'><xsl:value-of select='concat(@modifier, " ")' />
 </xsl:if><xsl:if test='@hasLink'><a href='{@type}.html'><xsl:value-of select='@type' /></a><xsl:text> </xsl:text></xsl:if><xsl:if test='not(@hasLink)'><xsl:value-of select='concat(@type, " ")' /></xsl:if> <span class='sig-kw'><xsl:value-of select='@name' /></span>
<xsl:if test='@default'> = <xsl:value-of select='@default'/></xsl:if>
</xsl:template>

<xsl:template match='ParamElement' mode='CS'>,
<xsl:if test='@modifier'><xsl:value-of select='concat(@modifier, " ")' /></xsl:if>
<xsl:if test='@hasLink'><a href='{@type}.html'><xsl:value-of select='@type' /></a><xsl:text> </xsl:text></xsl:if><xsl:if test='not(@hasLink)'><xsl:value-of select='concat(@type, " ")' /></xsl:if> <span class='sig-kw'><xsl:value-of select='@name' /></span>
<xsl:if test='@default'> = <xsl:value-of select='@default'/></xsl:if>
</xsl:template>

<!-- Description elements -->
<xsl:template match='Summary'>
  <p><xsl:apply-templates /></p>
</xsl:template>

<xsl:template match='Description'>
  <div class="subsection">
    <p><xsl:apply-templates /></p>
  </div>
</xsl:template>

<!-- Code examples -->
<xsl:template match='Example'>
<div class="subsection"><xsl:apply-templates /></div>
</xsl:template>

<xsl:template match='Example/JavaScript'>
</xsl:template>

<xsl:template match='Example/CSharp'>
<pre class='codeExampleCS'><xsl:apply-templates /></pre>
</xsl:template>

<xsl:template match='Example/Unconverted'>
</xsl:template>

<!-- Members (only affects class pages) -->
<xsl:template match='StaticVars'>
  <xsl:if test='member'>
    <div class="subsection">
      <h2><xsl:value-of select="/Root/website_terms/static_variables" /></h2>
      <table class="list">
        <xsl:apply-templates />
      </table>
    </div>
  </xsl:if>
</xsl:template>

  <xsl:template match='Vars'>
    <xsl:if test='member'>
      <div class="subsection">
        <h2>
          <xsl:value-of select="/Root/website_terms/variables" />
        </h2>
        <table class="list">
          <xsl:apply-templates />
        </table>
      </div>
    </xsl:if>
  </xsl:template>

  <xsl:template match='ProtVars'>
    <xsl:if test='member'>
      <div class="subsection">
        <h2>
          Protected Properties
        </h2>
        <table class="list">
          <xsl:apply-templates />
        </table>
      </div>
    </xsl:if>
  </xsl:template>

  <xsl:template match='StaticProtVars'>
    <xsl:if test='member'>
      <div class="subsection">
        <h2>
          Static Protected Properties
        </h2>
        <table class="list">
          <xsl:apply-templates />
        </table>
      </div>
    </xsl:if>
  </xsl:template>

<xsl:template match='Constructors'>
  <xsl:if test='member'>
    <div class="subsection">
      <h2><xsl:value-of select="/Root/website_terms/constructors" /></h2>
      <table class="list">
        <xsl:apply-templates />
      </table>
    </div>
  </xsl:if>
</xsl:template>

<xsl:template match='MemberFunctions'>
  <xsl:if test='member'>
    <div class="subsection">
      <h2><xsl:value-of select="/Root/website_terms/public_functions" /></h2>
      <table class="list">
        <xsl:apply-templates />
      </table>
    </div>
  </xsl:if>
</xsl:template>

<xsl:template match='ProtectedFunctions'>
  <xsl:if test='member'>
    <div class="subsection">
      <h2><xsl:value-of select="/Root/website_terms/protected_functions" /></h2>
      <table class="list">
        <xsl:apply-templates />
      </table>
    </div>
  </xsl:if>
</xsl:template>

<xsl:template match='StaticFunctions'>
  <xsl:if test='member'>
    <div class="subsection">
      <h2><xsl:value-of select="/Root/website_terms/static_functions" /></h2>
      <table class="list">
        <xsl:apply-templates />
      </table>
    </div>
  </xsl:if>
</xsl:template>

<xsl:template match='Operators'>
  <xsl:if test='member'>
    <div class="subsection">
      <h2><xsl:value-of select="/Root/website_terms/operators" /></h2>
      <table class="list">
        <xsl:apply-templates />
      </table>
    </div>
  </xsl:if>
</xsl:template>

<xsl:template match='Messages'>
  <xsl:if test='member'>
    <div class="subsection">
      <h2><xsl:value-of select="/Root/website_terms/messages" /></h2>
      <table class="list">
        <xsl:apply-templates />
      </table>
    </div>
  </xsl:if>
</xsl:template>

<xsl:template match='Delegates'>
  <xsl:if test='member'>
    <div class="subsection">
      <h2><xsl:value-of select="/Root/website_terms/delegates" /></h2>
      <table class="list">
        <xsl:apply-templates />
      </table>
    </div>
  </xsl:if>
</xsl:template>

<xsl:template match='Events'>
  <xsl:if test='member'>
    <div class="subsection">
      <h2>Events</h2>
      <table class="list">
        <xsl:apply-templates />
      </table>
    </div>
  </xsl:if>
</xsl:template>

<xsl:template match='Classes'>
  <xsl:if test='member'>
    <div class="subsection">
      <h2><xsl:value-of select="/Root/website_terms/classes" /></h2>
      <table class="list">
        <xsl:apply-templates />
      </table>
    </div>
  </xsl:if>
</xsl:template>

<xsl:template match='Structs'>
  <xsl:if test='member'>
    <div class="subsection">
      <h2><xsl:value-of select="/Root/website_terms/structs" /></h2>
      <table class="list">
        <xsl:apply-templates />
      </table>
    </div>
  </xsl:if>
</xsl:template>

<xsl:template match='Enumerations'>
  <xsl:if test='member'>
    <div class="subsection">
      <h2><xsl:value-of select="/Root/website_terms/enumerations" /></h2>
      <table class="list">
        <xsl:apply-templates />
      </table>
    </div>
  </xsl:if>
</xsl:template>

<xsl:template match='member'>
  <tr>
    <td class='lbl'>
      <xsl:choose>
        <xsl:when test="/Root/@LongForm='true'">
          <a href='#{@id}'><xsl:value-of select='name' /></a>
        </xsl:when>
        <xsl:otherwise>
          <a href='{@id}.html'><xsl:value-of select='name' /></a>
        </xsl:otherwise>
      </xsl:choose>
    </td>
    <td class='desc'><xsl:value-of select='summary' /></td>
  </tr>
</xsl:template>

<xsl:template match='BaseType'>
  <div class="subsection">
    <h2><xsl:value-of select="/Root/website_terms/inherited_members" /></h2>
    <xsl:apply-templates/>
  </div>
</xsl:template>

<!-- Landing page elements. -->
<xsl:template match='Intro'>
  <h1><xsl:value-of select="/Root/website_terms/intro" /></h1>
  <p><xsl:value-of disable-output-escaping="yes" select="/Root/website_terms/intro_body_1" /></p>
  <p><xsl:value-of disable-output-escaping="yes" select="/Root/website_terms/intro_body_2" /></p>
  <p><xsl:value-of disable-output-escaping="yes" select="/Root/website_terms/intro_body_3" /></p>
  <p><xsl:value-of disable-output-escaping="yes" select="/Root/website_terms/intro_body_4" /></p>
</xsl:template>

<xsl:template match='Intro/heading'></xsl:template>

<!-- Search page -->
<xsl:template match='Search'>
  <div class='search-spinner'>
    <h2>Searching Script Reference, please wait.</h2><br/><br/>
    <img src="../StaticFilesManual/images/loading_spinner.gif"/>
  </div>
  <div class='search-results'>
    <script type='text/javascript' src='docdata/index.js'>//searchindex</script><xsl:comment>local search index</xsl:comment>
    <script type='text/javascript' src='docdata/global_index.js'>//searchindex</script><xsl:comment>global search index</xsl:comment>
    <script type='text/javascript'>PerformSearch();</script>
  </div>
</xsl:template>

<!-- Feedback page elements. -->
<xsl:template match='Feedback'>
  <h1><xsl:value-of select="/Root/website_terms/feedback_title" /></h1>
  <p><xsl:value-of disable-output-escaping="yes" select="/Root/website_terms/feedback_body" /></p>
</xsl:template>

</xsl:stylesheet>
