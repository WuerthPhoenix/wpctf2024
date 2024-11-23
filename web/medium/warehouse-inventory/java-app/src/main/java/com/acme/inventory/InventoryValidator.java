package com.acme.inventory;

import org.w3c.dom.Document;
import org.w3c.dom.Element;
import org.w3c.dom.NodeList;
import org.xml.sax.SAXException;

import javax.xml.parsers.DocumentBuilder;
import javax.xml.parsers.DocumentBuilderFactory;
import javax.xml.parsers.ParserConfigurationException;
import javax.xml.transform.OutputKeys;
import javax.xml.transform.Transformer;
import javax.xml.transform.TransformerException;
import javax.xml.transform.TransformerFactory;
import javax.xml.transform.dom.DOMSource;
import javax.xml.transform.stream.StreamResult;
import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.time.LocalDateTime;
import java.time.format.DateTimeFormatter;

public class InventoryValidator {
    public static byte[] validate(byte[] xmlBytes) {
        byte[] resultBytes;

        try {
            String currentDateTime = LocalDateTime.now().format(DateTimeFormatter.ISO_DATE_TIME);

            Document doc = bytes2document(xmlBytes);
            NodeList itemsList = doc.getElementsByTagName("item");

            for (int i = 0; i < itemsList.getLength(); i++) {
                Element itemElement = (Element) itemsList.item(i);
                NodeList quantityNodes = itemElement.getElementsByTagName("quantity");

                if (quantityNodes.getLength() > 0) {
                    Element quantityElement = (Element) quantityNodes.item(0);
                    int quantityValue = Integer.parseInt(quantityElement.getTextContent());

                    if (quantityValue < 0) {
                        throw new RuntimeException(itemElement.getTagName() + " has negative quantity");
                    }

                    // Add or edit timestamp
                    Element timestampElement;
                    NodeList timestampNodes = itemElement.getElementsByTagName("timestamp");
                    if (timestampNodes.getLength() > 0) {
                        timestampElement = (Element) timestampNodes.item(0);
                        timestampElement.setTextContent(currentDateTime);
                    } else {
                        timestampElement = doc.createElement("timestamp");
                        timestampElement.appendChild(doc.createTextNode(currentDateTime));
                        itemElement.appendChild(timestampElement);
                    }
                }
            }

            resultBytes = document2bytes(doc);
        } catch (Exception e) {
            return null;
        }

        return resultBytes;
    }

    private static Document bytes2document(byte[] xmlBytes) throws ParserConfigurationException, SAXException, IOException {
        DocumentBuilderFactory dbFactory = DocumentBuilderFactory.newInstance();
        DocumentBuilder dBuilder = dbFactory.newDocumentBuilder();
        ByteArrayInputStream input = new ByteArrayInputStream(xmlBytes);
        Document doc = dBuilder.parse(input);
        doc.getDocumentElement().normalize();
        return doc;
    }

    private static byte[] document2bytes(Document doc) throws TransformerException {
        TransformerFactory transformerFactory = TransformerFactory.newInstance();
        Transformer transformer = transformerFactory.newTransformer();
        transformer.setOutputProperty(OutputKeys.INDENT, "yes");
        DOMSource source = new DOMSource(doc);
        ByteArrayOutputStream outputStream = new ByteArrayOutputStream();
        StreamResult result = new StreamResult(outputStream);
        transformer.transform(source, result);
        return outputStream.toByteArray();
    }
}