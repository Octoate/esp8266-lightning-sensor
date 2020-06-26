// Reset configurations for testing purposes
// #define RESET_WIFI_CONFIG
// #define RESET_PARAMETER_CONFIG

// WiFi configuration
static const char wifiConfigSsid[] = "Lightning Sensor";
static const char wifiConfigPassword[] = "lightning";

// define your default values here, if there are different values in config.json, they are overwritten.
char mqtt_server[40];
char mqtt_port[6] = "1883";