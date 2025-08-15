import subprocess
import re
import requests
import os
import sys

# --- Lógica de Persistência do IP ---

def _save_last_known_ip(ip, filename="last_known_ip.txt"):
    """Salva o IP mais recente em um arquivo para uso futuro."""
    try:
        with open(filename, "w") as f:
            f.write(ip)
        print(f"IP descoberto ({ip}) salvo para a próxima iteração.")
    except IOError as e:
        print(f"Erro ao salvar o IP: {e}")

def _load_last_known_ip(filename="last_known_ip.txt"):
    """Lê o último IP salvo no arquivo, se existir."""
    if os.path.exists(filename):
        try:
            with open(filename, "r") as f:
                return f.read().strip()
        except IOError as e:
            print(f"Erro ao carregar o último IP: {e}")
    return None

# --- Lógica de Validação e Chamada HTTP ---

def _validate_and_test_ip(ip, expected_schema, mac_address=None):
    """
    Faz uma chamada HTTP para o IP e valida a resposta.
    Retorna True se a resposta for válida, False caso contrário.
    """
    try:
        response = requests.get(f"http://{ip}/", timeout=5)
        response.raise_for_status()  # Lança um erro para status de erro HTTP
        
        # O cabeçalho HTTP 'X-ESP8266-MAC' pode ser usado para validação.
        # Verifica se o cabeçalho existe antes de tentar a comparação.
        # Nota: Seu firmware NodeMCU_1.ino não envia este cabeçalho.
        if mac_address and 'X-ESP8266-MAC' in response.headers:
            if response.headers.get('X-ESP8266-MAC', '').lower() != mac_address.lower():
                print(f"Erro: MAC do cabeçalho '{response.headers.get('X-ESP8266-MAC')}' não corresponde ao MAC esperado '{mac_address}'.")
                return False

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

def _find_ip_from_arp(mac_address):
    """
    Busca o IP na tabela ARP do sistema. Pode falhar em sistemas sem acesso root.
    """
    print("Tentando buscar na tabela ARP...")
    try:
        if sys.platform.startswith('linux') or sys.platform.startswith('darwin'):
            # Comando para Linux e macOS
            output = subprocess.check_output(['arp', '-a'], timeout=5).decode('utf-8', errors='ignore')
        elif sys.platform.startswith('win'):
            # Comando para Windows
            output = subprocess.check_output(['arp', '-a'], timeout=5).decode('latin-1', errors='ignore')
        else:
            print("Plataforma não suportada para busca ARP.")
            return None
        
        lines = output.splitlines()
        for line in lines:
            if mac_address.lower() in line.lower():
                ip_match = re.search(r'(\d{1,3}\.\d{1,3}\.\d{1,3}\.\d{1,3})', line)
                if ip_match:
                    print(f"IP encontrado via tabela ARP: {ip_match.group(1)}")
                    return ip_match.group(1)
        return None
    except (subprocess.CalledProcessError, subprocess.TimeoutExpired, FileNotFoundError):
        print("Falha ao acessar a tabela ARP.")
        return None

# --- Lógica de Fallback por Faixa de IPs ---

def _find_ip_by_range(mac_address, expected_schema):
    """
    Faz uma busca de força bruta em uma faixa de IPs comuns para encontrar o MAC.
    """
    print("Tentando busca por faixa de IPs (fallback)...")
    
    # Exemplo: busca na faixa de 192.168.0.200 a 192.168.0.299
    base_ip = "192.168.0."
    for i in range(200, 300):
        ip = base_ip + str(i)
        
        if _validate_and_test_ip(ip, expected_schema, mac_address):
            return ip
            
    return None

# --- Função Principal de Descoberta (usada pelo backend_central.py) ---

def find_nodemcu1_ip(mac_address, expected_schema):
    """
    Função principal que busca o IP da NodeMCU.
    Prioriza o último IP conhecido e, se falhar, tenta a tabela ARP.
    Se tudo falhar, usa uma busca por faixa de IPs.
    """
    print(f"Iniciando busca pelo IP da NodeMCU (MAC: {mac_address})...")
    
    # 1. Tenta o último IP conhecido (método mais rápido)
    last_known_ip = _load_last_known_ip()
    if last_known_ip and _validate_and_test_ip(last_known_ip, expected_schema, mac_address):
        print(f"Último IP conhecido {last_known_ip} é válido.")
        return last_known_ip
    
    # 2. Tenta a busca na tabela ARP
    ip_from_arp = _find_ip_from_arp(mac_address)
    if ip_from_arp and _validate_and_test_ip(ip_from_arp, expected_schema, mac_address):
        _save_last_known_ip(ip_from_arp)
        return ip_from_arp
    
    # 3. Tenta o fallback por faixa de IPs
    ip_from_range = _find_ip_by_range(mac_address, expected_schema)
    if ip_from_range:
        _save_last_known_ip(ip_from_range)
        return ip_from_range
    
    print("Nenhum IP válido encontrado para a NodeMCU.")
    return None