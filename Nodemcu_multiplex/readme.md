
# 💨 Nodemcu Multiplex - Monitor  de Qualidade Ambiental 

Este projeto consiste na agregação de vários sensores analógicos em uma única aplicação de API local para monitoramento. Uma vez que a Nodemcu só dispõe de uma única entrada analógica, de API local o um multiplexador é usado para gerenciar as leituras de forma automática.

## 🛠️ Capacidades Atuais do Projeto

| Categoria | Recurso | Descrição Detalhada |
| :--- | :--- | :--- |
| **Microcontrolador** | NodeMCU ESP8266-12E | Placa principal com conectividade Wi-Fi integrada. |
| **Aquisição de Dados** | Multiplexador **CD74HC4067** (16 canais) | Expande a única porta analógica ($\text{A0}$) da NodeMCU para ler até 16 sensores. |
| **Conectividade** | Wi-Fi (Modo Station) | Conexão à rede local com impressão do IP de acesso no Serial Monitor. |
| **API WebServer** | Endpoint **JSON** na rota raiz (`/`) | Expõe os dados de todos os sensores em um formato JSON amigável para APIs e Dashboards. |
| **Sensores Monitorados** | 4 Sensores Analógicos | **Gases Tóxicos** ($\text{C0}$), **Umidade Ambiente** ($\text{C1}$), **Luminosidade LDR** ($\text{C2}$), **Umidade do Solo** ($\text{C3}$). |
| **Frequência** | Leitura e *Print* Serial a cada **5 segundos** | Garante que os dados do WebServer e do Serial Monitor sejam atualizados frequentemente. |
| **Envio IoT** | Configuração ThingSpeak | O código possui a função de envio para o ThingSpeak configurada, mas está **comentada**, com um intervalo de 10 minutos ($\text{600.000 ms}$). |

### 🔍 Estrutura do Endpoint JSON

A API retorna os dados brutos (0-1023) com os seguintes nomes de campos (de-para):

| Sensor | Canal | Nome do Campo JSON | Exemplo de Leitura (Bruto) |
| :--- | :--- | :--- | :--- |
| Umidade do Solo Capacitivo | $\text{C3}$ | `umidade_solo_1` | 550 |
| Umidade Ambiente / Chuva | $\text{C1}$ | `estado_chuva` | 610 |
| Gases Tóxicos / Qualidade do Ar | $\text{C0}$ | `qualidade_ar_gases` | 420 |
| Luminosidade (LDR) | $\text{C2}$ | `luminosidade_local` | 850 |

**Exemplo de resposta da API (`http://<IP_DO_NODEMCU>/`)**

```json
{"umidade_solo_1": 550, "estado_chuva": 610, "qualidade_ar_gases": 420, "luminosidade_local": 850}
```

### ⚙️ Configuração de Hardware

O multiplexador **CD74HC4067** é o núcleo de leitura analógica do projeto.

| Pino do CD74HC4067 | Conexão na NodeMCU | Função |
| :--- | :--- | :--- |
| $\text{SIG}$ (Comum) | $\text{A0}$ | Entrada analógica principal |
| $\text{S0}$ | $\text{D5}$ (GPIO14) | Bit de seleção 0 |
| $\text{S1}$ | $\text{D6}$ (GPIO12) | Bit de seleção 1 |
| $\text{S2}$ | $\text{D7}$ (GPIO13) | Bit de seleção 2 |
| $\text{S3}$ | $\text{D8}$ (GPIO15) | Bit de seleção 3 |
| $\text{C0}$ a $\text{C3}$ | Saídas Analógicas dos Sensores | Entradas do Multiplexador |
| $\text{EN}$ (Enable) | $\text{GND}$ | Mantém o multiplexador sempre ativo |
| $\text{VCC}$ | $\text{3.3V}$ | Alimentação |

-----

## 🚀 Sugestões de Evolução Futura

### 1\. Calibração e Conversão dos Dados

Para que o projeto forneça dados significativos, é crucial converter os valores brutos ($\text{0-1023}$) em unidades físicas:

  * **Gases (PPM):** Implementar a lógica de calibração específica do sensor $\text{MQ-x}$ (MQ-135, MQ-2, etc.), que geralmente envolve a conversão logarítmica ou o uso da função `map()` em faixas de concentração (PPM - Partes Por Milhão).
  * **Umidade do Solo (%):** Calibrar o sensor capacitivo, definindo os valores brutos mínimo (solo seco ao ar) e máximo (solo saturado) para mapeá-los para uma escala de $\text{0\%}$ a $\text{100\%}$.

### 2\. Expansão de Sensores

O multiplexador tem 12 canais livres ($\text{C4}$ a $\text{C15}$). Sugere-se:

  * **Multiplicação da Umidade de Solo:** Adicionar mais sensores de solo em canais sequenciais ($\text{C4}, \text{C5}$, etc.) para monitorar múltiplas plantas.
  * **Sensores Digitais/I2C:** Embora o multiplexador seja analógico, a NodeMCU pode acomodar facilmente sensores digitais como $\text{DHT11/22}$ (Temperatura/Umidade Digital) ou $\text{BMP180}$ (Pressão/Temperatura) em pinos digitais separados.

### 3\. Envio IoT

  * **ThingSpeak:** Esta node possui capacidade de envio de forma automática de dados para o thingspeak, para isso, é necessário inserir sua chave de API e id de canal