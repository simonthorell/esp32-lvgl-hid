#include "connect_mqtt.h"
#include <inttypes.h>
#include "esp_log.h"
#include "esp_tls.h"
#include "string.h"
#include "secret_credentials.h" // Define your MQTT broker URL, username and password in this file

static const char *TAG = "MQTT_CLIENT";

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
    const esp_mqtt_client_config_t mqtt_cfg = {
        .broker = {
            .address = {
                .uri = MQTT_BROKER_URL,
                .port = MQTT_BROKER_PORT,
            },
        },
        .credentials = { // Uncomment and set these if your broker requires authentication
            .username = MQTT_BROKER_USERNAME,
            .authentication = {
                .password = MQTT_BROKER_PASSWORD,
            },
        }
    };

    //********************************************************************************
    // TODO: Create an esp_tls_cfg_t struct for SSL/TLS configuration
    //           / STH 2024-01-30
    //********************************************************************************

    client = esp_mqtt_client_init(&mqtt_cfg);
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

// Function to parse the UI-textfields and send the credentials over MQTT
void send_credentials_over_mqtt(lv_obj_t* emailField, lv_obj_t* passwordField) {
    const char* email = lv_textarea_get_text(emailField);
    const char* password = lv_textarea_get_text(passwordField);

    // Define the maximum lengths for the email and password
    const int maxEmailLength = 35;
    const int maxPasswordLength = 35;
    const int maxJsonLength = maxEmailLength + maxPasswordLength + 50; // Extra space for JSON formatting

    // Allocate memory for the JSON message on the stack
    char jsonMessage[maxJsonLength];

    // Format the message as JSON
    snprintf(jsonMessage, sizeof(jsonMessage), "{\"email\":\"%s\", \"password\":\"%s\"}", email, password);

    // Publish the JSON message to the MQTT topic
    mqtt_publish(MQTT_TOPIC, jsonMessage);

    ESP_LOGI(TAG, "Credentials sent over MQTT: [JSON hidden]");
}