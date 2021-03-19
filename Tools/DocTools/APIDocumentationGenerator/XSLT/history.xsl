<?xml version="1.0" encoding="utf-8"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
	<xsl:import href="staticRoot.xsl"/>

	<xsl:key name="version" match="/items/item/@versionAdded" use="." />

	<xsl:template match='items'>
		<h1><xsl:value-of select="/Root/website_terms/api_histroy" /></h1>
		<p><xsl:value-of select="/Root/website_terms/api_histroy_body" /></p>

		<!--make a list of available versions  -->
		<xsl:for-each select="/items/item/@versionAdded[generate-id() = generate-id(key('version',.)[1])]">
			<xsl:sort order='descending'/>
			<a>
				<xsl:attribute name="href">#<xsl:value-of select="translate(., '.','-')"/></xsl:attribute>
				<xsl:value-of select="."/>
			</a>&#160;
		</xsl:for-each>
		<div class="clear"><xsl:value-of select="' '"/></div>

		<!-- table of all the added / removed elements -->
		<xsl:for-each select="/items/item/@versionAdded[generate-id() = generate-id(key('version',.)[1])]">
			<xsl:sort order='descending'/>

			<!-- Display version number -->
			<xsl:variable name="version" select="." />
			<p><a>
				<xsl:attribute name="name">
					<xsl:value-of select="translate($version, '.','-')" />
				</xsl:attribute>
				</a><br/></p>
			<br/> <br/>
			<h3>Changes in <xsl:value-of select="$version" /></h3>

			<table class="history-table">
				<thead>
					<tr>
						<th class="link"><xsl:value-of select="/Root/website_terms/api_histroy_name" /></th>
						<th class="type"><xsl:value-of select="/Root/website_terms/api_histroy_type" /></th>
						<th class="namespace"><xsl:value-of select="/Root/website_terms/api_histroy_namespace" /></th>
						<th><xsl:value-of select="/Root/website_terms/api_histroy_status" /></th>
						<th><xsl:value-of select="/Root/website_terms/api_histroy_version" /></th>
					</tr>
				</thead>
				<tbody>
					<xsl:for-each select="/items/item[@versionAdded = $version or @versionRemoved = $version or @versionObsolete = $version]">
						<xsl:variable name="status">
							<xsl:if test='@versionAdded=$version'><xsl:value-of select="/Root/website_terms/api_histroy_added" /></xsl:if>
							<xsl:if test='@versionObsolete=$version'><xsl:value-of select="/Root/website_terms/api_histroy_obsolete" /></xsl:if>
							<xsl:if test='@versionRemoved=$version'><xsl:value-of select="/Root/website_terms/api_histroy_removed" /></xsl:if>
						</xsl:variable>
						<xsl:apply-templates select='.' mode='list'>
							<xsl:with-param name='version' select='$version'/>
							<xsl:sort select='$status' order='ascending'/>
							<xsl:sort select='@namespace' order='ascending'/>
							<xsl:sort select='@memberID' order='ascending'/>
							<xsl:sort select='@type' order='ascending'/>
						</xsl:apply-templates>
					</xsl:for-each>
				</tbody>
			</table>
		</xsl:for-each>
	</xsl:template>

	<xsl:template match='item' mode='filter'>
		<xsl:if test='@alive'>
			<li><xsl:value-of select='@version'/></li>
		</xsl:if>
	</xsl:template>

	<xsl:template match='item' mode='list'>
    <xsl:param name='version'/>
		<xsl:if test='@alive'>
			<tr>
				<xsl:choose>
					<xsl:when test="api/link">
						<td><a>
								<xsl:attribute name="href"><xsl:value-of select='api/link/@ref'/>.html</xsl:attribute>
								<xsl:value-of select='api/link'/>
							</a></td>
					</xsl:when>
					<xsl:otherwise>
						<td><xsl:if test="@namespace != ''"><xsl:value-of select="@namespace"/>.</xsl:if><xsl:value-of select='api'/></td>
					</xsl:otherwise>
				</xsl:choose>
				<xsl:variable name="status">
					<xsl:if test='@versionAdded=$version'><xsl:value-of select="/Root/website_terms/api_histroy_added" /></xsl:if>
					<xsl:if test='@versionObsolete=$version'><xsl:value-of select="/Root/website_terms/api_histroy_obsolete" /></xsl:if>
					<xsl:if test='@versionRemoved=$version'><xsl:value-of select="/Root/website_terms/api_histroy_removed" /></xsl:if>
				</xsl:variable>
				<td><xsl:value-of select='@type'/></td>
				<td><xsl:value-of select='@namespace'/></td>
				<td><xsl:value-of select='$status'/></td>
				<td><xsl:value-of select='$version'/></td>
			</tr>
		</xsl:if>
	</xsl:template>

</xsl:stylesheet>
