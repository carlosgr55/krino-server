#include <WiFi.h>
#include <WebServer.h>

//wifi config
#define ssid "iPhone de Ruben"
#define password "Motomami5"

//leds pins
#define led_temperatura 19
#define motor 21
#define frenos 22
#define bateria 23

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

  if (server.method() != HTTP_GET) {
    server.send(400, "text/plain", "HTTP equivocado");
    return;
  }

  //Si no tiene modulo y state se erra
  if (!(server.hasArg("modulo") && server.hasArg("state"))) {
    server.send(400, "text/plain", "Faltan argumentos");
    return;
  }

  String modulo = server.arg("modulo");
  String state = server.arg("state");

  if (modulo == "motor") {
    digitalWrite(motor, state == "ON" ? HIGH : LOW);

  } else if (modulo == "bateria") {
    digitalWrite(bateria, state == "ON" ? HIGH : LOW);

  } else if (modulo == "frenos") {
    digitalWrite(frenos, state == "ON" ? HIGH : LOW);

  } else {
    server.send(400, "text/plain", "ESTADO NO VALIDO");
    return;
  }

  server.send(200, "text/plain", "ok");
}

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);

  pinMode(led_temperatura, OUTPUT);
  pinMode(motor, OUTPUT);
  pinMode(bateria, OUTPUT);
  pinMode(frenos, OUTPUT);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Conectado a WiFi");
  Serial.println(WiFi.localIP());

  server.on("/temperatura", HTTP_ANY, handleLedControl);
  server.on("/DTC", HTTP_ANY, handleErrores);

  server.begin();
}

void loop() {
  server.handleClient();
}
