use execute::execute_command;
use openssl::{
    pkey::{PKey, Private, Public},
    x509::X509,
};
use rocket::{
    config::TlsConfig,
    launch, routes,
    serde::{Deserialize, Serialize},
};
use validate::validate_command;

mod execute;
mod memory_format;
mod validate;

struct EncryptionKeys {
    public: PKey<Public>,
    private: PKey<Private>,
}

struct ServerToken {
    token: String,
}

#[derive(Serialize, Deserialize)]
#[serde(crate = "rocket::serde")]
pub struct VerifiedRequest {
    pub server_token: String,
    pub command: String,
    pub param: Option<String>,
}

#[launch]
async fn rocket() -> _ {
    let token = rocket::tokio::fs::read_to_string("/var/crypto-medium/conf/token")
        .await
        .unwrap();
    let token = format!("validation-token-{}", token.trim());

    let cert_pem = rocket::tokio::fs::read("/var/crypto-medium/conf/certs/crypto-medium.crt")
        .await
        .unwrap();
    let key_pem = rocket::tokio::fs::read("/var/crypto-medium/conf/certs/crypto-medium.key")
        .await
        .unwrap();

    let cert = X509::from_pem(&cert_pem).unwrap();

    let public_key = cert.public_key().unwrap();
    let private_key = openssl::pkey::PKey::private_key_from_pem(&key_pem).unwrap();

    let config = rocket::Config {
        address: "0.0.0.0".parse().unwrap(),
        tls: Some(TlsConfig::from_bytes(&cert_pem, &key_pem)),
        ..Default::default()
    };

    let var_name = routes![validate_command, execute_command];
    rocket::custom(config)
        .manage(ServerToken { token })
        .manage(EncryptionKeys {
            public: public_key,
            private: private_key,
        })
        .mount("/", var_name)
}
