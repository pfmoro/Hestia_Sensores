# 📘 **README — Sistema Arduino (Slave I2C) + NodeMCU (Master I2C) para Telemetria Ambiental**

## Resumo
Este sistema utiliza dois microcontroladores trabalhando de forma cooperativa:

1. **Arduino Uno/Nano** (Slave I2C) — Responsável por leituras analógicas precisas e cálculo de grandezas físicas.
2. **NodeMCU ESP8266** (Master I2C + Wi-Fi) — Responsável por receber os dados do Arduino, processá-los, disponibilizá-los via JSON HTTP e enviá-los ao ThingSpeak.


---

## 📡 1. Introdução ao Protocolo I2C

O **I²C (Inter-Integrated Circuit)** é um protocolo de comunicação serial síncrona usado para conectar microcontroladores e sensores usando apenas **duas linhas**:

| Linha   | Função                         |
| ------- | ------------------------------ |
| **SDA** | Dados (envio e recebimento)    |
| **SCL** | Clock (sincroniza transmissão) |

Características importantes:

* É um barramento **master → slave**, onde o master inicia todas as comunicações.
* Cada dispositivo no barramento tem um **endereço único** (7 bits).
* Permite conectar vários sensores/microcontroladores nos mesmos 2 fios.
* No seu projeto:

  * A **NodeMCU atua como MASTER**.
  * O **Arduino atua como SLAVE**, com endereço `0x08`.

---

## 🧰 2. Lista de Materiais

| Quantidade | Item                                              |
| ---------- | ------------------------------------------------- |
| 1          | Arduino (UNO/Nano/Pro Mini)                       |
| 1          | NodeMCU ESP8266                                   |
| 1          | Sensor MQ-135                                     |
| 1          | Sensor de luminosidade LDR GL5528 + resistor 10kΩ |
| 1          | Sensor DHT11 ou DHT22                             |
| 1          | Sensor de chuva analógico                         |
| 1          | Sensor capacitivo de umidade do solo              |
| —          | Jumpers masculino-masculino                       |
| —          | Protoboard                                        |
| —          | Fonte USB 5V                                      |

---

## 🔌 3. Tabela de Pinagem

### **Arduino (SLAVE)**

| Função                | Pino |
| --------------------- | ---- |
| LDR (GL5528)          | A0   |
| MQ135 (RAW analógico) | A1   |
| Sensor de chuva (RAW) | A2   |
| SDA (I2C)             | A4   |
| SCL (I2C)             | A5   |
| DHT                   | D2   |

---

### **NodeMCU ESP8266 (MASTER)**

| Função                         | Pino    |
| ------------------------------ | ------- |
| I2C SDA                        | D2      |
| I2C SCL                        | D1      |
| Sensor solo (capacitivo)       | A0      |
| Wi-Fi + Webserver + ThingSpeak | Interno |

---

### **Conexão I2C entre Arduino ↔ NodeMCU**

| Arduino  | NodeMCU |
| -------- | ------- |
| SDA (A4) | D2      |
| SCL (A5) | D1      |
| GND      | GND     |

---

# 🧩 4. Arquitetura do Sistema

O sistema possui **duas controladoras** cooperando:

---

## **🔹 Arduino — SLAVE I2C**

Responsável por **ler sensores analógicos** e enviar valores ao master em formato CSV.

### Sensores lidos no Arduino:

* LDR (valor bruto)
* Lux calculado (via curva do GL5528)
* MQ135 Raw (ADC)
* MQ135 PPM corrigido (temperatura + umidade)
* DHT (temperatura e umidade)
* Chuva analógica (0–1023)

O Arduino envia ao master uma string CSV:

```
LDRraw, LUX, MQraw, MQppm, Temp, Hum, ChuvaRaw
```

Exemplo real:

```
523, 180.2, 240, 41.5, 27.0, 62.0, 789
```

---

## **🔹 NodeMCU — MASTER I2C**

Tarefas:

1. Solicita o CSV ao Arduino via I2C.
2. Faz o parse e armazena cada variável.
3. Lê o sensor de solo capacitivo.
4. Converte umidade do solo para porcentagem.
5. Exibe tudo em JSON pelo servidor web interno (debug).
6. Envia ao ThingSpeak:

   | Campo  | Enviado      |
   | ------ | ------------ |
   | field1 | solo_perc    |
   | field2 | ar_lux       |
   | field3 | ar_mq_ppm    |
   | field4 | ar_temp      |
   | field5 | ar_hum       |
   | field6 | ar_chuva_raw |

---

# 🧮 5. Lógicas de Calibração e Equações

---

## **5.1 LDR GL5528 — Conversão para LUX**

A resistência do LDR segue uma curva não linear aproximada por:

### 📌 **Equação:**

```
lux = A * (R_ldr ^ B)
```

No código:

```
const float A = 600000;
const float B = -1.25;
```

A resistência é derivada do divisor resistivo:

```
R_LDR = R_SERIE * (1/Vout - 1)
Vout = leitura / 1023
```

---

## **5.2 MQ135 — Conversão para PPM corrigido**

Usando a biblioteca oficial:

```
ppmCorrigido = mq.getCorrectedPPM(temp, hum);
```

A biblioteca compensa variações de temperatura e umidade usando curvas empíricas do sensor.

---

## **5.3 Sensor capacitivo de umidade do solo**

Calibração baseada nas leituras fornecidas:

* 200 → seco
* 600 → imerso em água

### 📌 **Equação de porcentagem:**

```
percentual = (leitura - seco) * 100 / (molhado - seco)
```

---

## **5.4 Sensor de chuva — Lógica binária**

```
chuva_bin = (chuva_raw <= threshold) ? 1 : 0;
```

Com:

```
threshold = 500
```

---

# 🔄 6. Fluxo Operacional Completo

1. **NodeMCU inicia** e conecta ao Wi-Fi.
2. Inicia servidor HTTP e barramento I2C.
3. A cada 10s:

   * Solicita CSV ao Arduino.
   * Lê sensor de solo.
   * Converte solo → %
   * Atualiza JSON `/` para debug.
4. A cada 10 min:

   * Envia dados ao ThingSpeak.
5. Arduino repete:

   * Lê sensores
   * Calcula lux
   * Calcula ppm corrigido
   * Envia CSV quando solicitado

---

# 📄 7. Estrutura dos Arquivos

```
/Arduino_slave.ino        → Código completo do Arduino I2C Slave
/NodeMCU_master.ino       → Código completo do ESP8266 Master
/README.md                → Este documento
```

---

Se quiser, posso gerar:

✅ Um diagrama em blocos
✅ Um fluxograma do protocolo I2C entre os dois
✅ Um PDF formatado deste README
✅ Um esquema elétrico em Fritzing

Basta pedir!
