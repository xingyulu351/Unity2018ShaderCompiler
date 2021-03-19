<?xml version="1.0" encoding="utf-8"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

  <xsl:template match='link'><a href='{@ref}.html'><xsl:value-of select='.' /></a></xsl:template>
  <xsl:template match='a'><a href="{@href}"><xsl:apply-templates /></a></xsl:template>
  <xsl:template match='p'><p class='basic'><xsl:apply-templates /></p></xsl:template>
  <xsl:template match='i'><em><xsl:apply-templates /></em></xsl:template>
  <xsl:template match='b'><strong><xsl:apply-templates /></strong></xsl:template>
  <xsl:template match='bi'><strong><em><xsl:apply-templates /></em></strong></xsl:template>
  <xsl:template match='tt'><code class='tt'><xsl:apply-templates /></code></xsl:template>
  <xsl:template match='varname'><code class='varname'><xsl:apply-templates /></code></xsl:template>
  <xsl:template match='monotype'><code class='mono'><xsl:apply-templates /></code></xsl:template>
  <xsl:template match='br'><br /></xsl:template>

</xsl:stylesheet>
