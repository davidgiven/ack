<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="1.0"
	xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
	
	<xsl:output method="html"/>
	<xsl:param name="SRC"/>
	<xsl:param name="THIS"/>
	
	<xsl:variable name="mytitle" select="/html/head/title"/>
	<xsl:variable name="contentsxml" select="document(concat($SRC, '/contents.xml'))"/>
	
	<xsl:key name="title" match="page"
		use="document(concat($SRC, '/', @src))/html/head/title"/>
	
	<xsl:template match="page" mode="site-contents">
		<xsl:variable name="child" select="document(concat($SRC, '/', @src))"/>
		<xsl:variable name="childtitle">
			<xsl:value-of select="$child/html/head/title"/>
		</xsl:variable>
		
		<xsl:element name="a">
			<xsl:attribute name="href">
				<xsl:value-of select="$DESTURL"/>
				<xsl:text>/</xsl:text>
				<xsl:choose>
					<xsl:when test="substring(@src, string-length(@src)-1) = '.i'">
						<xsl:value-of select="substring(@src, 1, string-length(@src)-2)"/>
						<xsl:text>.html</xsl:text>
					</xsl:when>
					<xsl:otherwise>
						<xsl:value-of select="@src"/>
					</xsl:otherwise>
				</xsl:choose>
			</xsl:attribute>

			<xsl:value-of select="$childtitle"/>
		</xsl:element>
	</xsl:template>

	<xsl:template match="page" mode="site-contents-li">
		<li>
			<xsl:if test="@src = $THIS">
				<xsl:attribute name="class">
					<xsl:text>selected</xsl:text>
				</xsl:attribute>
			</xsl:if>
			
			<xsl:apply-templates select="." mode="site-contents"/>
		</li>
	</xsl:template>
	
	<xsl:template name="sibling-page-list">
		<xsl:variable name="currentnode" select="$contentsxml//page[@src = $THIS]"/>

		<h4>Page Group</h4>
		<ul>
			<xsl:for-each select="$currentnode/../page">
				<xsl:apply-templates select="." mode="site-contents-li"/>
			</xsl:for-each>
		</ul>
	</xsl:template>

	<xsl:template name="child-page-list">
		<xsl:variable name="currentnode" select="$contentsxml//page[@src = $THIS]"/>
		<xsl:variable name="hierarchy" select="$currentnode/ancestor::*"/>
		
		<xsl:if test="$currentnode/page">
			<h4>Child Pages</h4>
			<ul>
				<xsl:for-each select="$currentnode/page">
					<xsl:apply-templates select="." mode="site-contents-li"/>
				</xsl:for-each>
			</ul>
		</xsl:if>
	</xsl:template>

	<xsl:template name="site-contents">
		<xsl:variable name="currentnode" select="$contentsxml//page[@src = $THIS]"/>
		<xsl:variable name="hierarchy" select="$currentnode/ancestor::*"/>
		
		<div class="toclist">
			<!-- Parents -->
			<xsl:for-each select="$hierarchy">
				<xsl:variable name="child" select="document(concat($SRC, '/', @src))"/>
				<xsl:variable name="childtitle">
					<xsl:value-of select="$child/html/head/title"/>
				</xsl:variable>
				<div class="parent">
					<xsl:apply-templates select="." mode="site-contents"/>
				</div>
			</xsl:for-each>
			
			<xsl:choose>
				<xsl:when test="$currentnode/page">
					<!-- The current page has children. Show the page itself. -->
					<div class="parent">
						<xsl:apply-templates select="$currentnode" mode="site-contents"/>
					</div>
					
					<!-- Children, if any -->
					<xsl:for-each select="$currentnode/page">
						<div class="child">
							<xsl:apply-templates select="." mode="site-contents"/>
						</div>
					</xsl:for-each>
				</xsl:when>
				<xsl:otherwise>
					<!-- The current page has no children, so show siblings instead. -->
					<xsl:for-each select="$currentnode/../page">
						<div class="child">
							<xsl:apply-templates select="." mode="site-contents"/>
						</div>
					</xsl:for-each>
				</xsl:otherwise>
			</xsl:choose>
		</div>
	</xsl:template>

	<xsl:template name="path-to-page">
		<xsl:variable name="currentnode" select="$contentsxml//page[@src = $THIS]"/>
		
		<xsl:for-each select="$currentnode/ancestor-or-self::*">
			<xsl:if test="position() > 1">
				<xsl:text> / </xsl:text>
			</xsl:if>
			<xsl:apply-templates select="." mode="site-contents"/>
		</xsl:for-each>
	</xsl:template>

	<xsl:template name="navigation-block">
		<xsl:variable name="currentnode" select="$contentsxml//page[@src = $THIS]"/>
		
		<xsl:variable name="prevnode" select="$currentnode/preceding-sibling::*[1]"/>
		<xsl:variable name="upnode" select="$currentnode/parent::*"/>
		<xsl:variable name="nextnode" select="$currentnode/following-sibling::*[1]"/>
		
		<xsl:if test="$prevnode | $nextnode">
			<div class="navigation-block">
				<table>
					<tr><td class="navigation-left">
						<xsl:if test="$prevnode">
							<b>«</b>
							<xsl:apply-templates select="$prevnode" mode="site-contents"/>
						</xsl:if>
					</td><td class="navigation-right">
						<xsl:if test="$nextnode">
							<xsl:apply-templates select="$nextnode" mode="site-contents"/>
							<b>»</b>
						</xsl:if>
					</td></tr>
				</table>
			</div>
		</xsl:if>
	</xsl:template>
	
	<xsl:template match="a" mode="smartlink">
		<xsl:variable name="pagename" select="@href"/>
		<xsl:variable name="currentnode" select="$contentsxml//page[@src = $THIS]"/>
	
		<xsl:variable name="targets" select="$contentsxml//page[contains(@src, $pagename)]"/>
		<xsl:if test="count($targets) = 0">
			<xsl:message terminate="yes">
				<xsl:value-of select="$pagename"/>
				<xsl:text> could not be resolved</xsl:text>
			</xsl:message>
		</xsl:if>
		<xsl:if test="count($targets) > 1">
			<xsl:message terminate="yes">
				<xsl:value-of select="$pagename"/>
				<xsl:text> is ambiguous among (</xsl:text>
				<xsl:for-each select="$targets">
					<xsl:value-of select="@src"/>
					<xsl:text> </xsl:text>
				</xsl:for-each>
				<xsl:text>)</xsl:text>
			</xsl:message>
		</xsl:if>

		<xsl:apply-templates select="$targets[1]" mode="site-contents"/>
	</xsl:template>
	
	<xsl:template match="*" mode="site-contents">
		<xsl:message terminate="yes">
			<xsl:text>Unsupported tag </xsl:text>
			<xsl:value-of select="name(.)"/>
		</xsl:message>
	</xsl:template>
</xsl:stylesheet>
