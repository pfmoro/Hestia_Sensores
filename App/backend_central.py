import time
import json
import os
from dotenv import load_dotenv


# Importando as funções dos scripts que iremos criar
from ip_finder import find_nodemcu1_ip
from communication_service import get_data_from_nodemcu1, get_data_from_nodemcu2
from upload_service import upload_data


# Carrega as variáveis do arquivo .env
load_dotenv()


# --- Parâmetros de Configuração ---
TEMPO_ESPERA_SEGUNDOS = int(os.getenv("TEMPO_ESPERA_SEGUNDOS"))
MAC_NODEMCU_1 = os.getenv("MAC_NODEMCU_1")
IP_NODEMCU_2_AP = os.getenv("IP_NODEMCU_2") # IP fixo para o modo bateria
EXPECTED_SCHEMA_NODE_1 = os.getenv("EXPECTED_SCHEMA_NODE_1").split(',')
LOCAL_SAVE_PATH = os.getenv("LOCAL_SAVE_PATH")
EXECUTION_ENVIRONMENT = os.getenv("EXECUTION_ENVIRONMENT")
NODEMCU2_POWER_MODE = os.getenv("NODEMCU2_POWER_MODE")


# Configurações para o modo tomada
MAC_NODEMCU_2_CLIENT = os.getenv("MAC_NODEMCU_2")
EXPECTED_SCHEMA_NODE_2_CLIENT = os.getenv("EXPECTED_SCHEMA_NODE_2").split(',')




# --- Lógica Principal do Backend ---


def main():
    """
    Função principal do backend que roda em loop.
    """
    print("Backend central iniciado. Aguardando a primeira coleta...")
    
    while True:
        try:
            print("\n--- Início do ciclo de coleta ---")
            
            # 1. Coletar dados da NodeMCU #1
            ip_nodemcu1 = find_nodemcu1_ip(MAC_NODEMCU_1, EXPECTED_SCHEMA_NODE_1)
            dados_node1_json = get_data_from_nodemcu1(ip_nodemcu1)
            dados_node1 = json.loads(dados_node1_json)
            print(f"Dados NodeMCU #1: {dados_node1}")
            
            # 2. Coletar dados da NodeMCU #2
            if NODEMCU2_POWER_MODE.lower() == "plugged":
                # Busca o IP se estiver no modo tomada
                ip_nodemcu2 = find_nodemcu1_ip(MAC_NODEMCU_2_CLIENT, EXPECTED_SCHEMA_NODE_2_CLIENT)
                # O parâmetro 'ip_or_ssid' é o IP
                dados_node2_json = get_data_from_nodemcu2(ip_nodemcu2, power_mode=NODEMCU2_POWER_MODE)
            else: # modo bateria
                # O parâmetro 'ip_or_ssid' é o SSID do AP
                dados_node2_json = get_data_from_nodemcu2("NodeMCU_Sensores_2", environment=EXECUTION_ENVIRONMENT, power_mode=NODEMCU2_POWER_MODE)
            
            dados_node2 = json.loads(dados_node2_json)
            print(f"Dados NodeMCU #2: {dados_node2}")
            
            # 3. Combinar os dados e fazer o upload
            dados_completos = {**dados_node1, **dados_node2}
            upload_data(dados_completos, LOCAL_SAVE_PATH, EXECUTION_ENVIRONMENT)
            
            print("--- Fim do ciclo de coleta ---")
            
        except Exception as e:
            print(f"Ocorreu um erro no ciclo de coleta: {e}")
            
        print(f"Aguardando {TEMPO_ESPERA_SEGUNDOS} segundos para o próximo ciclo...")
        time.sleep(TEMPO_ESPERA_SEGUNDOS)


if __name__ == "__main__":
    main()