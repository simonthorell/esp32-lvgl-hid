// #ifndef SECRET_CREDENTIALS_H
// #define SECRET_CREDENTIALS_H

//==============================================================================
// Change the name of this file to secret_credentials.h
// Uncomment the lines and fill in the credentials to use the example
//==============================================================================

// AES Encryption key (16 bytes exactly)
// #define AES_128_ENCRYPTION_KEY "<YOUR-KEY>"

// Wi-Fi credentials
// #define EXAMPLE_ESP_WIFI_SSID "<YOUR-SSID>"
// #define EXAMPLE_ESP_WIFI_PASS "<YOUR-PASSWORD>"

// MQTT credentials
// #define MQTT_BROKER_URL "mqtts://<YOUR-BROKER-URL>"
// #define MQTT_BROKER_PORT 8883
// #define MQTT_BROKER_USERNAME "<YOUR-USERNAME>"
// #define MQTT_BROKER_PASSWORD "<YOUR-PASSWORD>"

/*================================================================================
// How to get the certificate and key:
1. open a terminal and download openSSL (brew install openssl)
2. run the following command to check the certificate of the broker:
    (change the URL and port to the one you want to check)
    openssl s_client -showcerts -connect mqtt.<BROKER_URL>:<PORT> </dev/null
3. Copy each certificate (between -----BEGIN CERTIFICATE----- and 
    -----END CERTIFICATE-----) to the ca_chain.pem file and save it.
4. Add the certificate file in src/CMakeLists.txt
//==============================================================================*/

// #endif // SECRET_CREDENTIALS_H