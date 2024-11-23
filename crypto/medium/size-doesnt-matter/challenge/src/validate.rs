use base64::prelude::{Engine as _, BASE64_STANDARD as base64};
use openssl::{encrypt::Encrypter, rsa::Padding};
use rocket::{http::Status, post, State};

use crate::{memory_format::compress, EncryptionKeys, ServerToken, VerifiedRequest};

#[post("/validate", data = "<command>")]
pub fn validate_command(
    token: &State<ServerToken>,
    keys: &State<EncryptionKeys>,
    command: String,
) -> (Status, String) {
    // validate
    let response = match validate(&command, &token.inner().token) {
        Ok(response) => response,
        Err(err) => return err,
    };

    // Create response json
    let json = serde_json::to_string(&response).unwrap();

    // compress the response
    let compressed = compress(json.as_bytes());

    // encrypt
    let mut encrypter = Encrypter::new(&keys.public).unwrap();
    encrypter.set_rsa_padding(Padding::PKCS1).unwrap();
    let buffer_len = encrypter.encrypt_len(&compressed).unwrap();
    let mut encrypted = vec![0; buffer_len];
    let Ok(encrypted_len) = encrypter.encrypt(&compressed, &mut encrypted) else {
        return (Status::BadRequest, "Command to long!".to_owned());
    };
    encrypted.truncate(encrypted_len);

    // base64
    (Status::Ok, base64.encode(&encrypted))
}

fn validate(command: &str, token: &str) -> Result<VerifiedRequest, (Status, String)> {
    let (command, param) = match command.split_once(" ") {
        Some((command, params)) => (command, Some(params)),
        None => (command, None),
    };

    let valid_commands = ["ls", "help", "cat"];
    if !valid_commands.contains(&command) {
        return Err((Status::BadRequest, format!("Unknown command: {}", command)));
    }

    let param = param.map(|p| p.trim().replace('/', "\\/"));

    if command == "cat" {
        if param.is_none() {
            return Err((
                Status::Forbidden,
                "The cat command always takes an argument!".to_owned(),
            ));
        }
        if param.as_deref() == Some("flag.txt") {
            return Err((Status::Forbidden, "Cannot open that file!!!!".to_owned()));
        }
    }

    if command == "help" && param.is_some() {
        return Err((
            Status::Forbidden,
            "Help does not take arguments.".to_owned(),
        ));
    }

    Ok(VerifiedRequest {
        server_token: token.to_owned(),
        command: command.to_owned(),
        param: param,
    })
}
