# Héstia - Backend Central (App) 🧠

Este diretório contém todo o software Python que orquestra o sistema Héstia, atuando como o cérebro central. Ele coleta dados de todas as NodeMCUs (via Wi-Fi/HTTP), processa, armazena e os exibe.

## ⚙️ Explicação e Configuração do Arquivo `.env`

O arquivo `.env` é crucial para o funcionamento do backend. Crie-o na raiz do diretório do projeto e preencha com as configurações das Nodes.

### Variáveis e Esquemas (Schema)

| Variável | Descrição |
| :--- | :--- |
| `TEMPO_ESPERA_SEGUNDOS` | Intervalo de tempo entre cada ciclo de coleta de dados. |
| `THINGSPEAK_API_KEY` | Sua chave de API do ThingSpeak para upload de dados. |
| `GOOGLE_DRIVE_API_KEY` | Sua chave de API do Google Drive (opcional). |
| `LOCAL_SAVE_PATH` | Caminho para salvar os dados localmente (ex: `.` para o diretório atual). |
| `EXECUTION_ENVIRONMENT` | Sistema operacional do backend: `"windows"`, `"linux_pc"` ou `"android"`. |

#### Configurações das NodeMCUs (X = 1, 3, 4, 5)

| Variável | Descrição |
| :--- | :--- |
| `MAC_NODEMCU_X` | Endereço MAC da NodeMCU X. Usado para descoberta de IP. |
| `EXPECTED_SCHEMA_NODE_X` | Esquema JSON esperado da NodeMCU X. Ex: `"temperatura,umidade"`. |
| `IP_NODEMCU_X` | Endereço IP fixo da NodeMCU X (Opcional). Deixe **em branco** para usar a lógica de busca do `ip_finder.py`. |

> ⚠️ **Lógica de 'Pular Node':** Se o valor de `EXPECTED_SCHEMA_NODE_X` for deixado **em branco**, o backend **ignora** completamente a coleta de dados daquela NodeMCU.

## 📂 Arquivos do Backend

| Arquivo | Função |
| :--- | :--- |
| `backend_central.py` | Orquestrador principal. Gerencia o loop de coleta, valida as NodeMCUs ativas e aciona os serviços. |
| `communication_service.py` | Lida com todas as requisições HTTP (`requests.get`) para as NodeMCUs. **Controla o *timeout* de conexão (atualmente 30s).** |
| `ip_finder.py` | Contém a lógica de busca de IP por MAC (tabela ARP e força bruta) e persistência de IPs. |
| `upload_service.py` | Envia dados para ThingSpeak, Google Drive e salva localmente. |
| `streamlit_app.py` | O frontend da aplicação (Dashboard de visualização). |

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
