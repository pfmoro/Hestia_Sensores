import requests
import os
import json
from dotenv import load_dotenv


# Carrega as variáveis de ambiente
load_dotenv()


# --- Funções de Upload ---


def save_data_atomically(data, path, filename="dados_completos.json"):
    if not path:
        print("Caminho de salvamento local não definido. Ignorando...")
        return

    if path == ".":
        full_path = os.path.join(os.path.dirname(__file__), filename)
    else:
        os.makedirs(path, exist_ok=True)
        full_path = os.path.join(path, filename)

    temp_filename = full_path + ".tmp"

    # Lê dados existentes
    if os.path.exists(full_path):
        try:
            with open(full_path, "r") as f:
                existing_data = json.load(f)
                if not isinstance(existing_data, list):
                    existing_data = [existing_data]
        except json.JSONDecodeError:
            existing_data = []
    else:
        existing_data = []

    # Append
    existing_data.append(data)

    # Escrita atômica
    with open(temp_filename, "w") as f:
        json.dump(existing_data, f, indent=4)

    os.replace(temp_filename, full_path)
    print(f"Dados adicionados em '{full_path}' com sucesso.")



def upload_thingspeak(data):
    """Envia os dados coletados para o ThingSpeak."""
    api_key = os.getenv("THINGSPEAK_API_KEY")
    if not api_key:
        print("Erro: THINGSPEAK_API_KEY não encontrada no arquivo .env.")
        return

    # Mapeamento de todos os campos coletados para os fields do ThingSpeak.
    # Você deve verificar se o seu canal ThingSpeak tem fields suficientes.
    payload = {
        "api_key": api_key,
        
        # NodeMCU #1 (Exemplo)
        "field1": data.get("temperatura_dht"),
        "field2": data.get("umidade_dht"),
        "field3": data.get("luminosidade_ldr"),
        
        # NodeMCU #3 (Ruidosos)
        "field4": data.get("umidade_solo_perc"),
        "field5": data.get("chuva"),
        
        # NodeMCU #4 (BMP280)
        "field6": data.get("temperatura_bmp"),
        "field7": data.get("pressao_bmp"),
        
        # NodeMCU #5 (MQ-135)
        "field8": data.get("qualidade_ar_raw"), 
    }
    
    # Remove entradas None (dados não coletados)
    payload = {k: v for k, v in payload.items() if v is not None}
    
    try:
        url = "https://api.thingspeak.com/update"
        response = requests.post(url, data=payload, timeout=10)
        response.raise_for_status()
        print(f"Dados enviados para o ThingSpeak com sucesso. Resposta: {response.text}")
    except requests.exceptions.RequestException as e:
        print(f"Erro ao enviar dados para o ThingSpeak: {e}")


def upload_google_drive(data):
    """
    Função de placeholder para upload de dados para o Google Drive.
    """
    print("Iniciando upload para o Google Drive...")
    print("A função de upload para o Google Drive exige configuração da API.")
    print("Upload para o Google Drive concluído (placeholder).")


def upload_data(data, local_save_path, environment):
    """
    Função principal que gerencia todos os serviços de upload.
    """
    print("\n--- Iniciando uploads para serviços externos ---")


    # Salva os dados localmente
    save_data_atomically(data, local_save_path)


    # Envia os dados para o ThingSpeak
    upload_thingspeak(data)
    
    # Upload para o Google Drive (condicional)
    google_drive_api_key = os.getenv("GOOGLE_DRIVE_API_KEY")
    if environment.lower() in ["windows", "linux_pc"] and google_drive_api_key:
        upload_google_drive(data)
    elif not google_drive_api_key:
        print("Upload para o Google Drive desativado: Chave da API não fornecida.")
    else: # Ambiente Android
        print("Upload para o Google Drive desativado para ambiente Android.")


    print("--- Todos os uploads concluídos ---")