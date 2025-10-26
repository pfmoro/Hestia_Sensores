#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h> // Adicionado para o WebServer
#include <Wire.h>
#include <DHT.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>
#include <ArduinoJson.h> // Adicionado para o JSON [cite: 1, 9]
#include "ThingSpeak.h"

// --- Parâmetros de Configuração ---
//const char* ssid = "Sua Rede Aqui";        // Nome da rede Wi-Fi [cite: 15]
//const char* password = "Sua Senha Aqui";    // Senha da rede Wi-Fi [cite: 16]
//unsigned long const CHANNEL_ID = 1111111111;  // Substitua XXXXXX pelo seu Channel ID
//const char * const WRITE_API_KEY = "YYYYYYYYYYYYYYYY"; // Substitua YYYYY pelo seu Write API Key

const char* ssid = "NET_2G41D729_EXT";        // Nome da rede Wi-Fi [cite: 15]
const char* password = "PAgm8685";    // Senha da rede Wi-Fi [cite: 16]
unsigned long const CHANNEL_ID = 3018477;  // Substitua XXXXXX pelo seu Channel ID
const char * const WRITE_API_KEY = "EZ37TSYU6DZIEQ60"; // Substitua YYYYY pelo seu Write API Key


// Intervalo de atualização (em milissegundos). 10 minutos = 600000 ms
const unsigned long UPDATE_INTERVAL_MS = 600000;

// Definições para o sensor DHT11
#define DHTPIN D4     // Pino onde o DHT11 está conectado (GPIO2)
#define DHTTYPE DHT11 // DHT 11
DHT dht(DHTPIN, DHTTYPE);

// Definição para o sensor LDR
#define LDR_PIN A0    // Pino onde o LDR está conectado (pino analógico) [cite: 10]

// Definições para o sensor BMP280
const uint8_t BMP_ADDRESS = 0x76; // Endereço I2C do BMP280 [cite: 2]
Adafruit_BMP280 bmp;

WiFiClient client;
ESP8266WebServer server(80); // Objeto WebServer [cite: 1, 9]

// Variáveis para armazenar os dados dos sensores
float dht_temp = -1.0;
float dht_umidade = -1.0;
int luminosidade = 0;
float bmp_temp = NAN;
float bmp_pressao = NAN;

// Função para ler os sensores
void lerSensores() {
  Serial.println("Lendo sensores...");
  // 1. Lendo o sensor DHT11
  dht_umidade = dht.readHumidity(); // [cite: 18]
  dht_temp = dht.readTemperature(); //[cite: 19]

  // Verificando se as leituras do DHT11 foram bem-sucedidas
  if (isnan(dht_umidade) || isnan(dht_temp)) {
    Serial.println("Falha na leitura do sensor DHT!");
    dht_umidade = -1.0; // Valor de erro [cite: 20]
    dht_temp = -1.0;    // Valor de erro [cite: 20]
  }

  // 2. Lendo o sensor LDR
  luminosidade = analogRead(LDR_PIN); //[cite: 21]

  // 3. Lendo o sensor BMP280
  // Leitura BMP280 // [cite: 3]
  bmp_temp = bmp.readTemperature(); //[cite: 3]
  bmp_pressao = bmp.readPressure() / 100.0; // Converte para hPa [cite: 4]
}

// Função que será chamada na URL raiz (/) - Resposta JSON
void handleRoot() {
  // Não precisa ler os sensores aqui, pois 'lerSensores' é chamado antes da atualização do ThingSpeak.
  // Apenas cria o JSON com os últimos valores lidos.
  
  StaticJsonDocument<200> doc;
  
  doc["temperatura_dht"] = dht_temp;
  doc["umidade_dht"] = dht_umidade;
  doc["luminosidade_ldr"] = luminosidade;
  doc["temperatura_bmp"] = bmp_temp; //[cite: 5]
  doc["pressao_bmp"] = bmp_pressao; //[cite: 5]

  String jsonString;
  serializeJson(doc, jsonString);

  server.send(200, "application/json", jsonString); //[cite: 23]
  Serial.println("JSON enviado via WebServer.");
}


void setup() {
  Serial.begin(115200); //[cite: 5, 24]
  delay(10);
  
  // 1. Inicializa sensores e I2C
  dht.begin(); //[cite: 24]
  Wire.begin(D2, D1); // Inicializa I2C (SDA=D2, SCL=D1) [cite: 6]

  // Tenta inicializar o BMP280
  if (!bmp.begin(BMP_ADDRESS)) {
    Serial.println(F("BMP280 não encontrado ou falha na inicialização!")); //[cite: 7]
    // O código não para, mas as leituras do BMP serão NAN.
  }

  // 2. Conexão Wi-Fi
  WiFi.mode(WIFI_STA);
  ThingSpeak.begin(client);
  
  Serial.print("Conectando a ");
  Serial.println(ssid);
  WiFi.begin(ssid, password); //[cite: 7, 24]

  // Aguarda a conexão [cite: 7, 25]
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print("."); //[cite: 8, 26]
  }

  Serial.println("\nWiFi conectado!");
  Serial.print("Endereço IP: ");
  Serial.println(WiFi.localIP());

  // 3. Configuração do Servidor Web
  server.on("/", handleRoot); //[cite: 27]
  server.begin(); //[cite: 27]
  Serial.println("HTTP server started");

  // Primeira leitura dos sensores para ter dados iniciais [cite: 28]
  lerSensores();
}


void loop() {
  // O servidor web deve ser "cuidado" dentro do loop [cite: 29]
  server.handleClient();

  static unsigned long lastUpdate = 0;

  // Verifica o intervalo de tempo para atualização [cite: 30]
  if (millis() - lastUpdate >= UPDATE_INTERVAL_MS) {
    
    // Conecta/Reconecta ao Wi-Fi se necessário
    if(WiFi.status() != WL_CONNECTED){
      Serial.print("Reconectando ao WiFi...");
      WiFi.begin(ssid, password);
      while(WiFi.status() != WL_CONNECTED){
        delay(500);
        Serial.print(".");
      }
      Serial.println(" Conectado!");
    }
    
    // Leitura dos sensores (Esta leitura será usada tanto para o ThingSpeak quanto para o JSON)
    lerSensores();
    
    Serial.print("DHT Temp: "); Serial.print(dht_temp);
    Serial.print(" °C, Umid: "); Serial.print(dht_umidade); //[cite: 31]
    Serial.print(" %, LDR: "); Serial.print(luminosidade); //[cite: 31]
    Serial.print(", BMP Temp: "); Serial.print(bmp_temp);
    Serial.print(" °C, Pressao: "); Serial.print(bmp_pressao);
    Serial.println(" hPa");

    // Envia dados para o ThingSpeak

    // Define os campos do ThingSpeak
    ThingSpeak.setField(1, dht_temp);      // Field 1: Temperatura DHT11
    ThingSpeak.setField(2, dht_umidade);   // Field 2: Umidade DHT11
    ThingSpeak.setField(3, luminosidade);  // Field 3: Luminosidade LDR
    //Fields 4 e 5 neste canal são os sensores de umidade de solo e de chuva,
    // não usados neste projeto
    ThingSpeak.setField(6, bmp_temp);      // Field 6: Temperatura BMP280
    ThingSpeak.setField(7, bmp_pressao);   // Field 7: Pressão BMP280

    // Escreve os campos no canal
    int statusCode = ThingSpeak.writeFields(CHANNEL_ID, WRITE_API_KEY);

    if (statusCode == 200) {
      Serial.println("Atualização ThingSpeak bem-sucedida!");
    } else {
      Serial.print("Falha na atualização ThingSpeak. Código de erro HTTP: ");
      Serial.println(statusCode);
    }
    
    lastUpdate = millis();
  }
}