#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>


// --- Parâmetros de Configuração ---
const unsigned long TEMPO_SLEEP_MS = 300000; // Tempo em modo "deep sleep" (5 minutos = 300000 ms)
const unsigned long TEMPO_ATIVO_MS = 10000;  // Tempo que a NodeMCU permanece ativa (10 segundos)


const char* ssid = "NodeMCU_Sensores_2"; // Nome da rede Wi-Fi que a NodeMCU irá criar
const char* password = "sua_senha_aqui"; // Senha da rede


// Definição para o sensor de umidade do solo (analógico)
#define SOLO_PIN A0
/*
  Instruções para o circuito do Sensor de Umidade do Solo (analógico):
  1. Conecte o pino VCC do sensor ao pino 3V3 da NodeMCU.
  2. Conecte o pino GND do sensor ao pino GND da NodeMCU.
  3. Conecte o pino AO (Analog Out) do sensor ao pino A0 da NodeMCU.
*/


// Definição para o sensor de chuva (digital)
#define CHUVA_PIN D5 // Pino D5 da NodeMCU (GPIO14)
/*
  Instruções para o circuito do Sensor de Chuva (digital):
  1. Conecte o pino VCC do sensor ao pino 3V3 da NodeMCU.
  2. Conecte o pino GND do sensor ao pino GND da NodeMCU.
  3. Conecte o pino DO (Digital Out) do sensor ao pino D5 da NodeMCU.
  Este sensor já possui um módulo com comparador e regulador de tensão, não
  sendo necessário resistor externo para a leitura digital.
*/


// --- Variáveis de Leitura ---
int umidadeSolo;
int estadoChuva;


ESP8266WebServer server(80);


// Função para ler os sensores
void lerSensores() {
  // Lendo o sensor de umidade do solo (valor analógico 0-1023)
  umidadeSolo = analogRead(SOLO_PIN);
  
  // Lendo o sensor de chuva (HIGH=seco, LOW=chuva)
  estadoChuva = digitalRead(CHUVA_PIN);
}


// Função que será chamada na URL raiz (/)
void handleRoot() {
  String json = "{\"umidade_solo\": " + String(umidadeSolo) + ", \"estado_chuva\": " + String(estadoChuva) + "}";
  server.send(200, "application/json", json);
}


void setup() {
  Serial.begin(115200);
  Serial.println("\nNodeMCU acordou!");
  
  // AVISO IMPORTANTE: Para que a NodeMCU acorde do deep sleep,
  // você deve conectar fisicamente o pino D0 ao pino RST.
  
  // Configura o pino do sensor de chuva como entrada
  pinMode(CHUVA_PIN, INPUT);


  // Lê os sensores e armazena os dados
  lerSensores();


  // Configura a NodeMCU como Access Point
  Serial.print("Configurando AP...");
  WiFi.softAP(ssid, password);
  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);


  // Configurando o servidor web
  server.on("/", handleRoot);
  server.begin();
  Serial.println("Servidor HTTP iniciado.");


  // Imprimindo as leituras na serial para debug
  Serial.print("Umidade do solo (0-1023): ");
  Serial.println(umidadeSolo);
  Serial.print("Estado de chuva (0=chuva, 1=seco): ");
  Serial.println(estadoChuva);
  
  // A NodeMCU permanecerá ativa por TEMPO_ATIVO_MS para que o backend possa se conectar.
  Serial.print("Aguardando conexao do backend por ");
  Serial.print(TEMPO_ATIVO_MS / 1000);
  Serial.println(" segundos.");
  delay(TEMPO_ATIVO_MS); 


  // Entra em deep sleep pelo tempo configurado
  Serial.print("Entrando em deep sleep por ");
  Serial.print(TEMPO_SLEEP_MS / 1000);
  Serial.println(" segundos.");
  ESP.deepSleep(TEMPO_SLEEP_MS * 1000); // deepSleep espera microssegundos
}


void loop() {
  // O loop fica vazio, pois a NodeMCU reinicia após o deep sleep
}