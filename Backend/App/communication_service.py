import requests
import json
import time
import subprocess
from dotenv import load_dotenv
import os
timeout=float(os.getenv("timeout"))

# --- Funções de Comunicação ---

def get_data_from_nodemcu(ip, expected_schema_list):
    """
    Faz a requisição HTTP para a NodeMCU e valida o schema.
    Retorna o JSON da NodeMCU como dicionário ou None em caso de falha.
    """
    try:
        url = f"http://{ip}/"
        response = requests.get(url, timeout=timeout)
        response.raise_for_status() 
        
        data = response.json()
        
        # Validação do schema
        for key in expected_schema_list:
            if key not in data:
                print(f"Erro: Chave '{key}' não encontrada no JSON de {ip}.")
                return None
        
        return data
        
    except requests.exceptions.RequestException as e:
        print(f"Erro de conexão com {ip}: {e}")
        return None
    except ValueError as e:
        print(f"Erro de parsing JSON em {ip}: {e}")
        return None

def get_data_from_nodemcu1(ip):
    # ... (código inalterado) ...
    """
    Coleta dados da NodeMCU #1 fazendo uma requisição HTTP GET para o IP fornecido.
    Retorna o corpo da resposta em formato JSON.
    """
    url = f"http://{ip}/"
    print(f"Tentando coletar dados da NodeMCU #1 em {url}...")
    try:
        response = requests.get(url, timeout=timeout)
        response.raise_for_status()
        
        return response.text
        
    except requests.exceptions.RequestException as e:
        print(f"Erro de conexão com a NodeMCU #1 em {url}: {e}")
        return json.dumps({"error": f"Erro de conexão com NodeMCU #1: {e}"})


def get_data_from_nodemcu2(ip_or_ssid, environment="android", power_mode="battery"):
    """
    Coleta dados da NodeMCU #2, adaptando a forma de conexão de acordo com o modo de energia.
    - Se power_mode="battery", usa a lógica de conexão com AP.
    - Se power_mode="plugged", usa a lógica de conexão com cliente (HTTP GET direto).
    """
    if power_mode.lower() == "plugged":
        url = f"http://{ip_or_ssid}/"
        print(f"Tentando coletar dados da NodeMCU #2 em modo tomada em {url}...")
        try:
            response = requests.get(url, timeout=timeout)
            response.raise_for_status()
            return response.text
        except requests.exceptions.RequestException as e:
            print(f"Erro de conexão com a NodeMCU #2 em modo tomada: {e}")
            return json.dumps({"error": f"Erro de conexão com NodeMCU #2: {e}"})
            
    # Lógica para power_mode="battery" (código existente)
    ssid_nodemcu = ip_or_ssid
    url = f"http://192.168.4.1/"
    
    print(f"Iniciando coleta de dados da NodeMCU #2 em modo bateria...")
    # ... (código restante da função, inalterado) ...
    if environment.lower() in ["windows", "linux_pc"]:
        print(f"Ambiente '{environment}' detectado. Tentando mudar a rede Wi-Fi para '{ssid_nodemcu}'...")
        try:
            if environment.lower() == "windows":
                subprocess.run(["netsh", "wlan", "connect", "name=", ssid_nodemcu], check=True, capture_output=True)
            elif environment.lower() == "linux_pc":
                subprocess.run(["nmcli", "device", "wifi", "connect", ssid_nodemcu], check=True, capture_output=True)
            
            print(f"Rede Wi-Fi alterada com sucesso. Aguardando a conexão...")
            time.sleep(5)
        except subprocess.CalledProcessError as e:
            print(f"Erro ao tentar mudar a rede Wi-Fi: {e.stderr.decode()}")
            return json.dumps({"error": f"Erro ao mudar a rede Wi-Fi para {ssid_nodemcu}"})
    
    else:
        print(f"Ambiente '{environment}' detectado. Por favor, conecte manualmente")
        print(f"o dispositivo Android à rede '{ssid_nodemcu}' para continuar.")


    for _ in range(15):
        try:
            response = requests.get(url, timeout=timeout)
            response.raise_for_status()
            print("Conexão com a NodeMCU #2 bem-sucedida!")
            return response.text
        except requests.exceptions.RequestException:
            print("Aguardando conexão com a NodeMCU #2...")
            time.sleep(1)
            
    print("Erro: Não foi possível conectar à NodeMCU #2 após 15 segundos.")
    return json.dumps({"error": "Falha na conexão com NodeMCU #2 após tentativas."})