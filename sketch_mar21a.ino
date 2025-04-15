#include <Arduino.h>
#include <Wire.h>
#include <EEPROM.h>
#include <WiFi.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Adafruit_ADS1X15.h>
#include <DFRobot_ESP_EC.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

#define ONE_WIRE_BUS 4
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

DFRobot_ESP_EC ec;
Adafruit_ADS1115 ads;

float voltage, ecValue, temperature = 25;

String apiKey = "09TN9P1DMWQIQZ65";

const char *ssid = "Le Binh";
const char *pass = "0939099873";
const char *server = "api.thingspeak.com";

// AWS IoT endpoint
const char *mqttServer = "a32stwsikc6rzu-ats.iot.us-east-1.amazonaws.com";
const int mqttPort = 8883;

// AWS IoT certificates
const char *rootCA = R"EOF(
-----BEGIN CERTIFICATE-----
MIIDQTCCAimgAwIBAgITBmyfz5m/jAo54vB4ikPmljZbyjANBgkqhkiG9w0BAQsF
ADA5MQswCQYDVQQGEwJVUzEPMA0GA1UEChMGQW1hem9uMRkwFwYDVQQDExBBbWF6
b24gUm9vdCBDQSAxMB4XDTE1MDUyNjAwMDAwMFoXDTM4MDExNzAwMDAwMFowOTEL
MAkGA1UEBhMCVVMxDzANBgNVBAoTBkFtYXpvbjEZMBcGA1UEAxMQQW1hem9uIFJv
b3QgQ0EgMTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBALJ4gHHKeNXj
ca9HgFB0fW7Y14h29Jlo91ghYPl0hAEvrAIthtOgQ3pOsqTQNroBvo3bSMgHFzZM
9O6II8c+6zf1tRn4SWiw3te5djgdYZ6k/oI2peVKVuRF4fn9tBb6dNqcmzU5L/qw
IFAGbHrQgLKm+a/sRxmPUDgH3KKHOVj4utWp+UhnMJbulHheb4mjUcAwhmahRWa6
VOujw5H5SNz/0egwLX0tdHA114gk957EWW67c4cX8jJGKLhD+rcdqsq08p8kDi1L
93FcXmn/6pUCyziKrlA4b9v7LWIbxcceVOF34GfID5yHI9Y/QCB/IIDEgEw+OyQm
jgSubJrIqg0CAwEAAaNCMEAwDwYDVR0TAQH/BAUwAwEB/zAOBgNVHQ8BAf8EBAMC
AYYwHQYDVR0OBBYEFIQYzIU07LwMlJQuCFmcx7IQTgoIMA0GCSqGSIb3DQEBCwUA
A4IBAQCY8jdaQZChGsV2USggNiMOruYou6r4lK5IpDB/G/wkjUu0yKGX9rbxenDI
U5PMCCjjmCXPI6T53iHTfIUJrU6adTrCC2qJeHZERxhlbI1Bjjt/msv0tadQ1wUs
N+gDS63pYaACbvXy8MWy7Vu33PqUXHeeE6V/Uq2V8viTO96LXFvKWlJbYK8U90vv
o/ufQJVtMVT8QtPHRh8jrdkPSHCa2XV4cdFyQzR1bldZwgJcJmApzyMZFo6IQ6XU
5MsI+yMRQ+hDKXJioaldXgjUkK642M4UwtBV8ob2xJNDd2ZhwLnoQdeXeGADbkpy
rqXRfboQnoZsG4q5WTP468SQvvG5
-----END CERTIFICATE-----
)EOF";

const char *clientCert = R"EOF(
-----BEGIN CERTIFICATE-----
MIIDWTCCAkGgAwIBAgIUMiPc2jk03mJ3BMjvmRyzEuDFp5QwDQYJKoZIhvcNAQEL
BQAwTTFLMEkGA1UECwxCQW1hem9uIFdlYiBTZXJ2aWNlcyBPPUFtYXpvbi5jb20g
SW5jLiBMPVNlYXR0bGUgU1Q9V2FzaGluZ3RvbiBDPVVTMB4XDTI1MDMwNzEzNDU1
NVoXDTQ5MTIzMTIzNTk1OVowHjEcMBoGA1UEAwwTQVdTIElvVCBDZXJ0aWZpY2F0
ZTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAK0lTcju8yNiu2CqeAC8
OB9wPbQAfUf0J+vK7jzmJpM3etXiBm5QWoxh+4ukekqtMYCnAK4xgKbe+pulEFm8
UxGiYw+QJm6a4ENpQEuezS17cdbC7JfiL7TRpcU8u0nf0PzEXHQn37Yf1SMqPMAk
nc7K6Fn0ICYxTdcrW1ol8iT/AARFaUW43XU2dENojVVH7T2Mb0MkLeZC7marJFur
w/JxLnYbWDT5NuqWpsHEPhgp0Wi5Kv32tQkH9DvS+GtOfbkfH7umD+wGR3w+3+9l
o9Ephx0JGAK+cNEdTxPLl3Zg+1tS1ecmi6ZpGo2C47dw6IwntTzjyrYRhaRUmyq4
eKECAwEAAaNgMF4wHwYDVR0jBBgwFoAU6Uf9U2Sc4OVu/PHuz/0npXTcQi8wHQYD
VR0OBBYEFOPRBcg9CAoFZ3sIv418apqHmLWXMAwGA1UdEwEB/wQCMAAwDgYDVR0P
AQH/BAQDAgeAMA0GCSqGSIb3DQEBCwUAA4IBAQAd9SU/9koj8IBERcdyHCwpBu+y
k0085fXTx/4zel0Blrr3c2Gv4VT6POUslfRABaOw2++WB55A0SYU1mIQn7NkbE20
fAVfwJzqyL3DoI3A9M9gK0zWynO4daCztoWNhlCjHU5AwqVWsHFiDTH1dq+0YECt
omzotGujPri+EAjjRL9xNoUpzMtiahQ3lfouf057b1IqX8iWTT1eqHQC1Hx5GMDG
Hcj/zqIkgYAA3pq/7BMpSYJ/OelAY+2doCWtF90Oh2P5NdbhADamV4gbwugVQWH8
cMbrNFes4+ekaVUB2kGIz2ZM7sGp7UXkIcdFEyrMcXXYhrjMc7J0RAbnJzmE
-----END CERTIFICATE-----
)EOF";

const char *privateKey = R"EOF(
-----BEGIN RSA PRIVATE KEY-----
MIIEpQIBAAKCAQEArSVNyO7zI2K7YKp4ALw4H3A9tAB9R/Qn68ruPOYmkzd61eIG
blBajGH7i6R6Sq0xgKcArjGApt76m6UQWbxTEaJjD5AmbprgQ2lAS57NLXtx1sLs
l+IvtNGlxTy7Sd/Q/MRcdCffth/VIyo8wCSdzsroWfQgJjFN1ytbWiXyJP8ABEVp
RbjddTZ0Q2iNVUftPYxvQyQt5kLuZqskW6vD8nEudhtYNPk26pamwcQ+GCnRaLkq
/fa1CQf0O9L4a059uR8fu6YP7AZHfD7f72Wj0SmHHQkYAr5w0R1PE8uXdmD7W1LV
5yaLpmkajYLjt3DojCe1POPKthGFpFSbKrh4oQIDAQABAoIBAHwbwSXGIYG7w5QG
nP3p7FCco53iu+pXmEXrr9j3H69mlSW6XeG0tzLpCOWPgBkHC/SrwFEcKptyOTBX
oOCxnNmHDHqbWPDvEtE/IFcT/P4c/fxyvCQifQ+fNrOqVJUfWS3Vkm+wJSrmpmAO
Zn1Twcn4p5PS6Th3HtScv15e1LKu51Ikxf2nqxljyjVMmIlmxShc/VjRKXzxan/q
72e/YwHSWrNPuny6haXqrK80DHc7/O3miPXprBy1QpehOxTHwccqzunanD9+510e
dVzcokC5jfnTaDgDQmxGXQjUDU9qy0AAIFy6JAJQbB70X9DHZRrjzDzncf8GGNv0
1MxjngECgYEA4TUX/T3AYneS8vUl7f7dO/aIl9JS4Yr/He3YSNDtFUctluoBz3dP
uaHEKv1o1mEM0qZ02Dz50rhmQMJEu6bhzR7AExl/BJLtdfQmW7s2SbfAKMViWuJK
r64hizYxe5kmQgbD3b7sub9py4d2LLR8apvs3v0lfc325C4F2er2yscCgYEAxNHm
1pF8ytquH4MmS9xZhfJs9zwS2u2aca9230bT5h4Rj9fmtlzDSSPwByKyRf9DbL48
szWNF0s32h5UlnTjf7F/BCZ11X6d1pxK/Oe0lqA8S+0X551szy1mFyPJa+OM46XC
wqMy1yc8F092v7PrIVOU9MNMWDTgmqZdIz9y+VcCgYEAi6D4i6mSszpB4OL/rkyb
bEcRL1hWzMH6JNtHgvYOWGf2KyBRGQl3Y7eZQbI2ItyvL5fG0B8WIYD03OfV56az
TxLmsbDYIMTb94r1kWBEa3ZC7MRck87D0aKb540lJuWwY7WAEOcELLIx7N/C9Dq+
/PZJ6YN4kER388dZF2BnvRcCgYEAjcGogu4ydWBffO7WzFieWe0rMj6gTfJIBI2e
Dl5TDDCn8XRNBRd2x90MafdBc0zQ4Sd7TI/LWPWh+oVR4zO5DyVmGDUjNWuGfyX+
lHzGAp7SDtVrHDS0Q9us5gmubvhrZyLiTZGBcpkdnKRtINGTDmPGqytmdzlOc9RS
zKpMQu0CgYEApXlOOtgxVQjiJ0UCwIdznKjm7lUp/dmArpDHOi4QrmTInn7rAI/X
fiI1FlzOPsZ2hpBRNle35tiLiF16RQwkFwYarQHsR9kJYZjVbPN+tNo3W2PlW5a2
18oMgCk5DwPNjRTwjCBoRgIUpMdAF9LZagt41zYQ853dNa34OxpkckY=
-----END RSA PRIVATE KEY-----
)EOF";

WiFiClientSecure espClient;
PubSubClient mqttClient(espClient);

const char *mqttTopicPub = "esp32/pub";
const char *mqttTopicSub = "esp32/sub";

// **Khai báo chân LED**
#define LED_GREEN 26  // Đèn xanh lá
#define LED_RED 27    // Đèn đỏ

void connectAWS() {
  Serial.print("Connecting to AWS IoT...");

  espClient.setCACert(rootCA);
  espClient.setCertificate(clientCert);
  espClient.setPrivateKey(privateKey);

  mqttClient.setServer(mqttServer, mqttPort);

  while (!mqttClient.connected()) {
    if (mqttClient.connect("ESP32_water")) {
      Serial.println("Connected to AWS IoT!");
      mqttClient.subscribe(mqttTopicSub);
    } else {
      Serial.print(".");
      delay(1000);
    }
  }
}

void callback(char *topic, byte *payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

void setup() {
  Serial.begin(115200);
  EEPROM.begin(32);
  ec.begin();
  sensors.begin();
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }
  delay(200);
  display.clearDisplay();

  Serial.println("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, pass);

  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected!");

  connectAWS();
  mqttClient.setCallback(callback);

  // **Cấu hình chân LED là OUTPUT**
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_RED, OUTPUT);
}

void loop() {
  if (!mqttClient.connected()) {
    connectAWS();
  }
  mqttClient.loop();

  voltage = analogRead(34);
  sensors.requestTemperatures();
  temperature = sensors.getTempCByIndex(0);
  ecValue = ec.readEC(voltage, temperature);

  Serial.print("Temperature:");
  Serial.print(temperature, 2);
  Serial.println("℃");

  Serial.print("EC:");
  Serial.println(ecValue, 2);


  // **Điều khiển LED theo giá trị EC**
  if (ecValue <= 0.4) {
    digitalWrite(LED_GREEN, HIGH);  // Bật đèn xanh
    digitalWrite(LED_RED, LOW);     // Tắt đèn đỏ
  } else {
    digitalWrite(LED_GREEN, LOW);   // Tắt đèn xanh
    digitalWrite(LED_RED, HIGH);    // Bật đèn đỏ
  }

  display.setTextSize(2);
  display.setTextColor(WHITE);

  display.setCursor(0, 10);
  display.print("T:");
  display.print(temperature, 2);
  display.drawCircle(85, 10, 2, WHITE);
  display.setCursor(90, 10);
  display.print("C");

  display.setCursor(0, 40);
  display.print("EC:");
  display.print(ecValue, 2);
  display.display();
  delay(1500);
  display.clearDisplay();

  ec.calibration(voltage, temperature);

  WiFiClient client; // Khai báo WiFiClient client ở đây
  if (client.connect(server, 80)) {
    String postStr = apiKey;
    postStr += "&field1=";
    postStr += String(temperature, 2);
    postStr += "&field2=";
    postStr += String(ecValue, 2);
    postStr += "\r\n\r\n";
    delay(500);

    client.print("POST /update HTTP/1.1\n");
    client.print("Host: api.thingspeak.com\n");
    client.print("Connection: close\n");
    client.print("X-THINGSPEAKAPIKEY: " + apiKey + "\n");
    client.print("Content-Type: application/x-www-form-urlencoded\n");
    client.print("content-length: ");
    client.print(postStr.length());
    client.print("\n\n");
    client.print(postStr);
    delay(500);
    client.stop();
  }

  String payload = "{";
  payload += "\"temperature\":" + String(temperature, 2) + ",";
  payload += "\"EC\":" + String(ecValue, 2);
  payload += "}";

  Serial.print("Publishing to AWS IoT: ");
  Serial.println(payload);

  if (mqttClient.publish(mqttTopicPub, payload.c_str())) {
    Serial.println("Message sent successfully!");
  } else {
    Serial.println("Message failed to send!");
  }

  delay(5000);
}
