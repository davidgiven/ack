<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE stylesheet [
<!ENTITY nbsp "&#160;" >
]>
<xsl:stylesheet version="1.0"
	xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
	
	<xsl:import href="_htmlx.xslt"/>
	<xsl:import href="_contents.xslt"/>
	<xsl:output method="xml"
		doctype-system="http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd"
		doctype-public="-//W3C//DTD XHTML 1.0 Strict//EN"
		indent="no"
		encoding="UTF-8"/>
	
	<xsl:param name="SRC"/>
	<xsl:param name="DESTURL"/>
	<xsl:variable name="DOCUMENT" select="."/>
	
	<xsl:key name="parentheader"
		match="html/body/*[starts-with(local-name(),'h')]"
		use="generate-id(preceding-sibling::*[local-name()=concat('h', number(substring-after(local-name(current()),'h')) - 1)][1])"/>

	<xsl:template match="/">
		<html>
			<head>
				<title><xsl:value-of select="html/head/title"/></title>
				<link rel="stylesheet" type="text/css" href="{$DESTURL}/global.css"/>
				<link rel="home" type="text/html" href="/"/>
			</head>
			
			<body>
				<script type="text/javascript" src="{$DESTURL}/global.js"></script>
				<!-- Left-hand column -->
				<div class="nav nav-logo">
					<img src="{$DESTURL}/logo.png" alt="Prime Mover"/>
				</div>
				
				<div class="nav nav-related">
					<xsl:call-template name="sibling-page-list"/>
				</div>
				
				<div class="nav nav-children">
					<xsl:call-template name="child-page-list"/>
				</div>
				
				<div class="nav nav-icons">
					<a href="http://validator.w3.org/check?uri=referer">
						<img src="http://www.w3.org/Icons/valid-xhtml10"
							alt="Valid XHTML 1.0 Strict" height="31" width="88"/>
					</a>
					<br/>
					<a href="http://jigsaw.w3.org/css-validator/">
						<img src="http://jigsaw.w3.org/css-validator/images/vcss"
							alt="Valid CSS" height="31" width="88"/>
					</a>
					<br/>
					<a href="http://sourceforge.net/projects/tack">
						<img src="http://sflogo.sourceforge.net/sflogo.php?group_id=157791&amp;type=1"
							width="88" height="31" alt="SourceForge.net Logo"/>
					</a>
				</div>
				
				<!-- Right-hand column -->
				
				<div class="page-path">
					<p>
						<xsl:call-template name="path-to-page"/>
					</p>
				</div>
				
				<div class="page-body">
					<xsl:call-template name="navigation-block"/>
					
					<h1 class="title"><xsl:value-of select="html/head/title"/></h1>
					
					<xsl:if test="html/body/h1">
						<div id="toc" class="body-contents">
							<h4 id="toctitle">Page Contents</h4>
							<ol class="toclist" style="display:none">
								<xsl:apply-templates select="html/body/h1" mode="toc"/>
							</ol>
						</div>
					</xsl:if>

					<script type="text/javascript" src="{$DESTURL}/contents.js"></script>
					
					<xsl:apply-templates select="html/body/*"/>

					<xsl:call-template name="navigation-block"/>
				</div>
			</body>
		</html>
	</xsl:template>

	<xsl:template match="html/body/h1 | html/body/h2 | html/body/h3" mode="toc">
		<li>
			<xsl:element name="a">
				<xsl:attribute name="href">
					<xsl:text>#</xsl:text>
					<xsl:value-of select="generate-id(.)"/>
				</xsl:attribute>
				<xsl:value-of select="."/>
			</xsl:element>

			<xsl:if test="key('parentheader',generate-id())">
				<ol class="toclist">
					<xsl:apply-templates select="key('parentheader',generate-id())" mode="toc"/>
				</ol>
			</xsl:if>
		</li>
	</xsl:template>

	<xsl:template match="html/body/h4" mode="toc"/>
</xsl:stylesheet>
