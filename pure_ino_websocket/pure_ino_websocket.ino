#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ArduinoWebsockets.h>
#include <Esp.h>

#define LOCAL_MODE false
#define LOCAL_IP "http://192.168.15.8"
#define LOCAL_PORT "8080"
#define SERVER_HOST "https://pure-express-websocket.herokuapp.com/"
#define CLIENT_ID "123"

using namespace websockets;
WiFiClient client;
WebsocketsClient wsclient;

char ssidDev[] = "Romagnolo 2.4G";
char passwordDev[] = "melzinha123";

String getServerUri (bool devMode) {
  if (devMode) {
    return String(LOCAL_IP) + ":" + String(LOCAL_PORT);
  } else {
   return String(SERVER_HOST);
  }
}

void verifyConnection() {
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("...connecting!");
    ESP.wdtFeed();
  }
  Serial.println("Connected with wifi network");
}

void initWifi(String ssid = "noop", String password = "noop") {
  WiFi.begin(ssid, password);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssidDev, passwordDev);
  verifyConnection();
}

void recoverWiFiConnection() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Network connection lost!");
    WiFi.disconnect();
    delay(200);
    WiFi.reconnect();
    delay(500);
    yield();
    ESP.wdtFeed();
  }
}

void onEventsCallback(WebsocketsEvent event, String data) {
  if (event == WebsocketsEvent::ConnectionOpened) {
    Serial.println("Connnection Opened");
    wsclient.ping("test");
  } else if (event == WebsocketsEvent::ConnectionClosed) {
    handleWebsocketConnection();
  } else if (event == WebsocketsEvent::GotPing) {
    Serial.println(data);
  } else if (event == WebsocketsEvent::GotPong) {
    Serial.println("Got a Pong!");
  }

  ESP.wdtFeed();
}

void handleWebsocketConnection () {
  // Serial.println(wsclient.available());
  if (wsclient.available()) {
    wsclient.poll();
    ESP.wdtFeed();
  } else {
    bool connected = wsclient.connect(getServerUri(LOCAL_MODE));
    if (connected) {
    Serial.println("Connected with websocket server!");
    } else {
      Serial.println("...Reconnecting to websocket server");
    }
  }
}


void setup() {
  // ESP.enableWDT();
  Serial.begin(115200);
  while (!Serial); // Waiting for Serial Monitor

  initWifi();

  handleWebsocketConnection();
  wsclient.addHeader("Connection", "Keep-alive");
  // websocket events
  wsclient.onEvent(onEventsCallback);

  // callback where the messages are received
  wsclient.onMessage([&](WebsocketsMessage message) {
    // pongTimer = millis();
    String messageFromRemote = message.data();
    Serial.print("Message from server: ");
    Serial.println(message.data());
    ESP.wdtFeed();
  });
}

void loop() {
  recoverWiFiConnection();
  handleWebsocketConnection();
  ESP.wdtFeed();
}
