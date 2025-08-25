#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <Wire.h>
#include <Adafruit_BMP280.h>

// ---- Configurações Globais ----
const char* ssid     = "SEU_SSID";
const char* password = "SUA_SENHA";
const uint8_t bmpAddress = 0x76;  // endereço I2C do BMP280 (0x76 ou 0x77)

// ---- Controle de energia dos sensores ruidosos ----
#define PWR_UMIDADE D6   // VCC do sensor de umidade
#define PWR_CHUVA   D7   // VCC do sensor de chuva

// ---- Pinos de dados ----
#define PINO_UMIDADE A0   // saída analógica do sensor de umidade
#define PINO_CHUVA   D5   // saída digital do sensor de chuva

Adafruit_BMP280 bmp;          // BMP280 via I2C
ESP8266WebServer server(80);

float temperatura_bmp = NAN;
float pressao_bmp = NAN;
int umidadeSolo = 0;
int chuva = 0;

void setup() {
  Serial.begin(115200);

  // Inicializa controle de energia
  pinMode(PWR_UMIDADE, OUTPUT);
  pinMode(PWR_CHUVA, OUTPUT);
  digitalWrite(PWR_UMIDADE, LOW); 
  digitalWrite(PWR_CHUVA, LOW);

  // Inicializa BMP280
  Wire.begin(D2, D1); // SDA, SCL
  Wire.setClock(100000);
  if (!bmp.begin(bmpAddress)) {
    Serial.println(F("BMP280 não encontrado!"));
    while (1);
  }

  // Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi conectado");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());

  // Rota que retorna JSON
  server.on("/", handleRoot);
  server.begin();
}

void loop() {
  server.handleClient();

  // 1) Ler BMP280 (sensores ruidosos desligados)
  temperatura_bmp = bmp.readTemperature();
  pressao_bmp = bmp.readPressure() / 100.0; // hPa

  // 2) Ler sensor de umidade
  digitalWrite(PWR_UMIDADE, HIGH);   // liga
  delay(150);                        // estabilizar
  umidadeSolo = analogRead(PINO_UMIDADE);
  digitalWrite(PWR_UMIDADE, LOW);    // desliga

  // 3) Ler sensor de chuva
  digitalWrite(PWR_CHUVA, HIGH);     // liga
  delay(150);                        // estabilizar
  chuva = digitalRead(PINO_CHUVA);
  digitalWrite(PWR_CHUVA, LOW);      // desliga

  delay(1000); // intervalo entre ciclos
}

void handleRoot() {
  String json = "{";
  json += "\"umidade_solo\":" + String(umidadeSolo) + ",";
  json += "\"estado_chuva\":" + String(chuva) + ",";
  json += "\"temperatura_bmp\":" + String(temperatura_bmp, 2) + ",";
  json += "\"pressao_bmp\":" + String(pressao_bmp, 4);
  json += "}";

  server.send(200, "application/json", json);
}

