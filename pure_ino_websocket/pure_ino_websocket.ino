#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ArduinoWebsockets.h>

#define LOCAL_MODE true
#define LOCAL_IP "http://192.168.15.8"
#define LOCAL_PORT "8080"
#define SERVER_HOST "https://tobas"
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
  Serial.println("BeThere connected");
}

void initWifi(String ssid = "noop", String password = "noop") {
  WiFi.begin(ssid, password);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssidDev, passwordDev);
  verifyConnection();
}

void recoverWiFiConnection() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Connection lost!");
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
  } else if (event == WebsocketsEvent::ConnectionClosed) {
    Serial.println("Connnection Closed");
  } else if (event == WebsocketsEvent::GotPing) {
    Serial.println("Got a Ping!");
  } else if (event == WebsocketsEvent::GotPong) {
    Serial.println("Got a Pong!");
  }
}

void handleWebsocketConnection () {
  if (wsclient.available()) {
    wsclient.poll();
  } else {
    Serial.println("reconnecting to websocket server...");
    bool connected = wsclient.connect(getServerUri(LOCAL_MODE));
    Serial.println(getServerUri(LOCAL_MODE));
    if (connected) {
      Serial.println("websocket connected");
    }
  }
}


void setup() {
  Serial.begin(115200);
  while (!Serial); // Waiting for Serial Monitor

  initWifi();

  bool connected = wsclient.connect(getServerUri(LOCAL_MODE));
  
  if (connected) {
    Serial.println("Connected with websocket server!");
  } else {
    Serial.println("Not Connected!");
  }

  // websocket events
  wsclient.onEvent(onEventsCallback);

  // callback where the messages are received
  wsclient.onMessage([&](WebsocketsMessage message) {
    // pongTimer = millis();
    String messageFromRemote = message.data();
    Serial.print("Message from server: ");
    Serial.println(message.data());
  });
}

void loop() {
  // put your main code here, to run repeatedly:
  recoverWiFiConnection();
  handleWebsocketConnection();
}
