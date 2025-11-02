#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WiFiClient.h>

// Tempo de espera entre as leituras principais (5 segundos)
const int INTERVALO_LEITURA_MS = 5000;

// ---------------------- CONFIGURAÇÕES DE REDE -----------------------
const char* ssid = "NOME_DA_SUA_REDE";
const char* password = "SUA_SENHA_WIFI";
// O WebServer roda na porta 80 (padrão HTTP)
ESP8266WebServer server(80);

// ---------------------- CONFIGURAÇÕES THINGSPEAK (COMENTADAS) -------------------

#include "ThingSpeak.h"
unsigned long const THINGSPEAK_INTERVAL = 600000; // 10 minutos (600000 ms)
unsigned long myChannelNumber = XXXXXXX;        // Substitua pelo número do seu canal
const char * myWriteAPIKey = "SEU_WRITE_API_KEY"; // Substitua pela sua chave
WiFiClient client;

// --------------------------------------------------------------------------------

// Definição dos pinos de controle (endereço) do Multiplexador CD74HC4067
const int S0 = D5; // D5 (GPIO14) - Bit 0
const int S1 = D6; // D6 (GPIO12) - Bit 1
const int S2 = D7; // D7 (GPIO13) - Bit 2
const int S3 = D8; // D8 (GPIO15) - Bit 3

const int pinosSelecao[] = {S0, S1, S2, S3};
const int NUM_PINOS_SELECAO = 4;
const int pinoAnalogicoNodeMCU = A0; 

// --------------------- VARIÁVEIS DE LEITURA E NOMES DE DEPARA ------------------
// Definição dos canais e variáveis para armazenar os valores brutos.
const int CANAL_GASES = 0;
const int CANAL_UMIDADE = 1;
const int CANAL_LDR = 2;
const int CANAL_UMIDADE_SOLO = 3;

// Variáveis globais para guardar as leituras mais recentes
int valorGases;
int valorUmidade;
int valorLDR;
int valorUmidadeSolo;

// --------------------- FUNÇÕES DE LEITURA ----------------------

// Função que configura os pinos S0-S3 para selecionar o canal desejado e lê o valor.
int lerCanalMultiplexador(int canal) {
  for (int i = 0; i < NUM_PINOS_SELECAO; i++) {
    digitalWrite(pinosSelecao[i], bitRead(canal, i));
  }
  
  delayMicroseconds(5); 
  return analogRead(pinoAnalogicoNodeMCU);
}

// Função para ler todos os sensores e atualizar as variáveis globais
void lerTodosSensores() {
  valorGases = lerCanalMultiplexador(CANAL_GASES);
  valorUmidade = lerCanalMultiplexador(CANAL_UMIDADE);
  valorLDR = lerCanalMultiplexador(CANAL_LDR);
  valorUmidadeSolo = lerCanalMultiplexador(CANAL_UMIDADE_SOLO);
}

// --------------------- FUNÇÕES DE MANUSEIO DO WEBSERVER (JSON) -----------------------

// Endpoint: /
// Expõe os dados no formato JSON com os nomes de campo solicitados
void handleJson() {
  String json = "{";
  json += "\"umidade_solo_1\": " + String(valorUmidadeSolo) + ","; // Solo
  json += "\"estado_chuva\": " + String(valorUmidade) + ",";       // Umidade
  json += "\"qualidade_ar_gases\": " + String(valorGases) + ",";   // Gases
  json += "\"luminosidade_local\": " + String(valorLDR);           // LDR
  json += "}";
  
  // Envia a resposta como application/json
  server.send(200, "application/json", json);
}

// --------------------- FUNÇÃO THINGSPEAK (COMENTADA) -----------------------


void sendDataToThingSpeak() {
   //Configuração para enviar os 4 valores para 4 campos diferentes no ThingSpeak
  ThingSpeak.setField(8, valorGases);
  ThingSpeak.setField(5, valorUmidade);
  // ThingSpeak.setField(3, valorLDR); //manter comentado ou desativar da outra nodemcu
  ThingSpeak.setField(4, valorUmidadeSolo);

  int code = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);

  if(code == 200){
    Serial.println(">> ThingSpeak: Sucesso!");
  } else {
    Serial.print(">> ThingSpeak: Erro de envio. Código: ");
    Serial.println(code);
  }
}


// ------------------------------- SETUP ---------------------------------

void setup() {
  Serial.begin(115200);
  delay(100);
  
  // 1. Configuração do Multiplexador
  for (int i = 0; i < NUM_PINOS_SELECAO; i++) {
    pinMode(pinosSelecao[i], OUTPUT);
  }
  pinMode(pinoAnalogicoNodeMCU, INPUT);
  
  // 2. Conexão WiFi
  Serial.print("Conectando-se a ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  // Imprime o IP RECEBIDO
  Serial.println("\n-----------------------------------------");
  Serial.println("WiFi Conectado!");
  Serial.print("IP do WebServer (Endpoint JSON): ");
  Serial.println(WiFi.localIP());
  Serial.println("-----------------------------------------");

  // 3. Configuração do WebServer
  // O endpoint JSON é definido para a rota raiz "/"
  server.on("/", handleJson); 
  server.begin();
  Serial.println("WebServer iniciado.");

  // 4. Configuração ThingSpeak (Comentada)
  // ThingSpeak.begin(client);
  
  // Realiza a primeira leitura
  lerTodosSensores();
}

// -------------------------------- LOOP ---------------------------------

void loop() {
  // Variável estática para controlar o tempo do loop principal
  static unsigned long lastReadingTime = 0;
  
  // Trata as requisições do WebServer
  server.handleClient(); 
  
  // Verifica se o intervalo de 5 segundos (5000 ms) passou
  if (millis() - lastReadingTime >= INTERVALO_LEITURA_MS) {
    
    // 1. Leitura dos sensores
    lerTodosSensores();
    
    // 2. Envio para o Serial Monitor (para debug)
    Serial.print(millis() / 1000);
    Serial.print("s | Gases: "); Serial.print(valorGases);
    Serial.print(" | Umidade: "); Serial.print(valorUmidade);
    Serial.print(" | LDR: "); Serial.print(valorLDR);
    Serial.print(" | Solo: "); Serial.println(valorUmidadeSolo);

    // 3. Envio de dados para o ThingSpeak (Comentado)
    /*
    static unsigned long lastThingSpeakTime = 0;
    if (millis() - lastThingSpeakTime >= THINGSPEAK_INTERVAL) {
      sendDataToThingSpeak();
      lastThingSpeakTime = millis();
    }
    */
    
    // Atualiza o tempo da última leitura
    lastReadingTime = millis();
  }
}