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
#define airbags 4


bool diagnosticando_motor = false;
bool diagnosticando_bateria = false;
bool diagnosticando_frenos = false;
bool diagnosticando_airbags = false;
unsigned long lastBlink = 0;
bool ledState = false;



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

  } else if (modulo == "airbags") {
    digitalWrite(airbags, state == "ON" ? HIGH : LOW);

  } else {
    server.send(400, "text/plain", "ESTADO NO VALIDO");
    return;
  }

  server.send(200, "text/plain", "ok");
}

void deleteDTC() {
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

  digitalWrite(motor, LOW);
  digitalWrite(bateria, LOW);
  digitalWrite(frenos, LOW);
  digitalWrite(airbags, LOW);
  server.send(200, "text/plain", "OK");
}

void handleModulo() {
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
  if (!server.hasArg("modulo")) {
    server.send(400, "text/plain", "Faltan argumentos");
    return;
  }

  String modulo = server.arg("modulo");
  if (modulo == "motor") {
    diagnosticando_motor = true;

  } else if (modulo == "bateria") {
    diagnosticando_bateria = true;
  } else if (modulo == "frenos") {
    diagnosticando_frenos = true;
  } else if (modulo == "airbags") {
    diagnosticando_airbags = true;
  } else if (modulo == "exit") {
    diagnosticando_motor = false;
    diagnosticando_bateria = false;
    diagnosticando_frenos = false;
    diagnosticando_airbags = false;
    digitalWrite(motor, LOW);
    digitalWrite(bateria, LOW);
    digitalWrite(frenos, LOW);
    digitalWrite(airbags, LOW);
  } else {
    server.send(400, "text/plain", "ESTADO NO VALIDO");
    return;
  }
}



void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);

  pinMode(led_temperatura, OUTPUT);
  pinMode(motor, OUTPUT);
  pinMode(bateria, OUTPUT);
  pinMode(frenos, OUTPUT);
  pinMode(airbags, OUTPUT);


  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Conectado a WiFi");
  Serial.println(WiFi.localIP());

  server.on("/temperatura", HTTP_ANY, handleLedControl);
  server.on("/DTC", HTTP_ANY, handleErrores);
  server.on("/deleteDTC", HTTP_ANY, deleteDTC);
  server.on("/moduloDiag", HTTP_ANY, handleModulo);

  server.begin();
}

void loop() {
  server.handleClient();

  if (diagnosticando_motor) {
    unsigned long currentMillis = millis();

    // Parpadeo cada 500 ms
    if (currentMillis - lastBlink >= 500) {
      lastBlink = currentMillis;
      ledState = !ledState;
      digitalWrite(motor, ledState ? HIGH : LOW);
    }
  }
  if (diagnosticando_frenos) {
    unsigned long currentMillis = millis();

    // Parpadeo cada 500 ms
    if (currentMillis - lastBlink >= 500) {
      lastBlink = currentMillis;
      ledState = !ledState;
      digitalWrite(frenos, ledState ? HIGH : LOW);
    }
  }
  if (diagnosticando_bateria) {
    unsigned long currentMillis = millis();

    // Parpadeo cada 500 ms
    if (currentMillis - lastBlink >= 500) {
      lastBlink = currentMillis;
      ledState = !ledState;
      digitalWrite(bateria, ledState ? HIGH : LOW);
    }
  }
  if (diagnosticando_airbags) {
    unsigned long currentMillis = millis();

    // Parpadeo cada 500 ms
    if (currentMillis - lastBlink >= 500) {
      lastBlink = currentMillis;
      ledState = !ledState;
      digitalWrite(airbags, ledState ? HIGH : LOW);
    }
  }
}
