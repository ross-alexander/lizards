<xsl:stylesheet version="1.0"
      xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
      xmlns:fo="http://www.w3.org/1999/XSL/Format">

<xsl:template match="swamp">
<xsl:apply-templates select="players"/>
</xsl:template>

<xsl:template match="player">
<xsl:text>Report of clan </xsl:text><xsl:value-of select="@clan"/><xsl:text/>
<xsl:variable name="indx"><xsl:value-of select="@indx"/></xsl:variable>
<xsl:apply-templates select="/swamp/map/hex[@owner = $indx]"/>
</xsl:template>

<xsl:template match="den">
<xsl:text>Den</xsl:text>
</xsl:template>

<xsl:template match="hex">
<xsl:text>Hex </xsl:text><xsl:value-of select="./@title"/>
<xsl:apply-templates select="den"/>
<xsl:text>
</xsl:text>
</xsl:template>
</xsl:stylesheet>
