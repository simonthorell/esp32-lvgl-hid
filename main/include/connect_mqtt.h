#ifndef CONNECT_MQTT_H
#define CONNECT_MQTT_H

#include "mqtt_client.h"
#include "ui/ui.h"
#include "gui_textfields.h"

#define MQTT_TOPIC "iot23-st/credentials"
#define MAX_EMAIL_LENGTH 50
#define MAX_PASSWORD_LENGTH 50

// Initialize MQTT
void mqtt_app_start(void);

// Publish message
void mqtt_publish(const char *topic, const char *data);

// Parse the textfields and send the credentials over MQTT
void send_credentials_over_mqtt(lv_obj_t* emailField, lv_obj_t* passwordField);

#endif // CONNECT_MQTT_H