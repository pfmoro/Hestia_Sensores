import requests
import os
import json
from dotenv import load_dotenv


# Carrega as variáveis de ambiente
load_dotenv()


# --- Funções de Upload ---


def save_data_atomically(data, path, filename="dados_completos.json"):
    """
    Salva os dados em um arquivo de forma atômica para evitar corrupção.
    """
    if not path:
        print("Caminho de salvamento local não definido. Ignorando...")
        return
        
    if path == ".":
        full_path = os.path.join(os.path.dirname(__file__), filename)
    else:
        full_path = os.path.join(path, filename)
        os.makedirs(path, exist_ok=True)
    
    temp_filename = full_path + ".tmp"
    
    json_data = json.dumps(data, indent=4)
    with open(temp_filename, "w") as f:
        f.write(json_data)
        
    os.replace(temp_filename, full_path)
    print(f"Dados salvos em '{full_path}' com sucesso.")


def upload_thingspeak(data):
    """Envia os dados coletados para o ThingSpeak."""
    api_key = os.getenv("THINGSPEAK_API_KEY")
    if not api_key:
        print("Erro: THINGSPEAK_API_KEY não encontrada no arquivo .env.")
        return


    payload = {
        "api_key": api_key,
        "field1": data.get("temperatura"),
        "field2": data.get("umidade"),
        "field3": data.get("luminosidade"),
        "field4": data.get("umidade_solo"),
        "field5": data.get("estado_chuva"),
        "field6": data.get("temperatura_bmp"), # Novo campo para temperatura do BMP280
        "field7": data.get("pressao_bmp")      # Novo campo para pressão do BMP280
    }
    
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