#include <WiFi.h>
#include <WebServer.h>
#include <ArduinoJson.h>

//wifi config
#define ssid "IZZI-0EA1"
#define password "escorpion123"

//leds pins
#define led_temperatura 19
#define led_error1 21
#define led_error2 22
#define led_error3 23

WebServer server(80);

void handleLedControl() {

  // CORS
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.sendHeader("Access-Control-Allow-Methods", "POST, GET, OPTIONS");
  server.sendHeader("Access-Control-Allow-Headers", "Content-Type");

  if (server.method() == HTTP_OPTIONS) {
    server.send(204);
    return;
  }

  if (!server.hasArg("temp")) {
    server.send(400, "text/plain", "Falta parametro de temperatura");
    return;
  }

  int temperatura = server.arg("temp").toInt();
  int temperatura_limite = 200;

  if (temperatura >= temperatura_limite) {
    digitalWrite(led_temperatura, HIGH);
  } else {
    digitalWrite(led_temperatura, LOW);
  }

  server.send(200, "text/plain", "TEMPERATURA RECIBIDA");
}

void handleErrores() {

  // CORS
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.sendHeader("Access-Control-Allow-Methods", "POST, GET, OPTIONS");
  server.sendHeader("Access-Control-Allow-Headers", "Content-Type");

  if (server.method() == HTTP_OPTIONS) {
    server.send(204);
    return;
  }

  if (server.method() != HTTP_POST) {
    server.send(400, "text/plain", "HTTP equivocado");
    return;
  }

  String body = server.arg("plain");
  StaticJsonDocument<200> doc;

  DeserializationError error = deserializeJson(doc, body);
  if (error) {
    server.send(400, "application/json", "{\"error\":\"JSON inválido\"}");
    return;
  }

  //recibir errores correctamente
  bool error1 = doc["error1"];
  bool error2 = doc["error2"];
  bool error3 = doc["error3"];

  digitalWrite(led_error1, error1 ? HIGH : LOW);
  digitalWrite(led_error2, error2 ? HIGH : LOW);
  digitalWrite(led_error3, error3 ? HIGH : LOW);

  server.send(200, "text/plain", "ok");
}

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);

  pinMode(led_temperatura, OUTPUT);
  pinMode(led_error1, OUTPUT);
  pinMode(led_error2, OUTPUT);
  pinMode(led_error3, OUTPUT);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Conectado a WiFi");
  Serial.println(WiFi.localIP());

  server.on("/temperatura", HTTP_ANY, handleLedControl);
  server.on("/errores", HTTP_ANY, handleErrores);

  server.begin();
}

void loop() {
  server.handleClient();
}
