<?xml version='1.0'?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
  <xsl:import href="staticRoot.xsl"/>
  <xsl:import href='bodyContent.xsl' />
  
  <xsl:template match='image'>
    <img src='../StaticFiles/ScriptRefImages/{.}' />
  </xsl:template>
  
</xsl:stylesheet>
