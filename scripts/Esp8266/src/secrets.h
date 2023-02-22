#include <pgmspace.h>

#define SECRET
#define THINGNAME "pytinhas"

const char WIFI_SSID[] = "Falconfibra Francielly_2.4G";
const char WIFI_PASSWORD[] = "Pelucia12";

int8_t TIME_ZONE = -3;

const char MQTT_HOST[] = "a22fqccedfv11r-ats.iot.us-east-1.amazonaws.com";

static const char cacert[] PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----

-----END CERTIFICATE-----
)EOF";


static const char client_cert[] PROGMEM =R"KEY(
-----BEGIN CERTIFICATE-----

)KEY";


static const char privkey[] PROGMEM =R"KEY(
-----BEGIN RSA PRIVATE KEY-----

-----END RSA PRIVATE KEY-----
)KEY";
