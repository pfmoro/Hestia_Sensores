import subprocess
import pandas as pd
import re
import platform
import json
import requests
import os


# --- Lógica de Persistência do IP ---


def _save_last_known_ip(ip, filename="last_known_ip.txt"):
    """Salva o IP mais recente em um arquivo para uso futuro."""
    with open(filename, "w") as f:
        f.write(ip)
    print(f"IP descoberto ({ip}) salvo para a próxima iteração.")


def _load_last_known_ip(filename="last_known_ip.txt"):
    """Lê o último IP salvo no arquivo, se existir."""
    if os.path.exists(filename):
        with open(filename, "r") as f:
            return f.read().strip()
    return None


# --- Lógica de Validação e Chamada HTTP ---


def _validate_and_test_ip(ip, expected_schema):
    """
    Faz uma chamada HTTP para o IP e valida a resposta com o schema esperado.
    Retorna True se a resposta for válida, False caso contrário.
    """
    try:
        response = requests.get(f"http://{ip}/", timeout=5)
        response.raise_for_status()  # Lança um erro para status de erro HTTP
        data = response.json()
        
        # Valida se todas as chaves esperadas estão no JSON
        for key in expected_schema:
            if key not in data:
                print(f"Erro: Chave '{key}' não encontrada no JSON de {ip}.")
                return False
        
        print(f"Sucesso: IP {ip} respondeu com o schema esperado.")
        return True
        
    except requests.exceptions.RequestException as e:
        print(f"Erro de conexão com {ip}: {e}")
        return False
    except ValueError as e:
        print(f"Erro de parsing JSON em {ip}: {e}")
        return False


# --- Lógica de Tabela ARP ---


def get_arp_table():
    """
    Executa o comando 'arp -a' e retorna um DataFrame com as colunas
    IP Address, MAC Address e Type. Funciona no Windows e Linux.
    """
    system = platform.system().lower()
    
    try:
        command = ["arp", "-a"]
        result = subprocess.run(command, capture_output=True, text=True)
        output = result.stdout
    except Exception as e:
        raise RuntimeError(f"Erro ao executar 'arp -a': {e}")
    
    data = []
    
    if "windows" in system:
        pattern = re.compile(r"(\d+\.\d+\.\d+\.\d+)\s+([0-9a-fA-F-]{17})\s+(\w+)")
    else:
        pattern = re.compile(r"(\d+\.\d+\.\d+\.\d+)\s+[\w]+\s+([0-9a-fA-F:]{17})\s+")
    
    for line in output.splitlines():
        match = pattern.search(line)
        if match:
            ip = match.group(1)
            mac = match.group(2).replace('-', ':')  # Normaliza para o formato do Linux
            data.append((ip, mac, None))
    
    df = pd.DataFrame(data, columns=["IP Address", "MAC Address", "Type"])
    return df


def _get_all_ips_from_mac(mac_address):
    """
    Recebe um MAC Address, consulta a tabela ARP e retorna uma lista
    de todos os IPs correspondentes, ou uma lista vazia se não encontrar.
    """
    df = get_arp_table()
    
    # Normaliza o formato do MAC (maiúsculas) para a busca
    mac_address = mac_address.upper().replace('-', ':')
    df["MAC Address"] = df["MAC Address"].str.upper().str.replace('-', ':')
    
    matches = df[df["MAC Address"] == mac_address]
    
    if not matches.empty:
        return matches["IP Address"].tolist()
    else:
        return []


# --- Função Principal de Descoberta ---


def find_nodemcu1_ip(mac_address, expected_schema):
    """
    Função principal que busca o IP da NodeMCU #1.
    Prioriza o último IP conhecido e, se falhar, testa todos os outros IPs.
    """
    print(f"Iniciando busca pelo IP da NodeMCU #1 (MAC: {mac_address})...")
    
    # Carrega o IP salvo da última execução
    last_known_ip = _load_last_known_ip()
    
    # Obtém todos os IPs associados ao MAC
    all_ips = _get_all_ips_from_mac(mac_address)
    
    # Se houver um IP salvo, ele é o primeiro a ser testado
    if last_known_ip and last_known_ip in all_ips:
        all_ips.remove(last_known_ip)
        all_ips.insert(0, last_known_ip)
    
    # Loop pelos IPs encontrados
    for ip in all_ips:
        if _validate_and_test_ip(ip, expected_schema):
            _save_last_known_ip(ip)
            return ip
            
    print("Nenhum IP válido encontrado para a NodeMCU #1.")
    return None