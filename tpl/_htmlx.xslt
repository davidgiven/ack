<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="1.0"
	xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
	
	<xsl:output method="html"/>
	

	<xsl:template match="p | i | b | u | tt | hr | ol | ul | li | blockquote | h4 | table | tbody | tr | td | th">
		<xsl:copy>
			<xsl:copy-of select="@*"/>
			<xsl:apply-templates/>
		</xsl:copy>
	</xsl:template>
	
	<xsl:template match="code">
		<tt>
			<xsl:copy-of select="@*"/>
			<xsl:apply-templates/>
		</tt>
	</xsl:template>
	
	<xsl:template match="var | em">
		<i>
			<xsl:copy-of select="@*"/>
			<xsl:apply-templates/>
		</i>
	</xsl:template>
	
	<xsl:template match="pre">
		<xsl:copy>
			<xsl:copy-of select="@*"/>
			<xsl:value-of select="."/>
		</xsl:copy>
	</xsl:template>
	
	<xsl:template match="hr">
		<hr/>
	</xsl:template>
	
	
	<xsl:template match="a[@href = text()]">
		<xsl:apply-templates select="." mode="smartlink"/>
	</xsl:template>

	<xsl:template match="a">
		<xsl:copy>
			<xsl:copy-of select="@*"/>
			<xsl:apply-templates/>
		</xsl:copy>
	</xsl:template>
	

	<xsl:template match="dl">
		<table columns="2" class="dl">
			<xsl:for-each select="dt">
				<tr>
					<td>
						<xsl:apply-templates select="node()"/>
					</td><td>
						<xsl:apply-templates select="following-sibling::dd[1]/node()"/>
					</td>
				</tr>
			</xsl:for-each>
		</table>
	</xsl:template>
	
	
	<xsl:template match="img[substring(@src, string-length(@src)-3) = '.svg']">
		<xsl:element name="object">
			<xsl:attribute name="data">
				<xsl:value-of select="@src"/>
			</xsl:attribute>
			<xsl:attribute name="type" value="image/svg+xml"/>
			<xsl:text>[An SVG file goes here]</xsl:text>
		</xsl:element>
	</xsl:template>
	
	<xsl:template match="img">
		<xsl:copy>
			<xsl:copy-of select="@*"/>
			<xsl:apply-templates/>
		</xsl:copy>
	</xsl:template>		

	<xsl:template match="h1|h2|h3">
		<h1>
			<xsl:element name="a">
				<xsl:attribute name="name">
					<xsl:value-of select="generate-id(.)"/>
				</xsl:attribute>
				<xsl:apply-templates select="." mode="header-label"/>
			</xsl:element>
		</h1>
	</xsl:template>
	
	<xsl:template match="h1" mode="header-label">
		<xsl:number level="any" count="h1"/>
		<xsl:text>. </xsl:text>
		<xsl:apply-templates/>
	</xsl:template>
	
	<xsl:template match="h2" mode="header-label">
		<xsl:number level="any" count="h1"/>
		<xsl:text>.</xsl:text>
		<xsl:number level="any" from="h1" count="h2"/>
		<xsl:text>. </xsl:text>
		<xsl:apply-templates/>
	</xsl:template>
		
	<xsl:template match="h3" mode="header-label">
		<xsl:number level="any" count="h1"/>
		<xsl:text>.</xsl:text>
		<xsl:number level="any" from="h1" count="h2"/>
		<xsl:text>.</xsl:text>
		<xsl:number level="any" from="h2" count="h3"/>
		<xsl:text>. </xsl:text>
		<xsl:apply-templates/>
	</xsl:template>
	
	
	<xsl:template match="h1" mode="toc-indent">
	</xsl:template>
	
	<xsl:template match="h2" mode="toc-indent">
		<xsl:text>...</xsl:text>
	</xsl:template>
		
	<xsl:template match="h3" mode="toc-indent">
		<xsl:text>......</xsl:text>
	</xsl:template>
	
	
	<xsl:template match="h1" mode="toc-label">
		<xsl:number level="any" count="h1"/>
		<xsl:text>. </xsl:text>
		<xsl:apply-templates/>
	</xsl:template>
	
	<xsl:template match="h2" mode="toc-label">
		<xsl:number level="any" from="h1" count="h2"/>
		<xsl:text>. </xsl:text>
		<xsl:apply-templates/>
	</xsl:template>
		
	<xsl:template match="h3" mode="toc-label">
		<xsl:number level="any" from="h2" count="h3"/>
		<xsl:text>. </xsl:text>
		<xsl:apply-templates/>
	</xsl:template>
	
	<xsl:template match="h4" mode="toc-label">
	</xsl:template>
	
	
	<xsl:template match="*">
		<xsl:message terminate="yes">
			<xsl:text>Unsupported tag </xsl:text>
			<xsl:value-of select="name(.)"/>
		</xsl:message>
	</xsl:template>
</xsl:stylesheet>
