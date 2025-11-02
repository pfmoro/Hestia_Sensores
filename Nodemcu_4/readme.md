# NodeMCU #4: BMP280 (Pressão e Temperatura) 🌡️

Esta placa é dedicada ao sensor I2C (BMP280) para garantir a leitura mais precisa possível, longe de sensores ruidosos.

## 🔩 Pinagem

| Componente | Pino da NodeMCU | Observação |
| :--- | :--- | :--- |
| **BMP280 (SDA)** | **D2** (GPIO4) | I2C Data. |
| **BMP280 (SCL)** | **D1** (GPIO5) | I2C Clock. |
| **BMP280 (VCC)** | **3.3V** | Alimentação constante. |
| **BMP280 (GND)** | **GND** | Aterramento. |

## 💡 Esquema JSON Esperado (No Backend)

O backend espera este esquema para a NodeMCU #4:

```json
{
    "temperatura_bmp": 25.12,
    "pressao_bmp": 1013.25
}

EXPECTED_SCHEMA_NODE_4 no .env deve ser: "temperatura_bmp,pressao_bmp"

### 📝 Dicas de Conectividade do Sensor BMP280

O BMP280 opera usando o protocolo I2C, que é muito eficiente, mas sensível à fiação. Siga estas dicas para garantir uma conexão confiável:

* **Endereço I2C:** O endereço padrão do BMP280 é **`0x76`** ou **`0x77`**. O código fornecido usa a biblioteca da Adafruit, que geralmente encontra o endereço automaticamente, mas se houver problemas, pode ser necessário especificar o endereço na inicialização.
* **Aterramento (GND):** Certifique-se de que o pino `GND` do sensor esteja firmemente conectado ao `GND` da NodeMCU. Uma conexão ruim é uma causa comum de falha na leitura.
* **Pinos I2C:** A NodeMCU possui pinos específicos para I2C (`D1` para SCL e `D2` para SDA). Embora o ESP8266 possa usar software I2C em outros pinos, usar os pinos dedicados (`D1` e `D2`) é a prática recomendada para estabilidade.
* **Soldagem dos Pinos:** Para garantir melhor conectividade, os pinos do BMP280 devem ser soldados ao sensor, conforme guia da [Adafruit](https://cdn-learn.adafruit.com/downloads/pdf/adafruit-bmp280-barometric-pressure-plus-temperature-sensor-breakout.pdf)
---