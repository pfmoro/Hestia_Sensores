#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <Wire.h>
#include <ThingSpeak.h>

// ======================================================================
// CONFIGURAÇÕES GERAIS
// ======================================================================

// Intervalos
unsigned long intervalo_sensores_ms = 10000;   // 10s
unsigned long intervalo_thingspeak_ms = 600000; // 10min
unsigned long ultimo_sensores = 0;
unsigned long ultimo_thingspeak = 0;

// WiFi
const char* ssid = "SEU_WIFI";
const char* password = "SENHA";

// ThingSpeak
WiFiClient client;
unsigned long myChannelNumber = 123456;  // SEU CANAL
const char* myWriteAPIKey = "SUA_API_KEY";

// Entrada Node
#define PIN_UMIDADE A0

ESP8266WebServer server(80);

// Dados recebidos do Arduino (slave I2C)
int ar_ldr_raw = 0;        // leitura ADC bruta do LDR
float ar_lux = 0.0;        // lux (já calculado no Arduino)
int ar_mq_raw = 0;         // leitura bruta MQ135 (ADC)
float ar_mq_ppm = 0.0;     // ppm corrigido (não será exposto no JSON por enquanto)
float ar_temp = 0.0;       // temperatura vindo do Arduino
float ar_hum = 0.0;        // umidade vindo do Arduino
int ar_chuva_raw = 0;      // leitura analógica do sensor de chuva

int chuva_threshold = 500;
int chuva_bin = 0;

int solo = 0;
int solo_perc = 0;

// ======================================================================
// Parse CSV Arduino
// Formato esperado (7 campos):
// LDRraw,LUX,MQraw,MQppm,Temp,Hum,ChuvaRaw
// ======================================================================
void parseArduinoData(String csv) {
  Serial.print("[NODE] CSV recebido: ");
  Serial.println(csv);

  // Encontra posições das vírgulas (esperamos 6 vírgulas -> 7 campos)
  int i1 = csv.indexOf(',');
  int i2 = csv.indexOf(',', i1 + 1);
  int i3 = csv.indexOf(',', i2 + 1);
  int i4 = csv.indexOf(',', i3 + 1);
  int i5 = csv.indexOf(',', i4 + 1);
  int i6 = csv.indexOf(',', i5 + 1);

  if (i1 < 0 || i2 < 0 || i3 < 0 || i4 < 0 || i5 < 0) {
    Serial.println("[ERRO] CSV inválido! campos insuficientes.");
    return;
  }

  // Se não encontrou a 6ª vírgula, ainda podemos obter o último campo pegando substring(a partir de i5+1)
  // Mapear campos:
  // 0: 0..i1-1            -> LDRraw
  // 1: i1+1..i2-1         -> LUX
  // 2: i2+1..i3-1         -> MQraw
  // 3: i3+1..i4-1         -> MQppm
  // 4: i4+1..i5-1         -> Temp
  // 5: i5+1..i6-1 (ou to i6-1) -> Hum
  // 6: i6+1..end          -> ChuvaRaw (se i6 >=0) else i5+1..end

  // Ajuste caso a 6ª vírgula não exista (compatibilidade com strings menores)
  String s0 = csv.substring(0, i1);
  String s1 = csv.substring(i1 + 1, i2);
  String s2 = csv.substring(i2 + 1, i3);
  String s3 = csv.substring(i3 + 1, i4);
  String s4 = csv.substring(i4 + 1, i5);

  String s5;
  String s6;

  if (i6 >= 0) {
    s5 = csv.substring(i5 + 1, i6);
    s6 = csv.substring(i6 + 1);
  } else {
    // caso a 6ª vírgula não exista, tentamos recuperar hum/chuva a partir do que sobrar:
    // assumimos: s5 = csv.substring(i5+1, i5+1 + até meio) ... mas como o formato esperado tem 7 campos,
    // prefira sempre enviar corretamente do Arduino. Aqui fazemos fallback simples:
    int remainingStart = i5 + 1;
    // tenta separar pelo último ',' encontrado anteriormente (i5) — neste fallback colocamos todo resto em s5
    s5 = csv.substring(remainingStart);
    s6 = "0";
  }

  // Conversões (com robustez)
  ar_ldr_raw = s0.toInt();
  ar_lux = s1.toFloat();
  ar_mq_raw = s2.toInt();
  ar_mq_ppm = s3.toFloat();
  ar_temp = s4.toFloat();
  ar_hum = s5.toFloat();
  ar_chuva_raw = s6.toInt();

  Serial.println("[NODE] Valores do Arduino (após parse):");
  Serial.printf("   LDR RAW : %d\n", ar_ldr_raw);
  Serial.printf("   LUX     : %.2f\n", ar_lux);
  Serial.printf("   MQ RAW  : %d\n", ar_mq_raw);
  Serial.printf("   MQ PPM  : %.2f\n", ar_mq_ppm);
  Serial.printf("   Temp    : %.1f\n", ar_temp);
  Serial.printf("   Hum     : %.1f\n", ar_hum);
  Serial.printf("   Chuva RAW: %d\n", ar_chuva_raw);

  chuva_bin = (ar_chuva_raw <= chuva_threshold) ? 1 : 0;

  Serial.printf("[NODE] Chuva BIN (1=molhado / 0=seco): %d\n", chuva_bin);
}

// ======================================================================
// HTTP: retorna JSON de debug
// ======================================================================
void handleData() {
  String json = "{";

  json += "\"solo\":" + String(solo) + ",";
  json += "\"solo_perc\":" + String(solo_perc) + ",";
  json += "\"ldr_raw\":" + String(ar_ldr_raw) + ",";
  json += "\"lux\":" + String(ar_lux) + ",";
  // Não incluir MQ ppm no JSON de debug conforme solicitado (se quiser remover/adiicionar, edite aqui)
  json += "\"mq_raw\":" + String(ar_mq_raw) + ",";
  json += "\"temp_arduino\":" + String(ar_temp) + ",";
  json += "\"hum_arduino\":" + String(ar_hum) + ",";
  json += "\"chuva_raw\":" + String(ar_chuva_raw) + ",";
  json += "\"chuva_bin\":" + String(chuva_bin);

  json += "}";

  server.send(200, "application/json", json);

  Serial.println("[HTTP] JSON enviado.");
}

// ======================================================================
// Map da leitura do sensor de umidade para porcentagem
// (função já existente, mantida)
// ======================================================================
int umidadePercentual(int leitura) {
  const int seco = 200;     // leitura no seco
  const int molhado = 700;  // leitura totalmente na água

  // Garante que a leitura fique dentro do intervalo
  if (leitura < seco) leitura = seco;
  if (leitura > molhado) leitura = molhado;

  // Converte para porcentagem (0% = seco, 100% = molhado)
  float perc = (float)(leitura - seco) * 100.0 / (molhado - seco);

  return (int)perc;
}

// ======================================================================
// SETUP
// ======================================================================
void setup() {
  Serial.begin(9600);
  Serial.println("\n=== NodeMCU MASTER + ThingSpeak.h ===");

  // Wire: pinos SDA=SDA_PIN (D2), SCL=SCL_PIN (D1) conforme seu código anterior
  Wire.begin(D2, D1);

  WiFi.begin(ssid, password);
  Serial.print("[WiFi] Conectando");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(200);
  }
  Serial.print("\n[WiFi] IP: ");
  Serial.println(WiFi.localIP());

  server.on("/", handleData);
  server.begin();
  Serial.println("[HTTP] WebServer iniciado.");

  ThingSpeak.begin(client);
}

// ======================================================================
// LOOP
// ======================================================================
void loop() {
  server.handleClient();
  unsigned long agora = millis();

  // ------------------------------------------------------------------
  // LEITURA SENSORES A CADA 10s (solicita o CSV do Arduino)
  // ------------------------------------------------------------------
  if (agora - ultimo_sensores >= intervalo_sensores_ms) {
    ultimo_sensores = agora;

    Serial.println("\n[NODE] Atualizando sensores (I2C) ...");

    Wire.requestFrom(0x08, 64); // pede até 64 bytes (ajuste se necessário)
    String csv = "";

    unsigned long start = millis();
    // espera por dados por curto período
    while (Wire.available()) {
      char c = (char)Wire.read();
      csv += c;
      // proteção contra loops infinitos
      if (millis() - start > 200) break;
    }

    if (csv.length() > 0){
       parseArduinoData(csv);
    }
    else{
      Serial.printf("csv.length zero \n");
    }
    solo = analogRead(PIN_UMIDADE);
    solo_perc = umidadePercentual(solo);
    Serial.printf("[NODE] Solo ADC: %d\n", solo);
    Serial.printf("[NODE] Solo_perc: %d\n", solo_perc);
    
  }

  // ------------------------------------------------------------------
  // ENVIO AO THINGSPEAK A CADA 10min (apenas solo_perc e lux)
  // ------------------------------------------------------------------
  if (agora - ultimo_thingspeak >= intervalo_thingspeak_ms) {
    ultimo_thingspeak = agora;

    Serial.println("\n[TS] Enviando dados via ThingSpeak.h ...");

    // Envia Campos ao thingspeak
    ThingSpeak.setField(1, solo_perc);      // % solo
    ThingSpeak.setField(2, ar_lux);         // LUX vindo do Arduino
    ThingSpeak.setField(3, ar_mq_ppm);      // MQ-135 PPM corrigido
    ThingSpeak.setField(4, ar_chuva_raw);   // chuva RAW
    ThingSpeak.setField(5, ar_temp);        // temperatura Arduino
    ThingSpeak.setField(6, ar_hum);         // umidade Arduino


    int httpCode = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);

    if (httpCode == 200) {
      Serial.println("[TS] Atualização OK (200)");
    } else {
      Serial.print("[TS] Falha na atualização. Código: ");
      Serial.println(httpCode);
    }
  }
}
