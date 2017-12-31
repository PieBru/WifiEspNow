#include <ESP8266WiFi.h>
#include <WifiEspNowBroadcast.h>

static const int BUTTON_PIN = 0; // "flash" button on NodeMCU, Witty Cloud, etc
static const int LED_PIN = 2;    // ESP-12F blue LED

int ledState = HIGH;

void processRx(const uint8_t mac[6], const uint8_t* buf, size_t count, void* cbarg) {
  Serial.printf("Message from %02X:%02X:%02X:%02X:%02X:%02X\n", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
  for (int i = 0; i < count; ++i) {
    Serial.print(static_cast<char>(buf[i]));
  }
  Serial.println();

  digitalWrite(LED_PIN, ledState);
  ledState = 1 - ledState;
}

void setup() {
  Serial.begin(115200);
  Serial.println();

  Serial.print("MAC address of this node is ");
  Serial.println(WiFi.softAPmacAddress());

  WiFi.persistent(false);
  bool ok = WifiEspNowBroadcast.begin("ESPNOW", 3);
  if (!ok) {
    Serial.println("WifiEspNowBroadcast.begin() failed");
    ESP.restart();
  }

  WifiEspNowBroadcast.onReceive(processRx, nullptr);

  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, ledState);
}

void sendMessage() {
  char msg[60];
  int len = snprintf(msg, sizeof(msg), "hello ESP-NOW from %s at %lu", WiFi.softAPmacAddress().c_str(), millis());
  WifiEspNowBroadcast.send(reinterpret_cast<const uint8_t*>(msg), len);
}

void loop() {
  if (digitalRead(BUTTON_PIN) == LOW) { // button is pressed
    sendMessage();

    while (digitalRead(BUTTON_PIN) == LOW) // wait for button release
      ;
  }

  WifiEspNowBroadcast.loop();
  delay(10);
}
