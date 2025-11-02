# NodeMCU #5: MQ-135 (Qualidade do Ar) 💨

Esta placa é dedicada ao monitoramento da qualidade do ar (concentração de gases) usando o sensor [MQ-135](https://www.makerhero.com/blog/como-funciona-o-sensor-de-gas-mq-135/?srsltid=AfmBOorwAhi8It0JkbR7EZQjq7tZ1lGu5Q90_x1cF1DfJ-fndEwpIFdH).

## 🔩 Pinagem e Aquecimento

O MQ-135 deve ser alimentado constantemente para o **período de aquecimento (burn-in)** necessário, que pode levar até 20 minutos, garantindo leituras estáveis.

| Componente | Pino da NodeMCU | Observação |
| :--- | :--- | :--- |
| **MQ-135 (Data)** | **A0** | Saída analógica bruta (0-1023) para medição da concentração de gases. |
| **MQ-135 (VCC)** | **3.3V** | Alimentação **FIXA**. Deve permanecer ligado. |
| **MQ-135 (GND)** | **GND** | Aterramento. |

## 💡 Esquema JSON Esperado (No Backend)

O backend recebe o valor analógico bruto do sensor:

```json
{
    "qualidade_ar_raw": 350
}

EXPECTED_SCHEMA_NODE_5 no .env deve ser: "qualidade_ar_raw"