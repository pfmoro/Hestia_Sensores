import requests
import os
import json
from dotenv import load_dotenv
from ip_finder import find_nodemcu_ip 
from communication_service import get_data_from_nodemcu
from upload_service import upload_data

# Carrega as variáveis de ambiente
load_dotenv()

# Dicionário de configuração das NodeMCUs
NODEMCU_CONFIGS = {
    '1': {
        'name':"NODEMCU_1",
        'mac': os.getenv("MAC_NODEMCU_1"),
        'schema': os.getenv("EXPECTED_SCHEMA_NODE_1"),
        'ip': os.getenv("IP_NODEMCU_1"),
    },
    '3': { # NodeMCU Ruidosos
        'name':"NODEMCU_3",
        'mac': os.getenv("MAC_NODEMCU_3"),
        'schema': os.getenv("EXPECTED_SCHEMA_NODE_3"),
        'ip': os.getenv("IP_NODEMCU_3"),
    },
    '4': { # NodeMCU BMP280
        'name':"NODEMCU_4",
        'mac': os.getenv("MAC_NODEMCU_4"),
        'schema': os.getenv("EXPECTED_SCHEMA_NODE_4"),
        'ip': os.getenv("IP_NODEMCU_4"),
    },
    '5': { # NodeMCU MQ-135
        'name':"NODEMCU_5",
        'mac': os.getenv("MAC_NODEMCU_5"),
        'schema': os.getenv("EXPECTED_SCHEMA_NODE_5"),
        'ip': os.getenv("IP_NODEMCU_5"),
    },
}

def collect_all_data():
    """
    Coleta dados de todas as NodeMCUs configuradas, pulando as que têm schema vazio.
    """
    all_data = {}
    
    for num, config in NODEMCU_CONFIGS.items():
        schema_list = [s.strip() for s in config['schema'].split(',')] if config['schema'] else []

        if not schema_list:
            print(f"NodeMCU #{num} PULAR: EXPECTED_SCHEMA_NODE_{num} está vazio.")
            continue  # Pula esta NodeMCU se o schema for vazio

        print(f"\n--- Coletando dados da NodeMCU #{num} ---")
        
        # 1. Tenta usar o IP fixo ou buscar o IP
        ip_address = config['ip']
        if not ip_address:
             # Se o IP fixo falhar, tenta buscar (implementação em ip_finder.py)
             ip_address = find_nodemcu_ip(config['mac'], schema_list,config['name'])
        
        if ip_address:
            # 2. Coleta os dados
            data = get_data_from_nodemcu(ip_address, schema_list)
            if data:
                all_data.update(data)
        else:
            print(f"ERRO: Não foi possível obter o IP ou os dados da NodeMCU #{num}.")

    return all_data

def main():
    tempo_espera = int(os.getenv("TEMPO_ESPERA_SEGUNDOS", 60))
    local_save_path = os.getenv("LOCAL_SAVE_PATH", ".")
    environment = os.getenv("EXECUTION_ENVIRONMENT", "linux_pc")

    while True:
        print(f"\n{'='*50}")
        print(f"Iniciando ciclo de coleta de dados...")
        print(f"{'='*50}")
        
        # 1. Coletar dados de todas as NodeMCUs ativas
        data_to_upload = collect_all_data()

        if data_to_upload:
            # 2. Adicionar timestamp
            data_to_upload['timestamp'] = datetime.datetime.now().isoformat()
            
            # 3. Enviar para serviços
            upload_data(data_to_upload, local_save_path, environment)
        else:
            print("Nenhum dado coletado das NodeMCUs ativas. Pulando o upload.")

        print(f"\n--- Aguardando {tempo_espera} segundos para o próximo ciclo... ---")
        time.sleep(tempo_espera)

if __name__ == "__main__":
    # Importações adicionais necessárias para o main (se não estiverem no início)
    import datetime
    import time
    main()
