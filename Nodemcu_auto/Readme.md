# ☁️ Estação Meteorológica IoT com NodeMCU e ThingSpeak

Este projeto é independente do restante do Hestia. Ele utiliza um microcontrolador NodeMCU (ESP8266) para coletar dados de três sensores ambientais — DHT11 (Temperatura e Umidade), BMP280 (Pressão e Temperatura) e LDR (Luminosidade) — e publica as leituras em duas frentes:
1. Uma plataforma IoT (ThingSpeak) para visualização e análise de dados históricos.
2. Um Servidor Web local na própria NodeMCU que expõe os dados mais recentes em formato JSON.

O intervalo de atualização dos dados é configurado para **10 minutos**.

## 🛠️ Materiais Necessários

* **Placa:** NodeMCU ESP8266 (ou similar)
* **Sensor 1:** DHT11 (Temperatura e Umidade)
* **Sensor 2:** BMP280 (Pressão e Temperatura Barométrica)
* **Sensor 3:** LDR (Resistor Dependente de Luz)
* **Componentes Adicionais:**
    * Resistor de 10kΩ (para o LDR)
    * Fios Jumper
    * Protoboard
    * Cabo Micro USB para alimentação e programação

## 📌 Pinagem e Circuito

A pinagem foi configurada para evitar conflitos, utilizando os pinos GPIO dedicados da NodeMCU.

| Sensor | Pino do Sensor | Pino NodeMCU (ESP8266) | GPIO Correspondente | Observações |
| :--- | :--- | :--- | :--- | :--- |
| **DHT11** | Data (Sinal) | **D4** | GPIO2 | Utiliza resistor *pull-up* (4.7kΩ ou 10kΩ) entre Data e 3.3V (VCC). |
| **BMP280** | SDA | **D2** | GPIO4 | Barramento I2C |
| **BMP280** | SCL | **D1** | GPIO5 | Barramento I2C |
| **LDR** | Sinal (Divisor) | **A0** | ADC | Pino Analógico. **Deve ser usado em um Divisor de Tensão.** |

### Montagem do LDR (Divisor de Tensão)

Para o LDR, monte um divisor de tensão da seguinte forma:
1.  Conecte o pino **3V3** da NodeMCU a um terminal do LDR.
2.  Conecte o outro terminal do LDR a um terminal de um resistor de **10kΩ**.
3.  Conecte o outro terminal do resistor ao pino **GND** da NodeMCU.
4.  Conecte o pino analógico **A0** da NodeMCU no ponto de junção entre o LDR e o resistor (onde você conecta A0 para ler a tensão).

## 💻 Configuração do Ambiente e Bibliotecas

Para compilar e rodar o projeto, você deve ter o suporte ao ESP8266 instalado no Arduino IDE e as seguintes bibliotecas:

1.  **ESP8266WiFi** (Padrão no suporte ESP8266)
2.  **ESP8266WebServer** (Padrão no suporte ESP8266)
3.  **DHT sensor library** (da Adafruit)
4.  **Adafruit Unified Sensor** (da Adafruit, dependência do DHT)
5.  **Adafruit_BMP280** (da Adafruit)
6.  **ArduinoJson** (Versão 6 ou superior, para a resposta JSON do WebServer)
7.  **ThingSpeak** (da MathWorks)

Você pode instalar a maioria delas através do **Gerenciador de Bibliotecas** do Arduino IDE.

## 🔑 Configuração do ThingSpeak

1.  Crie uma conta gratuita no [ThingSpeak](https://thingspeak.com/).
2.  Crie um novo **Channel**.
3.  Habilite 5 campos (Fields) com os seguintes nomes:
    * **Field 1:** `temperatura_dht`
    * **Field 2:** `umidade_dht`
    * **Field 3:** `luminosidade_ldr`
    * **Field 4:** `temperatura_bmp`
    * **Field 5:** `pressao_bmp`
4.  Na aba **API Keys** do seu canal, copie o seu **Channel ID** e o **Write API Key**.

## ⚙️ Configuração do Código (Secrets)

No início do arquivo `.ino`, substitua as constantes pelos seus valores:

```cpp
// --- Parâmetros de Configuração ---
const char* ssid = "Sua Rede Aqui";        // <-- SUBSTITUA
const char* password = "Sua Senha Aqui";    // <-- SUBSTITUA
unsigned long const CHANNEL_ID = XXXXXXXXXX;  // <-- SUBSTITUA
const char * const WRITE_API_KEY = "YYYYYYYYYYYYYYYY"; // <-- SUBSTITUA

// Intervalo de atualização (10 minutos)
const unsigned long UPDATE_INTERVAL_MS = 600000;