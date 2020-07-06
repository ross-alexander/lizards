<xsl:stylesheet version="1.0"
      xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
      xmlns:fo="http://www.w3.org/1999/XSL/Format">

<xsl:template match="swamp">
<xsl:apply-templates select="players"/>
</xsl:template>

<xsl:template match="player">
<xsl:value-of select="@clan"/>
<xsl:variable name="indx"><xsl:value-of select="@indx"/></xsl:variable>
<xsl:apply-templates select="/swamp/map/hex[@owner = $indx]"/>
</xsl:template>

<xsl:template match="hex">
Hex <xsl:value-of select="./@title"/>
</xsl:template>
</xsl:stylesheet>
