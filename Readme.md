# Héstia - Monitoramento Doméstico de Plantas e Ambientes

Este projeto de software e hardware é um sistema de monitoramento para plantas e ambientes. Utilizando duas placas **NodeMCU ESP8266**, ele coleta dados de diversos sensores e os envia para um backend centralizado em Python. Os dados são então visualizados em tempo real através de uma interface web com Streamlit e armazenados em serviços de nuvem como ThingSpeak e Google Drive.

## 📋 Lista de Materiais Necessários

Para montar este projeto, você precisará dos seguintes componentes:

  * **Placas:** 2x NodeMCU ESP8266
  * **Sensores:**
      * 1x Sensor de Temperatura e Umidade (DHT11)
      * 1x Sensor de Luminosidade (LDR)
      * 1x Sensor de Umidade do Solo
      * 1x Módulo Sensor de Chuva
  * **Outros Componentes:**
      * Fios jumpers
      * Protoboard (placa de ensaio)
      * Fonte de energia para a NodeMCU \#2 (bateria ou tomada)

## ⚙️ Explicação e Configuração do Arquivo `.env`

O arquivo `.env` é fundamental para o projeto, pois armazena todas as variáveis de configuração e chaves de API, mantendo-as fora do código-fonte e do repositório Git por questões de segurança. **Ele não será incluído no repositório**, portanto você deve criá-lo manualmente na raiz da pasta do projeto.

-----

**Passo 1:** Crie um arquivo chamado **`.env`** na raiz do projeto.
**Passo 2:** Preencha-o com as seguintes variáveis, substituindo os valores pelos seus.

### Variáveis e Possíveis Valores:

  * `TEMPO_ESPERA_SEGUNDOS`: (Número inteiro) Intervalo de tempo entre cada ciclo de coleta de dados.
  * `MAC_NODEMCU_1`: (String) Endereço MAC da NodeMCU \#1.
  * `EXPECTED_SCHEMA_NODE_1`: (String) Esquema JSON da NodeMCU \#2: "temperatura,umidade,luminosidade".
  * `THINGSPEAK_API_KEY`: (String) Sua chave de API do ThingSpeak para upload de dados.
  * `GOOGLE_DRIVE_API_KEY`: (String) Sua chave de API do Google Drive (opcional).

-----

**Configurações de Rede (Modo "Tomada" para NodeMCU \#2):**
Estas variáveis são usadas quando a NodeMCU \#2 está em modo de tomada.

  * `WIFI_SSID`: (String) Nome da sua rede Wi-Fi.
  * `WIFI_PASSWORD`: (String) Senha da sua rede Wi-Fi.
  * `NODEMCU2_POWER_MODE`: (String) Modo de operação da NodeMCU \#2. Valores possíveis: **`"battery"`** ou **`"plugged"`**.
  * `MAC_NODEMCU_2`: (String) Endereço MAC da NodeMCU \#2.
  * `EXPECTED_SCHEMA_NODE_2`: (String) Esquema JSON da NodeMCU \#2:"umidade_solo,estado_chuva".

-----

**Configurações do Sistema e de Salvamento:**

  * `IP_NODEMCU_2`: (String) Endereço IP da NodeMCU \#2 no modo de bateria. Valor fixo padrão: `"192.168.4.1"`.
  * `LOCAL_SAVE_PATH`: (String) Caminho para salvar os dados localmente. Use **`.`** para salvar no diretório atual, ou **`""`** para desativar.
  * `EXECUTION_ENVIRONMENT`: (String) Sistema operacional em que o backend está sendo executado. Valores possíveis: **`"windows"`**, **`"linux_pc"`** ou **`"android"`**.

## 🔋 Modo de Funcionamento das NodeMCUs

Este projeto oferece duas lógicas de operação para a NodeMCU \#2, que afetam diretamente o funcionamento do backend.

### Modo Bateria (`"battery"`)

Neste modo, a NodeMCU \#2 opera como um **Ponto de Acesso (AP)**. Ele é ideal para locais sem energia, pois a placa pode entrar em modo de baixo consumo e acordar periodicamente para expor os dados. O backend, neste caso, precisa se desconectar da rede principal e se conectar à rede da NodeMCU para coletar os dados, o que é um processo mais complexo, automatizado para Windows e Linux, mas manual para Android.

### Modo Tomada (`"plugged"`)

Neste modo, a NodeMCU \#2 funciona como um **Cliente Wi-Fi**, conectando-se à rede principal. É ideal para uso contínuo, pois a placa fica ligada e expõe os dados através de seu IP na rede. Isso simplifica o backend, que apenas precisa de uma simples requisição HTTP para coletar os dados, sem a necessidade de alternar de rede.

-----

## 🛡️ Proteção de Componentes em Ambientes Externos

É fundamental proteger as placas NodeMCU e seus sensores quando utilizados em ambientes externos. A exposição à umidade, poeira e variações de temperatura pode danificar os componentes eletrônicos.

**Recomendação:**
Utilize uma **caixa hermética de roteador** ou um invólucro à prova d'água para abrigar a NodeMCU \#1 e a NodeMCU \#2. Essas caixas são projetadas para proteger equipamentos de rede da chuva e poeira, e oferecem saídas para os cabos dos sensores. Garanta que a caixa tenha ventilação adequada para evitar o superaquecimento, mas sem comprometer a proteção contra a água.

-----

## 🔩 Montagem Física e Pinagem

Para a correta montagem do hardware, siga o esquema de pinagem abaixo. As definições dos pinos também estão presentes nos códigos `.ino` das respectivas NodeMCUs.

### NodeMCU \#1 (DHT11 e LDR)

  * **Sensor DHT11 (Temperatura e Umidade):**

      * **VCC** → Pino **3.3V** da NodeMCU
      * **GND** → Pino **GND** da NodeMCU
      * **Data** → Pino **D2** da NodeMCU

  * **Sensor LDR (Luminosidade):**

      * **VCC** → Pino **3.3V** da NodeMCU
      * **GND** → Resistor (10kΩ) → Pino **GND** da NodeMCU
      * **Data** → Conexão entre o LDR e o resistor → Pino **A0** da NodeMCU

### NodeMCU \#2 (Umidade do Solo e Chuva)

  * **Sensor de Umidade do Solo:**

      * **VCC** → Pino **3.3V** da NodeMCU
      * **GND** → Pino **GND** da NodeMCU
      * **Data** → Pino **A0** da NodeMCU

  * **Sensor de Chuva:**

      * **VCC** → Pino **3.3V** da NodeMCU
      * **GND** → Pino **GND** da NodeMCU
      * **Data (Digital)** → Pino **D5** da NodeMCU

-----

## 📱 Considerações para a Operação em Android

Operar um backend Python em um sistema Android apresenta desafios específicos devido às restrições do sistema operacional. Para executar o backend, a recomendação é usar o aplicativo **Pydroid 3**, que fornece um ambiente de terminal e um interpretador Python.

A principal implicação dessas restrições é a impossibilidade de o script Python controlar diretamente o hardware de rede do dispositivo. Por isso, quando a **NodeMCU \#2** está no **modo bateria**, o backend não consegue alternar a conexão Wi-Fi automaticamente para o ponto de acesso da NodeMCU. Nesses casos, o usuário deve fazer a mudança de rede manualmente, seguindo as instruções que o próprio script exibe no terminal.

Em ambientes como Windows ou Linux, os comandos de terminal (`netsh` ou `nmcli`) são usados para automatizar essa troca de rede, o que não é viável em uma arquitetura móvel.

-----

## 📂 Estrutura e Explicação dos Arquivos

O projeto é organizado da seguinte forma:

  * **`app/`**: Contém todos os scripts Python do backend e frontend.
      * `backend_central.py`: O script principal que orquestra a coleta, processamento e upload dos dados.
      * `communication_service.py`: Gerencia a comunicação HTTP com as NodeMCUs e a lógica de mudança de rede.
      * `ip_finder.py`: Responsável por descobrir o endereço IP das NodeMCUs na rede local, usando seus endereços MAC.
      * `upload_service.py`: Lida com o salvamento de dados localmente e o envio para serviços de nuvem como ThingSpeak.
      * `streamlit_app.py`: O frontend da aplicação, que lê o arquivo de dados local para visualização.
  * **`nodemcu_1/`**: Contém o código Arduino (`.ino`) para a NodeMCU \#1.
  * **`nodemcu_2/`**: Contém os códigos Arduino (`.ino`) para a NodeMCU \#2, com opções para o modo **`plugged`** e **`battery`**.

## 🛠️ Ferramentas e Tecnologias

  * **Arduino IDE**: Ambiente de desenvolvimento para programar as placas NodeMCU. É altamente recomendado para a manipulação e upload dos códigos.
  * **ThingSpeak**: Uma plataforma de IoT que permite visualizar e analisar dados de sensores em tempo real na nuvem.
  * **Streamlit**: Uma biblioteca Python de código aberto que facilita a criação de aplicativos web interativos e dashboards.

## 🚀 Instruções de Instalação e Execução

### Pré-requisitos

  * **Python 3.x** instalado.
  * **Git** instalado.
  * **Arduino IDE** instalado para programar as NodeMCUs.

<!-- end list -->

1.  **Clone o Repositório:**

    ```bash
    git clone https://github.com/SeuUsuario/SeuProjeto.git
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

      * Para a **NodeMCU \#1**, use o código em `nodemcu_1/`.
      * Para a **NodeMCU \#2**, use o código em `nodemcu_2/` que corresponde ao modo de energia escolhido.

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

-----

## 📚 Referências

  * [Python](https://www.python.org/)
  * [Arduino IDE](https://www.arduino.cc/en/software)
  * [NodeMCU](http://www.nodemcu.com/index_en.html)
  * [Sensor DHT11](https://www.google.com/search?q=https://docs.arduino.cc/tutorials/dht-sensor/dht-sensor-library/)
  * [Sensor de Chuva](https://www.google.com/search?q=https://www.circuits.dk/regnsensor-til-arduino/)
  * [Blog Eletrogate - Estação Meteorológica](https://blog.eletrogate.com/estacao-meteorologica-residencial-com-esp8266/)
  * [Streamlit](https://streamlit.io/)
  * [ThingSpeak](https://thingspeak.com/)
