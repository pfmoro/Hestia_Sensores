#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <DHT.h>
#include <Adafruit_Sensor.h>
#include <ArduinoJson.h> // Biblioteca para manipular JSON

// Definições para o sensor DHT11
#define DHTPIN D2     // Pino onde o DHT11 está conectado (GPIO4)
#define DHTTYPE DHT11 // DHT 11
DHT dht(DHTPIN, DHTTYPE);

// Definição para o sensor LDR
#define LDR_PIN A0    // Pino onde o LDR está conectado (pino analógico)
/*
  Instruções para o circuito do LDR:
  O LDR precisa de um divisor de tensão para funcionar.
  Monte o circuito da seguinte forma:
  1. Conecte o pino 3V3 da NodeMCU a um terminal do LDR.
  2. Conecte o outro terminal do LDR a um terminal de um resistor de 10kOhm.
  3. Conecte o outro terminal do resistor ao pino GND da NodeMCU.
  4. Conecte o pino analógico A0 da NodeMCU no ponto de junção entre o LDR e o resistor.
*/

// --- Parâmetros de Configuração ---
// Substitua com as credenciais da sua rede Wi-Fi principal
const char* ssid = "NET_2G41D729_EXT";
const char* password = "PAgm8685";
const unsigned long UPDATE_INTERVAL_MS = 10000; // Intervalo de atualização em milissegundos (10 segundos)

// ----------------------------------

ESP8266WebServer server(80);

// Variáveis para armazenar os dados dos sensores
float temperatura;
float umidade;
int luminosidade;

// Função para ler os sensores
void lerSensores() {
  // Lendo o sensor DHT11
  umidade = dht.readHumidity();
  temperatura = dht.readTemperature();

  // Verificando se as leituras do DHT11 foram bem-sucedidas
  if (isnan(umidade) || isnan(temperatura)) {
    Serial.println("Falha na leitura do sensor DHT!");
    umidade = -1.0;
    temperatura = -1.0;
  }

  // Lendo o sensor LDR
  luminosidade = analogRead(LDR_PIN);
}

// Função que será chamada na URL raiz (/)
void handleRoot() {
  // Cria um documento JSON estático
  StaticJsonDocument<200> doc;
  doc["temperatura"] = temperatura;
  doc["umidade"] = umidade;
  doc["luminosidade"] = luminosidade;

  String jsonString;
  serializeJson(doc, jsonString);

  server.send(200, "application/json", jsonString);
}


void setup() {
  Serial.begin(115200);

  // Inicializando o sensor DHT11
  dht.begin();

  // Configurando a NodeMCU como Cliente e conectando-se à rede Wi-Fi
  Serial.print("Conectando a ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  // Aguarda a conexão com a rede Wi-Fi
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi conectado!");
  Serial.print("Endereço IP: ");
  Serial.println(WiFi.localIP());

  // Configurando o servidor web
  server.on("/", handleRoot);
  server.begin();
  Serial.println("HTTP server started");

  // Primeira leitura dos sensores para ter dados iniciais
  lerSensores();
}


void loop() {
  // O servidor precisa ser "cuidado" dentro do loop
  server.handleClient();

  // Atualizando os dados dos sensores a cada 10 segundos
  static unsigned long lastUpdate = 0;
  if (millis() - lastUpdate >= UPDATE_INTERVAL_MS) {
    lerSensores();
    Serial.print("Temperatura: ");
    Serial.print(temperatura);
    Serial.print(" °C, Umidade: ");
    Serial.print(umidade);
    Serial.print(" %, Luminosidade: ");
    Serial.println(luminosidade);
    lastUpdate = millis();
  }
}