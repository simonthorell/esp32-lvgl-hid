#include "connect_mqtt.h"
#include <inttypes.h>
#include "esp_log.h"
#include "esp_tls.h"
#include "string.h"
#include "secret_credentials.h" // Application secrets
#include "aes_encrypt.h" // Include the AES encryption header file
#include "cJSON.h" 

static const char *TAG = "MQTT_CLIENT";

extern const uint8_t server_cert_pem_start[] asm("_binary_ca_chain_pem_start");
extern const uint8_t server_cert_pem_end[] asm("_binary_ca_chain_pem_end");

// Global variable for the MQTT client handle
static esp_mqtt_client_handle_t client = NULL;

static void log_error_if_nonzero(const char *message, int error_code)
{
    if (error_code != 0) {
        ESP_LOGE(TAG, "Last error %s: 0x%x", message, error_code);
    }
}

// Event handler for MQTT events
static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    ESP_LOGD(TAG, "Event dispatched from event loop base=%s, event_id=%" PRId32, base, event_id);
    esp_mqtt_event_handle_t event = event_data;

    switch ((esp_mqtt_event_id_t)event_id) {
        case MQTT_EVENT_CONNECTED:
            ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");
            break;
        case MQTT_EVENT_DISCONNECTED:
            ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
            break;
        case MQTT_EVENT_SUBSCRIBED:
            ESP_LOGI(TAG, "MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);
            break;
        case MQTT_EVENT_UNSUBSCRIBED:
            ESP_LOGI(TAG, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
            break;
        case MQTT_EVENT_PUBLISHED:
            ESP_LOGI(TAG, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
            break;
        case MQTT_EVENT_DATA:
            ESP_LOGI(TAG, "MQTT_EVENT_DATA");
            printf("TOPIC=%.*s\r\n", event->topic_len, event->topic);
            printf("DATA=%.*s\r\n", event->data_len, event->data);
            break;
        case MQTT_EVENT_ERROR:
            ESP_LOGI(TAG, "MQTT_EVENT_ERROR");
            if (event->error_handle->error_type == MQTT_ERROR_TYPE_TCP_TRANSPORT) {
                log_error_if_nonzero("reported from esp-tls", event->error_handle->esp_tls_last_esp_err);
                log_error_if_nonzero("reported from tls stack", event->error_handle->esp_tls_stack_err);
                log_error_if_nonzero("captured as transport's socket errno", event->error_handle->esp_transport_sock_errno);
                ESP_LOGI(TAG, "Last errno string (%s)", strerror(event->error_handle->esp_transport_sock_errno));
            }
            break;
        default:
            ESP_LOGI(TAG, "Other event id:%d", event->event_id);
            break;
    }
}

void mqtt_app_start(void) {
    //********************************************************************************
    // TODO: Create an esp_tls_cfg_t struct for SSL/TLS configuration
    //           / STH 2024-01-30
    //********************************************************************************
    const esp_mqtt_client_config_t mqtt_cfg = {
        .broker = {
            .address = {
                .uri = MQTT_BROKER_URL,
                .port = MQTT_BROKER_PORT,
            },
            .verification = {
                .certificate = (const char *)server_cert_pem_start,
            },
        },
        .credentials = { // Uncomment and set these if your broker requires authentication
            .username = MQTT_BROKER_USERNAME,
            .authentication = {
                .password = MQTT_BROKER_PASSWORD,
            },
        }
    };

    client = esp_mqtt_client_init(&mqtt_cfg); //
    // Register event handler and other necessary MQTT client setup
    esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, client);

    esp_mqtt_client_start(client);
}

void mqtt_publish(const char *topic, const char *data)
{
    if (client == NULL) {
        ESP_LOGE(TAG, "Client hasn't been initialized");
        return;
    }

    int msg_id = esp_mqtt_client_publish(client, topic, data, 0, 1, 0);
    ESP_LOGI(TAG, "sent publish successful, msg_id=%d", msg_id);
}

// Parse and encrypt the UI fields, then send over MQTT
void send_credentials_over_mqtt(lv_obj_t* emailField, lv_obj_t* passwordField) {
    uint8_t key[32] = AES_256_ENCRYPTION_KEY; // Defined in secret_credentials.h

    // Initialize the IV and generate a random IV
    uint8_t iv[AES_BLOCK_SIZE];
    generate_random_iv(iv, sizeof(iv));

    // Get text from the UI fields
    const char* email = lv_textarea_get_text(emailField);
    const char* password = lv_textarea_get_text(passwordField);

    // Get the lengths of the email and password
    size_t email_len = strlen(email);
    size_t password_len = strlen(password);

    // Calculate the padded lengths
    size_t padded_email_len = email_len + (AES_BLOCK_SIZE - (email_len % AES_BLOCK_SIZE));
    size_t padded_password_len = password_len + (AES_BLOCK_SIZE - (password_len % AES_BLOCK_SIZE));

    // Allocate memory for the padded email and password
    uint8_t *padded_email = malloc(padded_email_len);
    uint8_t *padded_password = malloc(padded_password_len);

    // Copy the original data and add PKCS#7 padding
    memcpy(padded_email, email, email_len);
    memset(padded_email + email_len, (uint8_t)(padded_email_len - email_len), padded_email_len - email_len);
    memcpy(padded_password, password, password_len);
    memset(padded_password + password_len, (uint8_t)(padded_password_len - password_len), padded_password_len - password_len);

    // Encrypt the padded email and password
    uint8_t encryptedEmail[padded_email_len];
    uint8_t encryptedPassword[padded_password_len];
    if (aes256_encrypt_cbc(padded_email, padded_email_len, key, iv, encryptedEmail) != 0 ||
        aes256_encrypt_cbc(padded_password, padded_password_len, key, iv, encryptedPassword) != 0) {
        ESP_LOGE(TAG, "Encryption failed!");
        free(padded_email);
        free(padded_password);
        return;
    }

    // Encode encrypted data to Base64
    char* base64Email = base64_encode(encryptedEmail, sizeof(encryptedEmail));
    char* base64Password = base64_encode(encryptedPassword, sizeof(encryptedPassword));
    if (!base64Email || !base64Password) {
        ESP_LOGE(TAG, "Base64 encoding failed!");
        free(base64Email); // Free resources if allocated
        free(base64Password); // Free resources if allocated
        return;
    }

    // Create a JSON object for the MQTT message
    cJSON* mqttMessage = cJSON_CreateObject();
    if (!mqttMessage) {
        ESP_LOGE(TAG, "Failed to create MQTT message!");
        free(base64Email); // Free resources
        free(base64Password); // Free resources
        return;
    }

    // Add the encrypted and encoded credentials to the JSON object
    cJSON_AddStringToObject(mqttMessage, "email", base64Email);
    cJSON_AddStringToObject(mqttMessage, "password", base64Password);

    // Option 1. Convert IV to hexadecimal string for JSON
    // char ivHex[2 * AES_BLOCK_SIZE + 1]; // Each byte is represented by two hex characters
    // for (int i = 0; i < AES_BLOCK_SIZE; i++) {
    //     sprintf(ivHex + (i * 2), "%02x", iv[i]);
    // }
    // cJSON_AddStringToObject(mqttMessage, "iv", ivHex);

    // Option 2. Send iv as cleartext string
    // char ivStr[AES_BLOCK_SIZE + 1]; // +1 for the null terminator
    // memcpy(ivStr, iv, AES_BLOCK_SIZE);
    // ivStr[AES_BLOCK_SIZE] = '\0'; // Null-terminate the string
    // cJSON_AddStringToObject(mqttMessage, "iv", ivStr);

    // Option 3. Send iv as Base64 string
    char* ivBase64 = base64_encode(iv, AES_BLOCK_SIZE);
    cJSON_AddStringToObject(mqttMessage, "iv", ivBase64); // Add the Base64 encoded IV to the JSON object
    // Do not forget to free!

    // Convert the MQTT message to a string
    char* mqttMessageStr = cJSON_PrintUnformatted(mqttMessage);
    if (!mqttMessageStr) {
        ESP_LOGE(TAG, "Failed to print MQTT message!");
        cJSON_Delete(mqttMessage);
        free(base64Email); // Free resources
        free(base64Password); // Free resources
        return;
    }

    // Publish the MQTT message
    mqtt_publish(MQTT_TOPIC, mqttMessageStr);

    // Clean up
    cJSON_Delete(mqttMessage);
    free(mqttMessageStr);
    free(base64Email);
    free(base64Password);
    free(ivBase64);

    ESP_LOGI(TAG, "Credentials sent over MQTT (Encrypted)");
}

// Function to parse the UI-textfields and send the credentials over MQTT (no encryption)
// void send_credentials_over_mqtt(lv_obj_t* emailField, lv_obj_t* passwordField) {
//     const char* email = lv_textarea_get_text(emailField);
//     const char* password = lv_textarea_get_text(passwordField);

//     // Define the maximum lengths for the email and password
//     const int maxEmailLength = 35;
//     const int maxPasswordLength = 35;
//     const int maxJsonLength = maxEmailLength + maxPasswordLength + 50; // Extra space for JSON formatting

//     // Allocate memory for the JSON message on the stack
//     char jsonMessage[maxJsonLength];

//     // Format the message as JSON
//     snprintf(jsonMessage, sizeof(jsonMessage), "{\"email\":\"%s\", \"password\":\"%s\"}", email, password);

//     // Publish the JSON message to the MQTT topic
//     mqtt_publish(MQTT_TOPIC, jsonMessage);

//     ESP_LOGI(TAG, "Credentials sent over MQTT: [JSON hidden]");
// }