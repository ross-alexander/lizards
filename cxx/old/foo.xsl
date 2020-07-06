<?xml version="1.0"?> 
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0">

  <xsl:template match="/">
    <xsl:apply-templates/>
  </xsl:template>

  <xsl:template match="hex">
    <xsl:value-of select="."/>
    <xsl:value-of select="@title"/>

    <xsl:for-each select="./*">
      <xsl:value-of select="name(.)"/>
    </xsl:for-each>

  </xsl:template>
    
</xsl:stylesheet>
