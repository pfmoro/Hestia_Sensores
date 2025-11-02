# NodeMCU #3: Sensores Ruidosos (Umidade do Solo Capacitivo e Chuva) 💧

Esta placa isola os sensores de medição de água, que podem introduzir ruído elétrico na NodeMCU. O código implementa o controle de energia para ligar os sensores apenas no momento da leitura.

## 🔩 Pinagem e Controle de Energia

| Componente | Pino da NodeMCU | Observação |
| :--- | :--- | :--- |
| **Umidade do Solo (Data)** | **A0** | Sensor Capacitivo. Leitura analógica. |
| **Umidade do Solo (VCC)** | **D6** (GPIO12) | **CONTROLE DE ENERGIA:** Atraso de 500ms é usado para estabilização da leitura. |
| **Chuva (Data)** | **D5** (GPIO14) | Módulo Sensor de Chuva (Saída Digital). |
| **Chuva (VCC)** | **D7** (GPIO13) | **CONTROLE DE ENERGIA:** Liga apenas durante a leitura. |

## 💡 Esquema JSON Esperado (No Backend)

O backend espera este esquema para a NodeMCU #3:

```json
{
    "umidade_solo": 450,
    "estado_chuva": 1
}

EXPECTED_SCHEMA_NODE_3 no .env deve ser: "umidade_solo,estado_chuva"