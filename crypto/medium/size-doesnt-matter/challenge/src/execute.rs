use core::str;
use std::process::Command;

use base64::prelude::{Engine as _, BASE64_STANDARD as base64};
use openssl::{encrypt::Decrypter, rsa::Padding};
use rocket::{http::Status, post, State};

use crate::{memory_format::uncompress, EncryptionKeys, ServerToken, VerifiedRequest};

#[post("/execute", data = "<command_token>")]
pub async fn execute_command(
    token: &State<ServerToken>,
    keys: &State<EncryptionKeys>,
    command_token: String,
) -> (Status, String) {
    // base64 decode
    let Ok(encrypted) = base64.decode(&command_token) else {
        return (
            Status::BadRequest,
            "Command token not valid base64".to_owned(),
        );
    };

    // decrypt
    let mut decrypter = Decrypter::new(&keys.private).unwrap();
    decrypter.set_rsa_padding(Padding::PKCS1).unwrap();
    let buffer_len = match decrypter.decrypt_len(&encrypted) {
        Ok(len) => len,
        Err(error) => return (Status::BadRequest, format!("{:#?}", error.errors())),
    };
    let mut compressed = vec![0; buffer_len];
    let decrypted_len = match decrypter.decrypt(&encrypted, &mut compressed) {
        Ok(len) => len,
        Err(error) => return (Status::BadRequest, format!("{:#?}", error.errors())),
    };
    compressed.truncate(decrypted_len);

    // uncompress
    let Ok(json) = uncompress(&compressed) else {
        return (Status::BadRequest, "DecompressionError".to_owned());
    };

    // json decode
    let request = match serde_json::from_slice::<VerifiedRequest>(&json) {
        Ok(request) => request,
        Err(err) => return (Status::BadRequest, err.to_string()),
    };

    if request.server_token != token.token {
        return (Status::BadRequest, "Invalid validation token".to_owned());
    };

    execute(&request).await
}

async fn execute(request: &VerifiedRequest) -> (Status, String) {
    match request.command.as_str() {
        "ls" => ls(request.param.as_deref()).await,
        "cat" => cat(request.param.as_deref()).await,
        "help" => help(),
        _ => (
            Status::InternalServerError,
            format!("Unknown command: {}", &request.command),
        ),
    }
}

async fn ls(param: Option<&str>) -> (Status, String) {
    let mut command = Command::new("/bin/ls");
    if let Some(param) = param {
        command.arg(param.to_owned());
    }
    run_command(command).await
}

async fn cat(param: Option<&str>) -> (Status, String) {
    let mut command = Command::new("/bin/cat");
    if let Some(param) = param {
        command.arg(param.to_owned());
    }
    run_command(command).await
}

async fn run_command(mut command: Command) -> (Status, String) {
    let result = rocket::tokio::task::spawn_blocking(move || command.output())
        .await
        .expect("Task will not panic!")
        .expect("Programs can be loaded!");

    if !result.status.success() {
        return (
            Status::InternalServerError,
            format!(
                "Error while running command:\nstdout:\n{}\n\nstderr:\n{}\n",
                String::from_utf8_lossy(&result.stdout),
                String::from_utf8_lossy(&result.stderr)
            ),
        );
    }

    match String::from_utf8(result.stdout) {
        Ok(string) => (Status::Ok, string),
        Err(_) => (
            Status::InternalServerError,
            "Result is not valid utf8".to_owned(),
        ),
    }
}

fn help() -> (Status, String) {
    return (
        Status::Ok,
        r"Help, Available commands:
    help - print this message
    ls - list file
    cat - print file
"
        .to_owned(),
    );
}
