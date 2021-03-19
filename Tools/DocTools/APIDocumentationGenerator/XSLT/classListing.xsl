<?xml version="1.0" encoding="utf-8"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:template name='classList' match='TOC'>
  <xsl:param name='classDoc' select='.' />
  <div class="subhead" data-target="classes"><xsl:value-of select="/Root/website_terms/classes" /></div>
  <div class="clear">
  <div class="subcontent hide classes">
    <ul class='left-menu'>
      <xsl:apply-templates select='$classDoc/TOC/classes/class' />    
    </ul>
  </div>
  </div>
  <div class="subhead" data-target="enumerations"><xsl:value-of select="/Root/website_terms/enumerations" /></div>
  <div class="clear">
  <div class="subcontent hide enumerations">
    <ul class='left-menu'>
      <xsl:apply-templates select='$classDoc/TOC/enums/enum' />
    </ul>
  </div>
  </div>
  <div class="subhead" data-target="attributes"><xsl:value-of select="/Root/website_terms/attributes" /></div>
  <div class="clear">
  <div class="subcontent hide attributes">
    <ul class='left-menu'>
      <xsl:apply-templates select='$classDoc/TOC/attributes/attribute' />
    </ul>
  </div>
  </div>
</xsl:template>

<xsl:template match='class'>
  <li class='class'><xsl:if test='class'><div class='arrow'><xsl:value-of select="' '"/></div></xsl:if><a href='{@name}.html'><xsl:value-of select='@name' /></a>
  <xsl:if test='class'>
    <ul class='left-submenu'>
      <xsl:apply-templates />
    </ul>
  </xsl:if>
  </li>
</xsl:template>

<xsl:template match='enum'>
  <li class='enum'><a href='{@name}.html'><xsl:value-of select='@name' /></a></li>
</xsl:template>

<xsl:template match='attribute'>
  <li class='attr'><a href='{@name}.html'><xsl:value-of select='@name' /></a></li>
</xsl:template>

</xsl:stylesheet>
