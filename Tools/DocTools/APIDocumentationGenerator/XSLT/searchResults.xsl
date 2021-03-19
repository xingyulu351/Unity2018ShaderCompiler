<?xml version="1.0" encoding="utf-8"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
<xsl:import href="styling.xsl"/>
  
<xsl:template match="/"><xsl:apply-templates select="summary"/></xsl:template>
<xsl:template match="summary"><xsl:apply-templates/></xsl:template>
  
</xsl:stylesheet>
