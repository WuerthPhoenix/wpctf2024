package com.acme.inventory;

import java.io.File;
import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.Paths;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import jakarta.servlet.http.HttpServletRequest;
import org.springframework.boot.SpringApplication;
import org.springframework.boot.autoconfigure.SpringBootApplication;

import org.springframework.http.MediaType;
import org.springframework.web.bind.annotation.GetMapping;
import org.springframework.web.bind.annotation.PostMapping;
import org.springframework.web.bind.annotation.RequestParam;
import org.springframework.web.bind.annotation.RestController;
import org.springframework.core.io.FileSystemResource;
import org.springframework.http.HttpHeaders;
import org.springframework.http.HttpStatus;
import org.springframework.http.ResponseEntity;
import org.springframework.web.multipart.MultipartFile;

@SpringBootApplication
@RestController
public class InventoryApplication {

    public static final String INVENTORY_FILE = "/app/upload/inventory.xml";
    Pattern pattern = Pattern.compile("<!(?:DOCTYPE|ENTITY)(?:\s|%|&#[0-9]+;|&#x[0-9a-fA-F]+;)+[^\s]+\s+(?:SYSTEM|PUBLIC)\s+[\'\"]", Pattern.CASE_INSENSITIVE | Pattern.DOTALL);

    public static void main(String[] args) {
        SpringApplication.run(InventoryApplication.class, args);
    }

    @GetMapping("/admin")
    public ResponseEntity<String> getTemplate(@RequestParam(value = "template-name", defaultValue = "inventory_list.tpl") String templateName, HttpServletRequest request) {
        try {
            if(!request.getRemoteAddr().equals("127.0.0.1"))
                return ResponseEntity.status(HttpStatus.NOT_ACCEPTABLE).build();

            String templatePath = "/app/templates/" + templateName;
            Xml2HtmlTransformer transformer = new Xml2HtmlTransformer();
            String htmlContent = transformer.transform(INVENTORY_FILE, templatePath);
            return ResponseEntity.ok(htmlContent);
        } catch (Exception e) {
            e.printStackTrace();
            return ResponseEntity.status(HttpStatus.INTERNAL_SERVER_ERROR).body("Error processing template");
        }
    }

    @PostMapping(value = "/upload", consumes = MediaType.MULTIPART_FORM_DATA_VALUE)
    public ResponseEntity<Void> processXml(@RequestParam("attachment") MultipartFile attachment) {
        try {
            byte[] attachmentFile = attachment.getBytes();

            Matcher matcher = pattern.matcher(new String(attachmentFile));
            if(matcher.find())
                return ResponseEntity.status(HttpStatus.NOT_ACCEPTABLE).build();
            
            byte[] validatedXmlBytes = InventoryValidator.validate(attachmentFile);
            
            if (validatedXmlBytes == null) {
                return ResponseEntity.status(HttpStatus.NOT_ACCEPTABLE).build();
            }
            writeInventoryFile(validatedXmlBytes);
        } catch (IOException e) {
            throw new RuntimeException(e);
        }

        return ResponseEntity.ok().build();
    }

    @GetMapping("/download")
    public ResponseEntity<FileSystemResource> downloadInventory() {
        File file = new File(INVENTORY_FILE);
        if (!file.exists()) {
            System.out.println("File not found");
            return ResponseEntity.status(HttpStatus.NOT_FOUND).body(null);
        }

        // test
        FileSystemResource resource = new FileSystemResource(file);
        HttpHeaders headers = new HttpHeaders();
        headers.add(HttpHeaders.CONTENT_DISPOSITION, "attachment; filename=inventory.xml");
        headers.add(HttpHeaders.CONTENT_TYPE, "application/xml");

        return ResponseEntity.ok()
                .headers(headers)
                .body(resource);
    }

    private static void writeInventoryFile(byte[] xmlBytes) {
        try {
            File file = new File(INVENTORY_FILE);
            if (!file.exists()) {
                Files.createDirectories(Paths.get(file.getParent()));
                Files.createFile(file.toPath());
            }
            Files.write(file.toPath(), xmlBytes);
        } catch (IOException e) {
            throw new RuntimeException("Failed to create inventory file", e);
        }
    }
}
