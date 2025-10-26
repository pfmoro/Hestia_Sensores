
---

# Héstia - Monitoramento Doméstico de Plantas e Ambientes

Este projeto de software e hardware é um sistema de monitoramento para plantas e ambientes. Utilizando **quatro placas NodeMCU ESP8266** (NodeMCU #1, #3, #4 e #5), ele coleta dados de diversos sensores e os envia para um backend centralizado em Python. Os dados são então visualizados em tempo real através de uma interface web com Streamlit e armazenados em serviços de nuvem como ThingSpeak e Google Drive.

 Uma das nodes, Nodemcu_auto, não é vinculada ao restante do projeto e pode ser utilizada de forma totalmente autonoma para coleta de dados ambientais e upload no thingspeak, no entanto, ela não é capaz de fazer a leitura da totalidade dos sensores e nem armazenar dados localmente ou no google drive. Por ser apartada, possui uma documentação própria, podendo ser consultada na pasta respectiva.

## 📋 Lista de Materiais Necessários 

Para montar este projeto, você precisará dos seguintes componentes:

* **Placas:** 4x NodeMCU ESP8266
* **Sensores:**
    * 1x Sensor de Temperatura e Umidade (DHT11)
    * 1x Sensor de Luminosidade (LDR)
    * 1x Sensor de Umidade do Solo **Capacitivo** (Substitui o resistivo)
    * 1x Módulo Sensor de Chuva
    * 1x Sensor de Pressão e Temperatura (BMP280)
    * **1x Sensor de Qualidade do Ar (MQ-135)**
* **Outros Componentes:**
    * Fios jumpers
    * Protoboard (placa de ensaio)
    * Resistor de 10kΩ (para o LDR)
    * Fontes de energia (para as 4 NodeMCUs no modo Tomada)

## ⚙️ Explicação e Configuração do Arquivo `.env` (Atualizado)

O arquivo `.env` foi expandido para suportar a nova arquitetura com 4 NodeMCUs. **Ele não será incluído no repositório**, portanto você deve criá-lo manualmente na raiz da pasta do projeto.

### Variáveis e Possíveis Valores:

* `TEMPO_ESPERA_SEGUNDOS`: (Número inteiro) Intervalo de tempo entre cada ciclo de coleta de dados.
* 'timeout': (Número inteiro) Intervalo de tempo para timeout das conexões requests
* `THINGSPEAK_API_KEY`: (String) Sua chave de API do ThingSpeak para upload de dados.
* `GOOGLE_DRIVE_API_KEY`: (String) Sua chave de API do Google Drive (opcional).

#### Configurações da NodeMCU #1 (DHT11/LDR)
* `MAC_NODEMCU_1`: (String) Endereço MAC da NodeMCU #1.
* `EXPECTED_SCHEMA_NODE_1`: (String) Esquema JSON esperado: `"temperatura,umidade,luminosidade"`.
* `IP_NODEMCU_1`: (String) Endereço IP fixo ou em branco (para usar a busca).

# Configuração para a NodeMCU #2 (deprecated; BMP280 + ruidosos)
NODEMCU2_POWER_MODE="plugged" #(Pode ser "plugged" ou "battery")
MAC_NODEMCU_2=""
EXPECTED_SCHEMA_NODE_2= "" #Esperado "umidade_solo,estado_chuva,temperatura_bmp,pressao_bmp"

#### Configurações da NodeMCU #3 (Ruidosos: Chuva/Solo)
* `MAC_NODEMCU_3`: (String) Endereço MAC da NodeMCU #3.
* `EXPECTED_SCHEMA_NODE_3`: (String) Esquema JSON esperado: `"umidade_solo,estado_chuva"`.
* `IP_NODEMCU_3`: (String) Endereço IP fixo ou em branco (para usar a busca).

#### Configurações da NodeMCU #4 (BMP280)
* `MAC_NODEMCU_4`: (String) Endereço MAC da NodeMCU #4.
* `EXPECTED_SCHEMA_NODE_4`: (String) Esquema JSON esperado: `"temperatura_bmp,pressao_bmp"`.
* `IP_NODEMCU_4`: (String) Endereço IP fixo ou em branco (para usar a busca).

#### Configurações da NodeMCU #5 (MQ-135)
* `MAC_NODEMCU_5`: (String) Endereço MAC da NodeMCU #5.
* `EXPECTED_SCHEMA_NODE_5`: (String) Esquema JSON esperado: `"qualidade_ar_raw"`.
* `IP_NODEMCU_5`: (String) Endereço IP fixo ou em branco (para usar a busca).

#### Configurações de Sistema
* `LOCAL_SAVE_PATH`: (String) Caminho para salvar os dados localmente. Use **`.`** para salvar no diretório atual.
* `EXECUTION_ENVIRONMENT`: (String) Sistema operacional em que o backend está sendo executado. Valores possíveis: `"windows"`, `"linux_pc"` ou `"android"`.

> ⚠️ **Lógica de 'Pular Node':** Se o valor de `EXPECTED_SCHEMA_NODE_X` for deixado **em branco** no `.env`, o `backend_central.py` ignorará e pulará completamente a coleta de dados daquela NodeMCU, facilitando a desativação temporária de sensores.

## 🔋 Modo de Funcionamento das NodeMCUs (Simplificado)

Na arquitetura atual, todas as NodeMCUs estão configuradas para operar no **Modo Tomada (`"plugged"`)** como **Clientes Wi-Fi**.

### Modo Tomada (`"plugged"`)

Todas as NodeMCUs funcionam como **Clientes Wi-Fi**, conectando-se à rede principal. Elas ficam ligadas e expõem os dados através de seus IPs na rede, simplificando o backend, que apenas precisa de requisições HTTP para coletar os dados.

Modo Bateria ("battery")
Apenas a NodeMCU #2 possui este modo. NEste caso, ela opera como um Ponto de Acesso (AP). Ele é ideal para locais sem energia, pois a placa pode entrar em modo de baixo consumo e acordar periodicamente para expor os dados. O backend, neste caso, precisa se desconectar da rede principal e se conectar à rede da NodeMCU para coletar os dados, o que é um processo mais complexo, automatizado para Windows e Linux, mas manual para Android.

## 🔩 Montagem Física e Pinagem (Atualizada)

A arquitetura foi dividida para isolar sensores sensíveis e ruidosos.

### NodeMCU #1 (DHT11 e LDR)

* **Sensor DHT11 (Temperatura e Umidade):**
    * **VCC** → Pino **3.3V** da NodeMCU
    * **GND** → Pino **GND** da NodeMCU
    * **Data** → Pino **D2** da NodeMCU
* **Sensor LDR (Luminosidade):**
    * **VCC** → Pino **3.3V** da NodeMCU
    * **GND** → Resistor (10kΩ) → Pino **GND** da NodeMCU
    * **Data** → Conexão entre o LDR e o resistor → Pino **A0** da NodeMCU

### NodeMCU #2 (Umidade do Solo, Chuva e BMP280)
* **Sensor de Umidade do Solo:**
  * **VCC** → Pino **3.3V** da NodeMCU
  * **GND** → Pino **GND** da NodeMCU
  * **Data** → Pino **A0** da NodeMCU

* **Sensor de Chuva:**
  * **VCC** → Pino **3.3V** da NodeMCU
  * **GND** → Pino **GND** da NodeMCU
  * **Data** (Digital) → Pino **D5** da NodeMCU

* **Sensor BMP280 (Pressão e Temperatura):**
  * **VCC** → Pino **3.3V** da NodeMCU
  * **GND** → Pino **GND** da NodeMCU
  * **SDA** → Pino **D2** da NodeMCU
  * **SCL** → Pino **D1** da NodeMCU

### NodeMCU #3 (Sensores Ruidosos: Umidade do Solo e Chuva)

Esta NodeMCU utiliza controle de energia (`D6` e `D7`) e um tempo de espera aumentado (`500ms`) para estabilizar a leitura do sensor capacitivo, evitando os saltos observados em ambientes secos.

* **Sensor de Umidade do Solo (Capacitivo):**
    * **VCC** → Pino **D6** (GPIO12) da NodeMCU (Controlado via software)
    * **GND** → Pino **GND** da NodeMCU
    * **Data** → Pino **A0** da NodeMCU
* **Sensor de Chuva:**
    * **VCC** → Pino **D7** (GPIO13) da NodeMCU (Controlado via software)
    * **GND** → Pino **GND** da NodeMCU
    * **Data (Digital)** → Pino **D5** (GPIO14) da NodeMCU

### NodeMCU #4 (BMP280: Pressão e Temperatura)

Esta NodeMCU é dedicada ao sensor I2C, garantindo um ambiente elétrico limpo.

* **Sensor BMP280 (Pressão e Temperatura):**
    * **VCC** → Pino **3.3V** da NodeMCU (Fixo)
    * **GND** → Pino **GND** da NodeMCU
    * **SDA** → Pino **D2** (GPIO4) da NodeMCU
    * **SCL** → Pino **D1** (GPIO5) da NodeMCU

### NodeMCU #5 (MQ-135: Qualidade do Ar)

O MQ-135 é conectado diretamente, pois precisa de aquecimento contínuo (burn-in) para operar corretamente.

* **Sensor MQ-135 (Qualidade do Ar):**
    * **VCC** → Pino **3.3V** da NodeMCU (Fixo)
    * **GND** → Pino **GND** da NodeMCU
    * **Data (Analógica)** → Pino **A0** da NodeMCU

---

## 🛡️ Proteção de Componentes em Ambientes Externos

É fundamental proteger as placas NodeMCU e seus sensores quando utilizados em ambientes externos. A exposição à umidade, poeira e variações de temperatura pode danificar os componentes eletrônicos.

**Recomendação:**
Utilize uma **caixa hermética de roteador** ou um invólucro à prova d'água para abrigar a NodeMCU #1 e a NodeMCU #2. Essas caixas são projetadas para proteger equipamentos de rede da chuva e poeira, e oferecem saídas para os cabos dos sensores. Garanta que a caixa tenha ventilação adequada para evitar o superaquecimento, mas sem comprometer a proteção contra a água.

## 📂 Estrutura e Explicação dos Arquivos (Atualizada)

O projeto agora inclui diretórios para as novas NodeMCUs:

* **`app/`**: Contém todos os scripts Python do backend e frontend.
    * `backend_central.py`: O script principal que orquestra a coleta, processamento e upload dos dados. **Agora suporta múltiplas NodeMCUs dinamicamente e a lógica de pular leituras.**
    * `communication_service.py`: Gerencia a comunicação HTTP com as NodeMCUs.
    * `ip_finder.py`: Responsável por descobrir o endereço IP das NodeMCUs na rede local, usando seus endereços MAC.
    * `upload_service.py`: Lida com o salvamento de dados localmente e o envio para serviços de nuvem como ThingSpeak.
    * `streamlit_app.py`: O frontend da aplicação.
    * **`utils.py`**: Funções auxiliares.
* **`nodemcu_1/`**: Código Arduino (`.ino`) para a NodeMCU #1.
* **`nodemcu_2/`**: Código Arduino (`.ino`) para a NodeMCU #2 (**DEPRECATED**).
* **`nodemcu_3/`**: **Novo!** Código Arduino (`.ino`) para a NodeMCU #3 (Ruidosos).
* **`nodemcu_4/`**: **Novo!** Código Arduino (`.ino`) para a NodeMCU #4 (BMP280).
* **`nodemcu_5/`**: **Novo!** Código Arduino (`.ino`) para a NodeMCU #5 (MQ-135).

## 🚀 Instruções de Instalação e Execução

*As instruções de pré-requisitos, clonagem, ambiente virtual e execução permanecem as mesmas.*

4.  **Faça o Upload do Código nas NodeMCUs:**
    * Para a **NodeMCU #1**, use o código em `nodemcu_1/`.
    * Para a **NodeMCU #2**, **DEPRECATED** caso deseje usa-la o código está em `nodemcu_2/`.
    * Para a **NodeMCU #3**, use o código em `nodemcu_3/`.
    * Para a **NodeMCU #4**, use o código em `nodemcu_4/`.
    * Para a **NodeMCU #5**, use o código em `nodemcu_5/`.

---
### 📝 Dicas de Conectividade do Sensor BMP280

O BMP280 opera usando o protocolo I2C, que é muito eficiente, mas sensível à fiação. Siga estas dicas para garantir uma conexão confiável:

* **Endereço I2C:** O endereço padrão do BMP280 é **`0x76`** ou **`0x77`**. O código fornecido usa a biblioteca da Adafruit, que geralmente encontra o endereço automaticamente, mas se houver problemas, pode ser necessário especificar o endereço na inicialização.
* **Aterramento (GND):** Certifique-se de que o pino `GND` do sensor esteja firmemente conectado ao `GND` da NodeMCU. Uma conexão ruim é uma causa comum de falha na leitura.
* **Pinos I2C:** A NodeMCU possui pinos específicos para I2C (`D1` para SCL e `D2` para SDA). Embora o ESP8266 possa usar software I2C em outros pinos, usar os pinos dedicados (`D1` e `D2`) é a prática recomendada para estabilidade.
* **Soldagem dos Pinos:** Para garantir melhor conectividade, os pinos do BMP280 devem ser soldados ao sensor, conforme guia da [Adafruit](https://cdn-learn.adafruit.com/downloads/pdf/adafruit-bmp280-barometric-pressure-plus-temperature-sensor-breakout.pdf)
---
 ## 📱 Considerações para a Operação em Android

Operar um backend Python em um sistema Android apresenta desafios específicos devido às restrições do sistema operacional. Para executar o backend, a recomendação é usar o aplicativo **Pydroid 3**, que fornece um ambiente de terminal e um interpretador Python.

A principal implicação dessas restrições é a impossibilidade de o script Python controlar diretamente o hardware de rede do dispositivo. Por isso, quando a **NodeMCU #2** está no **modo bateria**, o backend não consegue alternar a conexão Wi-Fi automaticamente para o ponto de acesso da NodeMCU. Nesses casos, o usuário deve fazer a mudança de rede manualmente, seguindo as instruções que o próprio script exibe no terminal.

Em ambientes como Windows ou Linux, os comandos de terminal (`netsh` ou `nmcli`) são usados para automatizar essa troca de rede, o que não é viável em uma arquitetura móvel.

-----

## 🛠️ Ferramentas e Tecnologias

  * **Arduino IDE**: Ambiente de desenvolvimento para programar as placas NodeMCU. É altamente recomendado para a manipulação e upload dos códigos.
  * **ThingSpeak**: Uma plataforma de IoT que permite visualizar e analisar dados de sensores em tempo real na nuvem.
  * **Streamlit**: Uma biblioteca Python de código aberto que facilita a criação de aplicativos web interativos e dashboards.

## 🚀 Instruções de Instalação e Execução

### Pré-requisitos

  * **Python 3.x** instalado.
  * **Git** instalado.
  * **Arduino IDE** instalado para programar as NodeMCUs.

1.  **Clone o Repositório:**
    ```bash
    git clone [https://github.com/SeuUsuario/SeuProjeto.git](https://github.com/SeuUsuario/SeuProjeto.git)
    cd SeuProjeto
    ```
    (Substitua a URL pelo endereço do seu repositório)

2.  **Crie o Ambiente Virtual e Instale as Dependências:**
      * **Windows:**
        ```bash
        setup.bat
        ```
      * **Linux/Android (Pydroid):**
        ```bash
        chmod +x setup.sh
        ./setup.sh
        ```

3.  **Configure o `.env`:**
    Crie o arquivo `.env` na raiz do projeto e preencha-o com suas configurações, conforme explicado acima.

4.  **Faça o Upload do Código nas NodeMCUs:**
      * Para a **NodeMCU #1**, use o código em `nodemcu_1/`.
      * Para a **NodeMCU #2**, use o código em `nodemcu_2/` que corresponde ao modo de energia escolhido.
      * Para a **NodeMCU #3**, use o código em `nodemcu_3/`.
      * Para a **NodeMCU #4**, use o código em `nodemcu_4/`.
      * Para a **NodeMCU #5**, use o código em `nodemcu_5/`.

5.  **Execute a Aplicação:**
      * **Windows:**
        ```bash
        start_all.bat
        ```
      * **Linux/Android (Pydroid):**
        ```bash
        chmod +x start_all.sh
        ./start_all.sh
        ```
    A aplicação Streamlit será aberta no seu navegador, enquanto o backend continuará rodando em segundo plano.