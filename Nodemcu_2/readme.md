# NodeMCU #2: Modos Tomada/Bateria (DEPRECATED) ⚡

Esta NodeMCU foi a placa anterior que consolidava os sensores ruidosos e o BMP280. Embora **DEPRECATED** na nova arquitetura modular, seus códigos para os modos Tomada e Bateria são mantidos aqui para eventual reuso. O Backend é "Backward compatible" e ainda aceitaria este código caso se deseje utilizar

## 🔩 Pinagem

| Componente | Pino da NodeMCU | Observação |
| :--- | :--- | :--- |
| **Sensor de Umidade do Solo (Data)** | **A0** | Leitura analógica de Umidade. |
| **Sensor de Chuva (Data)** | **D5** (GPIO14) | Leitura digital. |
| **BMP280 (SDA)** | **D2** (GPIO4) | I2C Data. |
| **BMP280 (SCL)** | **D1** (GPIO5) | I2C Clock. |

## 🔌 Modos de Operação

Esta pasta contém os códigos para os dois modos:

1.  **`Nodemcu_2_modotomada/`**: A placa atua como **Cliente Wi-Fi** e Servidor HTTP. O backend a coleta por IP fixo ou descoberto.
2.  **`Nodemcu_2_modobateria/`**: A placa atua como **Access Point (AP)** e acorda periodicamente. O backend precisa se conectar diretamente à sua rede para coletar os dados.

## 💡 Esquema JSON Esperado (DEPRECATED)

O esquema esperado era a junção de todos os dados: `"umidade_solo,estado_chuva,temperatura_bmp,pressao_bmp"`

## Configuração .env:
Caso opte por usar esta placa, as seguintes variáveis devem ser preenchidas no .env:

NODEMCU2_POWER_MODE=`"plugged"` #"battery", se o modo bateria for usado
MAC_NODEMCU_2="" $preencher endereço MAC
EXPECTED_SCHEMA_NODE_2= "umidade_solo,estado_chuva,temperatura_bmp,pressao_bmp"