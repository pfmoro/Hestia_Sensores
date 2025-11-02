# NodeMCU #1: DHT11 e LDR (Temperatura, Umidade e Luminosidade) ☀️

Esta placa é responsável por monitorar as condições ambientais básicas, como temperatura, umidade e luminosidade.

## 🔩 Pinagem

| Componente | Pino da NodeMCU | Observação |
| :--- | :--- | :--- |
| **Sensor DHT11 (Data)** | **D2** (GPIO4) | Leitura de Temperatura e Umidade. |
| **Sensor LDR (Data)** | **A0** | Leitura analógica de Luminosidade. Requer um resistor (10kΩ) divisor de tensão. |
| **DHT11/LDR (VCC)** | **3.3V** | Alimentação. |
| **DHT11/LDR (GND)** | **GND** | Aterramento. |

Para que o Backend efetua a leitura desta placa, EXPECTED_SCHEMA_NODE_1 no .env deve ser: "temperatura,umidade,luminosidade"

## 💡 Esquema JSON Esperado (No Backend)

O backend espera este esquema para a NodeMCU #1:

```json
{
    "temperatura": 25.5,
    "umidade": 60,
    "luminosidade": 550
}

