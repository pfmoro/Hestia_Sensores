#include <Wire.h>
#include <DHT.h>
#include <MQ135.h>

#define I2C_ADDRESS 0x08
#define PIN_LDR A0
#define PIN_MQ135 A1
#define DHTPIN 2
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);
MQ135 mq(PIN_MQ135);

char payload[64]; // Aumentado para segurança

float ldrLux(int leituraADC) {
    const float ADC_MAX = 1023.0; 
    const float R_SERIE = 10000.0; 
    const float CONST_A = 600000.0; 
    const float CONST_B = -1.25; 
    if (leituraADC <= 0) leituraADC = 1; 
    float v_out = (float)leituraADC / ADC_MAX; 
    float r_ldr = R_SERIE * (1.0 / v_out - 1.0); 
    return (CONST_A * pow(r_ldr, CONST_B)); 
}

void setup() {
  Serial.begin(9600);
  dht.begin(); 
  Wire.begin(I2C_ADDRESS); 
  Wire.onRequest(requestEvent); 
  Serial.println("=== ARDUINO SLAVE PRONTO ===");
}

void loop() {
  int ldrRaw = analogRead(PIN_LDR); 
  float lux = ldrLux(ldrRaw); 
  int mqRaw = analogRead(PIN_MQ135); 
  float temp = dht.readTemperature(); 
  float hum = dht.readHumidity(); 
  float ppm = mq.getCorrectedPPM(temp, hum); 

  // Criando a string CSV manualmente para evitar o erro do snprintf com floats
  String csv = String(ldrRaw) + "," + 
               String(lux, 1) + "," + 
               String(mqRaw) + "," + 
               String(ppm, 1) + "," + 
               String(temp, 1) + "," + 
               String(hum, 1);

  // Copia para o buffer de char que o I2C usa
  csv.toCharArray(payload, 64);

  // Debug apenas no loop principal
  Serial.print("Payload Atualizado: ");
  Serial.println(payload); 

  delay(2000);
}

void requestEvent() {
  // Apenas envia o dado, sem Serial.print aqui dentro!
  Wire.write(payload); 
}