#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <Wire.h>
#include <ThingSpeak.h>

// ======================================================================
// CONFIGURAÇÕES DE REDE E THINGSPEAK
// ======================================================================
const char* ssid     = "NOME_DO_SEU_WIFI";
const char* password = "SUA_SENHA_AQUI";


unsigned long myChannelNumber = "ID_Canal";     // Seu ID do Canal ThingSpeak
const char* myWriteAPIKey = "Sua_chave_aqui";  // Sua Write API Key

// Pinos Locais
#define PIN_SOLO A0
#define PIN_CHUVA_DIGITAL D3

// Intervalos
unsigned long intervalo_sensores = 10000;   // 10 segundos para debug/serial
unsigned long intervalo_thingspeak = 600000; // 1 minuto para envio (TS exige pausa)
unsigned long ultimo_sensores = 0;
unsigned long ultimo_thingspeak = 0;

// Variáveis de Dados
float ar_temp, ar_hum, ar_lux, ar_mq_ppm;
int solo_perc, chuva_detectada;

WiFiClient client;
ESP8266WebServer server(80);

// ======================================================================
// FUNÇÕES AUXILIARES
// ======================================================================

void parseArduinoData(String csv) {
  // O formato esperado do Arduino é: LDRraw,Lux,MQraw,PPM,Temp,Hum
  // Exemplo: "450,120.5,30,12.2,25.4,60.0"
  
  int i1 = csv.indexOf(',');
  int i2 = csv.indexOf(',', i1 + 1);
  int i3 = csv.indexOf(',', i2 + 1);
  int i4 = csv.indexOf(',', i3 + 1);
  int i5 = csv.indexOf(',', i4 + 1);

  if (i1 != -1 && i5 != -1) {
    ar_lux    = csv.substring(i1 + 1, i2).toFloat();
    ar_mq_ppm = csv.substring(i3 + 1, i4).toFloat();
    ar_temp   = csv.substring(i4 + 1, i5).toFloat();
    ar_hum    = csv.substring(i5 + 1).toFloat();
  }
}

void handleRoot() {
  String json = "{";
  json += "\"umidade_solo_perc\":" + String(solo_perc) + ",";
  json += "\"chuva\":" + String(chuva_detectada == 1 ? "true" : "false") + ",";
  json += "\"temperatura\":" + String(ar_temp) + ",";
  json += "\"umidade_ar\":" + String(ar_hum) + ",";
  json += "\"lux\":" + String(ar_lux) + ",";
  json += "\"ppm_gas\":" + String(ar_mq_ppm);
  json += "}";
  server.send(200, "application/json", json);
}

// ======================================================================
// SETUP
// ======================================================================
void setup() {
  Serial.begin(115200);
  
  pinMode(PIN_CHUVA_DIGITAL, INPUT);
  
  // Inicia I2C nos pinos D2 (SDA) e D1 (SCL)
  Wire.begin(D2, D1); 
  
  // Conexão WiFi
  WiFi.begin(ssid, password);
  Serial.print("Conectando WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi Conectado!");
  Serial.print("IP: "); Serial.println(WiFi.localIP());

  ThingSpeak.begin(client);
  
  server.on("/", handleRoot);
  server.begin();
}

// ======================================================================
// LOOP
// ======================================================================
void loop() {
  server.handleClient();
  unsigned long agora = millis();

  // 1. LEITURA E PROCESSAMENTO (A cada 10s)
  if (agora - ultimo_sensores >= intervalo_sensores) {
    ultimo_sensores = agora;

    // --- Leituras Locais (NodeMCU) ---
    int solo_raw = analogRead(PIN_SOLO);
    // Calibração: 1024 (seco) a 200 (muito molhado)
    solo_perc = map(solo_raw, 680, 250, 0, 100);
    if (solo_perc > 100) solo_perc = 100;
    if (solo_perc < 0) solo_perc = 0;

    // Sensor de chuva digital (Geralmente LOW quando detecta água)
    chuva_detectada = (digitalRead(PIN_CHUVA_DIGITAL) == LOW) ? 1 : 0;

    // --- Leitura I2C (Arduino Slave 0x08) ---
    Wire.requestFrom(0x08, 64);
    String csv = "";
    while (Wire.available()) {
      char c = Wire.read();
      if (c == '\0') break; // Fim da string
      csv += c;
    }

    if (csv.length() > 0) {
      parseArduinoData(csv);
      Serial.println("\n--- Dados Recebidos do Arduino ---");
      Serial.println("CSV: " + csv);
    }

    // Debug Serial local
    Serial.printf("Solo: %d%% | Solo Raw: %d| Chuva: %s | Temp: %.1fC | Lux: %.1f\n", 
                  solo_perc, solo_raw,(chuva_detectada ? "SIM" : "NAO"), ar_temp, ar_lux);
  }

  // 2. ENVIO THINGSPEAK (A cada 60s)
  if (agora - ultimo_thingspeak >= intervalo_thingspeak) {
    ultimo_thingspeak = agora;
    // Campos NodeMCU
    ThingSpeak.setField(5, solo_perc);
    ThingSpeak.setField(4, (float)chuva_detectada);

    //Campso Arduino 
    ThingSpeak.setField(9, ar_lux);
    ThingSpeak.setField(9, ar_mq_ppm);
    ThingSpeak.setField(9, ar_temp);
    ThingSpeak.setField(9, ar_hum);

    int status = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);

    if (status == 200) {
      Serial.println(">>> ThingSpeak atualizado com sucesso!");
    } else {
      Serial.println(">>> Erro no ThingSpeak: " + String(status));
    }
  }
}