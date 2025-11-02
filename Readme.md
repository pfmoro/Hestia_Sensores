# Héstia - Mapeamento e Documentação dos Sensores

Este projeto de software e hardware é um sistema de monitoramento para plantas e ambientes. Este documento serve como um guia rápido para todos os sensores utilizados no projeto Héstia, indicando qual NodeMCU é responsável por sua leitura, a finalidade de cada placa e onde o firmware pode ser encontrado na estrutura de pastas.

## 🛡️ Proteção de Componentes em Ambientes Externos

Parte dos sensores é pensada em se manter em ambientes externos. Portanto, é fundamental proteger as placas NodeMCU e seus sensores quando utilizados nestas condições. A exposição à umidade, poeira e variações de temperatura pode danificar os componentes eletrônicos.

**Recomendação:**
Utilize uma **caixa hermética de roteador** ou um invólucro à prova d'água para abrigar as NodeMCUs colocadas em ambientes externos . Essas caixas são projetadas para proteger equipamentos de rede da chuva e poeira, e oferecem saídas para os cabos dos sensores. Garanta que a caixa tenha ventilação adequada para evitar o superaquecimento, mas sem comprometer a proteção contra a água.


## 🗺️ Mapeamento de Sensores por NodeMCU

A arquitetura do projeto Héstia é modular, com cada NodeMCU dedicada a um conjunto específico de sensores para otimizar a estabilidade, especialmente isolando sensores de comunicação serial (`I2C`) e sensores ruidosos (`Umidade/Chuva`).

| Sensor | Medição | NodeMCU Responsável | Tipo de Comunicação | Diretório do Firmware |
| :--- | :--- | :--- | :--- | :--- |
| **DHT11** | Temperatura/Umidade | **#1** | Digital | `Nodemcu_1/` |
| **LDR** | Luminosidade | **#1** | Analógica | `Nodemcu_1/` |
| **Umidade Solo (Capacitivo)** | Umidade do Solo | **#3** | Analógica (com controle de energia) | `Nodemcu_3/` |
| **Módulo Chuva** | Presença de Chuva | **#3** | Digital (com controle de energia) | `Nodemcu_3/` |
| **BMP280** | Pressão/Temperatura | **#4** | I2C | `Nodemcu_4/` |
| **MQ-135** | Qualidade do Ar (Gases) | **#5** | Analógica | `Nodemcu_5/` |

---

## 🛠️ Detalhe dos Sensores e Configuração

### 1. Sensores Primários (NodeMCU #1)

#### 🌡️ DHT11 (Temperatura e Umidade)
* **Função:** Fornece as leituras básicas do ambiente.
* **Pinagem Chave:** Data conectado ao pino **D2** da NodeMCU.
* **Código:** `Nodemcu_1/nodemcu_1.ino`

#### ☀️ LDR (Fotoresistor - Luminosidade)
* **Função:** Mede a intensidade da luz ambiente.
* **Pinagem Chave:** Leitura analógica no pino **A0** da NodeMCU. Requer um divisor de tensão (resistor de 10kΩ).
* **Código:** `Nodemcu_1/nodemcu_1.ino`

### 2. Sensores Ruidosos (NodeMCU #3)

Esta placa implementa um *delay* de 500ms e controle de energia (`D6` e `D7`) para garantir estabilidade.

#### 💧 Sensor de Umidade do Solo (Capacitivo)
* **Função:** Mede o teor de umidade do solo. Escolhido o modelo capacitivo por ser mais durável que o resistivo.
* **Pinagem Chave:** Leitura em **A0**. Controle de VCC em **D6** (GPIO12).
* **Código:** `Nodemcu_3/nodemcu_3.ino`

#### 🌧️ Módulo Sensor de Chuva
* **Função:** Detecta a presença de água em sua superfície (chuva).
* **Pinagem Chave:** Leitura em **D5** (GPIO14). Controle de VCC em **D7** (GPIO13).
* **Código:** `Nodemcu_3/nodemcu_3.ino`

### 3. Sensor I2C (NodeMCU #4)

#### 🏔️ BMP280 (Pressão Barométrica e Temperatura)
* **Função:** Mede a pressão atmosférica e a temperatura de alta precisão, essencial para a calibração de altitude ou para monitorar frentes frias/quentes.
* **Pinagem Chave:** I2C nos pinos **D1 (SCL)** e **D2 (SDA)**.
* **Código:** `Nodemcu_4/nodemcu_4.ino`

### 4. Sensor de Gases (NodeMCU #5)

#### 💨 MQ-135 (Qualidade do Ar)
* **Função:** Mede a concentração de vários gases (NH3, NOx, álcool, CO2, etc.), fornecendo um indicador bruto da qualidade do ar.
* **Pinagem Chave:** Leitura analógica em **A0**. Requer alimentação constante (VCC 3.3V fixo) para o período de **aquecimento (burn-in)** inicial.
* **Código:** `Nodemcu_5/nodemcu_5.ino`

### 5. NodeMCU Auxiliar

#### 🧭 NodeMCU_auto
* **Função:** Esta é uma NodeMCU **autônoma**, separada do backend central (`App/`). Ela possui sua própria documentação e firmware para coletar dados ambientais e fazer o upload diretamente para o ThingSpeak sem a intervenção do servidor Python.
* **Diretório:** `Nodemcu_auto/`

---

## 💻 Backend (App)

O backend Python, localizado na pasta `App/`, utiliza a informação do esquema JSON (`EXPECTED_SCHEMA_NODE_X` no `.env`) para saber quais sensores esperar de cada NodeMCU. O módulo `communication_service.py` é responsável por orquestrar a coleta via HTTP.