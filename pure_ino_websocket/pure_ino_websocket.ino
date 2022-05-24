#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ArduinoWebsockets.h>
#include <Esp.h>

#define LOCAL_MODE true
#define LOCAL_IP "http://192.168.0.26"
#define LOCAL_PORT "8080"
#define SERVER_HOST "ws://pure-express-websocket.herokuapp.com"
#define CLIENT_ID "123"

using namespace websockets;
WiFiClient client;
WebsocketsClient wsclient;

//char ssidDev[] = "Romagnolo 2.4G";
//char passwordDev[] = "melzinha123";
char ssidDev[] = "Satan`s Connection";
char passwordDev[] = "tininha157";

unsigned long maxPingInterval = 40000; // 40 secs
unsigned long pingTimer = 0;

const char *serialKeyClient_1 = "A0CAA-DN6PV-6U2OD-NPY1Q";

String getServerUri (bool devMode) {
  String host = devMode 
    ? String(LOCAL_IP) + ":" + String(LOCAL_PORT)
    : String(SERVER_HOST);
  Serial.println(String(host) + '/' + String(serialKeyClient_1));
  return String(host) + '/' + String(serialKeyClient_1);
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

    ESP.wdtFeed();
  }
}

void onPing(String pingMessage) {
  Serial.println(pingMessage);

  if (pingTimer == 0) {
   pingTimer = millis();
  }
}

void onEventsCallback(WebsocketsEvent event, String data) {
  if (event == WebsocketsEvent::ConnectionOpened) {
    Serial.println("Connnection Opened");
    wsclient.ping("test");
  } else if (event == WebsocketsEvent::ConnectionClosed) {
    Serial.println("Connection Closed");
  } else if (event == WebsocketsEvent::GotPing) {
    onPing(data);
  } else if (event == WebsocketsEvent::GotPong) {
    Serial.println("Got a Pong!");
  }
  ESP.wdtFeed();
}

void handleWebsocketConnection () {
  if (wsclient.available()) {
    wsclient.poll();
    // if client is available and there is no ping between max interval, force close.
    if (millis() - pingTimer > maxPingInterval) {
      Serial.println("No response from server, close connection");
      wsclient.close();
      pingTimer = 0;
    }
  } else {
    bool connected = wsclient.connect(getServerUri(LOCAL_MODE));
    if (connected) {
    Serial.println("Connected with websocket server!");
    } else {
      Serial.println("...Reconnecting to websocket server");
    }
  }
  ESP.wdtFeed();
}


void setup() {
  Serial.begin(115200);
  while (!Serial); // Waiting for Serial Monitor

  initWifi();

  handleWebsocketConnection();
  // wsclient.addHeader("Connection", "Keep-alive");
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
