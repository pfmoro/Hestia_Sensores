// Código Arduino para a NodeMCU #2 em modo de tomada


#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ArduinoJson.h>


// Substitua com as credenciais da sua rede Wi-Fi
const char* ssid = "NET_2G41D729_EXT";
const char* password = "PAgm8685";


// Pinos dos sensores
const int SOIL_MOISTURE_PIN = A0;
const int RAIN_SENSOR_PIN = D5;


ESP8266WebServer server(80);


void handleRoot() {
  // Leitura dos sensores
  int soilMoistureValue = analogRead(SOIL_MOISTURE_PIN);
  int rainSensorValue = digitalRead(RAIN_SENSOR_PIN); // 0 = chuva, 1 = sem chuva


  // Criação do objeto JSON com a biblioteca ArduinoJson
  StaticJsonDocument<100> doc;
  doc["umidade_solo"] = soilMoistureValue;
  doc["estado_chuva"] = (rainSensorValue == HIGH) ? 0 : 1; // 0 = sem chuva, 1 = chuva


  String jsonString;
  serializeJson(doc, jsonString);


  server.send(200, "application/json", jsonString);
}


void setup() {
  Serial.begin(115200);
  pinMode(RAIN_SENSOR_PIN, INPUT_PULLUP); // O sensor de chuva é ativo em LOW


  // Conecta ao Wi-Fi
  Serial.println();
  Serial.print("Conectando a ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);


  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }


  Serial.println("");
  Serial.println("WiFi conectado!");
  Serial.print("Endereço IP: ");
  Serial.println(WiFi.localIP());


  // Inicia o servidor web
  server.on("/", handleRoot);
  server.begin();
  Serial.println("Servidor HTTP iniciado.");
}


void loop() {
  server.handleClient();
}