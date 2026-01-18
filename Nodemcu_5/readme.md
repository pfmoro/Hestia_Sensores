# NodeMCU #5 – Monitoramento Avançado de Qualidade do Ar (MQ‑135 + DHT11) 💨

Esta é uma **nova versão do README**, atualizada para refletir:

* A criação da biblioteca **`MQ135_corr`** (fork local da MQ135)
* O **processo correto de calibração (RZero)**
* A **leitura estimada de RZero via Serial**
* Reconexão automática de Wi‑Fi e diagnóstico de IP

O projeto evoluiu de uma leitura analógica simples para um sistema de monitoramento ambiental **com compensação atmosférica, calibração explícita e telemetria em nuvem**.

---

## 🔩 Implementações de Hardware

### Sensores e Proteção Elétrica

* **MQ‑135** – Sensor de qualidade do ar (CO₂ equivalente / VOCs)
* **DHT11** – Sensor de temperatura e umidade (usado para correção química)
* **Divisor de tensão no A0** – Protege a entrada analógica da NodeMCU (3,3 V máx)

> ⚠️ O MQ‑135 **deve ser alimentado em 5 V (VIN)** para funcionamento correto do aquecedor interno.

### Tabela de Pinagem

| Componente   | Pino NodeMCU     | Observação                    |
| ------------ | ---------------- | ----------------------------- |
| MQ‑135 (VCC) | **VIN**          | Alimentação 5 V (obrigatório) |
| MQ‑135 (GND) | **GND**          | Terra comum                   |
| MQ‑135 (A0)  | **Divisor → A0** | Proteção do ADC               |
| DHT11 (VCC)  | **3V3**          | Alimentação lógica            |
| DHT11 (DATA) | **D2 (GPIO4)**   | Comunicação digital           |
| DHT11 (GND)  | **GND**          | Terra comum                   |

### Divisor de Tensão (Proteção A0)

Configuração utilizada:

* **R1 = 1 kΩ** (entre MQ‑135 A0 e A0 da NodeMCU)
* **R2 = 2 kΩ** (dois resistores de 1 kΩ em série entre A0 e GND)

Isso resulta em um fator aproximado de:

```
Vout = Vin × (2k / (1k + 2k)) ≈ 0,66
```

No software, a leitura é **re‑escalonada por 1,5** para compensar o divisor.

---

## 🚀 Software – Visão Geral

O firmware realiza:

1. **Leitura analógica do MQ‑135** (com correção do divisor)
2. **Leitura de temperatura e umidade (DHT11)**
3. **Cálculo de PPM corrigido** usando `MQ135_corr`
4. **Estimativa dinâmica de RZero**
5. **Servidor Web (JSON)**
6. **Envio periódico ao ThingSpeak**
7. **Reconexão automática de Wi‑Fi + log de IP**

---

## 📚 Biblioteca `MQ135_corr`

### O que é?

`MQ135_corr` é um **fork local** da biblioteca MQ135 tradicional, criado para:

* Tornar explícito o valor de **RZERO**
* Facilitar **calibração manual baseada em medições reais**
* Expor a função de **estimativa de RZero em tempo real**

📁 O arquivo **`MQ135_corr.h`** deve ficar **no mesmo diretório do `.ino`**.

---

## 🔥 Processo de Burn‑In e Calibração (ESSENCIAL)

### 1️⃣ Burn‑In do Sensor

Antes de qualquer calibração:

* **Mínimo funcional:** 20 minutos ligados
* **Calibração real:** **≥ 24 horas contínuas**
* Ambiente: **ar limpo, externo ou bem ventilado**

> Durante esse período, **não confie nos valores de PPM**.

---

### 2️⃣ Leitura do RZero Estimado

Após o burn‑in, o código imprime no Serial:

```
RZero Estimado: 76.43 kΩ  [VALIDO APÓS BURN-IN ≥ 24h EM AR LIMPO]
```

Esse valor representa o **RZero real do seu sensor específico**.

---

### 3️⃣ Fixando o RZero no Código

Abra o arquivo **`MQ135_corr.h`** e localize:

```cpp
#define MQ135_DEFAULT_RZERO 76.43
```

Substitua pelo valor médio observado no Serial (após estabilidade).

> 📌 Cada sensor MQ‑135 é diferente. **Nunca reutilize o RZero de outro módulo.**

---

## 🌐 Servidor Web – Endpoint `/`

Acessando o IP da placa no navegador:

```json
{
  "temperatura": 25.4,
  "umidade": 61.2,
  "mq135_raw_adc": 312,
  "mq135_raw_corrigido_adc": 468,
  "ppm_corrigido": 11.8,
  "rzero_estimado": 76.4
}
```

---

## ☁️ ThingSpeak

* Envio automático a cada **10 minutos**
* **Field 8** → `ppm_corrigido`

Configuração no código:

```cpp
const unsigned long timerDelay = 600000;
```

---

## 📡 Wi‑Fi – Diagnóstico e Reconexão

O firmware:

* Exibe progresso de conexão
* Mostra o **IP recebido**
* Detecta quedas de conexão
* Reconecta automaticamente

Exemplo no Serial:

```
[SISTEMA] WiFi conectado
[SISTEMA] IP obtido: 192.168.1.42
[WiFi] Conexão perdida! Tentando reconectar...
[WiFi] Reconectado com sucesso. IP: 192.168.1.42
```

---

## 📌 Observações Importantes

* MQ‑135 **não mede CO₂ real**, mas um equivalente baseado em VOCs
* Resultados são **comparativos e ambientais**, não laboratoriais
* Calibração correta **impacta drasticamente a qualidade dos dados**

---

## ✅ Checklist Final

* [ ] MQ‑135 ligado em **VIN (5 V)**
* [ ] Divisor de tensão montado corretamente
* [ ] Burn‑in ≥ 24h
* [ ] RZero observado e fixado em `MQ135_corr.h`
* [ ] Wi‑Fi estável
* [ ] ThingSpeak recebendo dados

---

🚀 **Projeto pronto para monitoramento ambiental contínuo, calibrado e documentado.**
