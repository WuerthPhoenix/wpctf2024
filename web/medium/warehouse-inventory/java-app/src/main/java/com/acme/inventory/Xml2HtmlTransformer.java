package com.acme.inventory;

import javax.xml.transform.*;
import javax.xml.transform.stream.StreamResult;
import javax.xml.transform.stream.StreamSource;
import java.io.File;
import java.io.StringWriter;

public class Xml2HtmlTransformer {
    public String transform(String xmlPath, String templatePath) throws Exception {
        File xsltFile = new File(templatePath);
        File xmlFile = new File(xmlPath);

        // Set the source for the XML document and the XSLT style sheet
        Source xmlSource = new StreamSource(xmlFile);
        Source xsltSource = new StreamSource(xsltFile);

        // Use StringWriter to get the string output
        StringWriter writer = new StringWriter();
        Result result = new StreamResult(writer);

        // Instantiate TransformerFactory and Transformer to transform
        TransformerFactory factory = TransformerFactory.newInstance();
        Transformer transformer = factory.newTransformer(xsltSource);

        // Apply the transformation
        transformer.transform(xmlSource, result);

        // Return the transformed XML as a string
        return writer.toString();
    }
}