#include <Wire.h>
#include <DHT.h>
#include <MQ135.h>

#define I2C_ADDRESS 0x08

// Sensores
#define PIN_LDR A0
#define PIN_MQ135 A1
#define PIN_CHUVA A2   // CHUVA ANALÓGICO
#define DHTPIN 2
#define DHTTYPE DHT11

MQ135 mq(PIN_MQ135);
DHT dht(DHTPIN, DHTTYPE);

int ldrValue, mqValue, rawChuva;
float temp, hum, ppm_corrigido;

void setup() {
  Serial.begin(9600);
  dht.begin();

  Wire.begin(I2C_ADDRESS);
  Wire.onRequest(requestEvent);

  Serial.println("=== ARDUINO SLAVE I2C INICIADO ===");
}

void loop() {
  // Leituras
  ldrValue = analogRead(PIN_LDR);
  mqValue = analogRead(PIN_MQ135);
  rawChuva = analogRead(PIN_CHUVA);

  hum = dht.readHumidity();
  temp = dht.readTemperature();

  ppm_corrigido = mq.getCorrectedPPM(temp, hum);

  Serial.println("\n[Arduino] Leituras:");
  Serial.print("  LDR: "); Serial.println(ldrValue);
  Serial.print("  MQ135: "); Serial.println(mqValue);
  Serial.print("  MQ135_PPM: "); Serial.println(ppm_corrigido);
  Serial.print("  Temp: "); Serial.println(temp);
  Serial.print("  Hum: "); Serial.println(hum);
  Serial.print("  Chuva RAW: "); Serial.println(rawChuva);

  delay(2000);
}

void requestEvent() {
  // Envia CSV: LDR,ppm_corrigido,temp,hum,chuva
  String payload =
      String(ldrValue) + "," +
      String(ppm_corrigido) + "," +
      String(temp) + "," +
      String(hum) + "," +
      String(rawChuva);

  Wire.write(payload.c_str());
}
