import subprocess
import re
import requests
import os
import sys
import json
from dotenv import load_dotenv

load_dotenv()

LAST_IP_FILE = os.getenv("LAST_IP_FILE", "last_known_ip.json")
timeout = float(os.getenv("timeout", 5))

# =========================================================
# Lógica de Persistência do IP (JSON por Node)
# =========================================================

def save_last_known_ips(data):
    """Salva os IPs conhecidos em um arquivo JSON."""
    with open(LAST_IP_FILE, "w") as f:
        json.dump(data, f, indent=4)

def load_last_known_ips():
    """Lê o JSON de IPs conhecidos."""
    if not os.path.exists(LAST_IP_FILE):
        return {}
    try:
        with open(LAST_IP_FILE, "r") as f:
            return json.load(f)
    except json.JSONDecodeError:
        return {}

# =========================================================
# Lógica de Validação HTTP
# =========================================================

def _validate_and_test_ip(ip, expected_schema, mac_address=None):
    try:
        response = requests.get(f"http://{ip}/", timeout=timeout)
        response.raise_for_status()

        if mac_address and 'X-ESP8266-MAC' in response.headers:
            if response.headers['X-ESP8266-MAC'].lower() != mac_address.lower():
                return False

        data = response.json()

        for key in expected_schema:
            if key not in data:
                return False

        print(f"Sucesso: IP {ip} respondeu com o schema esperado.")
        return True

    except (requests.exceptions.RequestException, ValueError):
        return False

# =========================================================
# Busca via ARP
# =========================================================

def _find_ip_from_arp(mac_address, expected_schema):
    print("Tentando buscar na tabela ARP...")
    try:
        if sys.platform.startswith(("linux", "darwin")):
            output = subprocess.check_output(['arp', '-a'], timeout=timeout).decode()
        elif sys.platform.startswith("win"):
            output = subprocess.check_output(['arp', '-a'], timeout=timeout).decode("latin-1")
        else:
            return None

        for line in output.splitlines():
            if mac_address.lower() in line.lower():
                match = re.search(r'(\d+\.\d+\.\d+\.\d+)', line)
                if match:
                    ip = match.group(1)
                    if _validate_and_test_ip(ip, expected_schema, mac_address):
                        return ip
        return None

    except Exception:
        return None

# =========================================================
# Busca por faixa de IP
# =========================================================

def _find_ip_by_range(mac_address, expected_schema):
    print("Tentando busca por faixa de IPs...")
    base_ip = "192.168.0."
    for i in range(100, 300):
        ip = base_ip + str(i)
        if _validate_and_test_ip(ip, expected_schema, mac_address):
            return ip
    return None

# =========================================================
# Função principal
# =========================================================

def find_nodemcu_ip(mac_address, expected_schema, nodemcu_name=None):
    """
    Busca o IP da NodeMCU.
    - Usa cache JSON por nome (se fornecido)
    - Tenta ARP
    - Faz brute-force na rede
    """

    last_ips = load_last_known_ips()

    # 1️⃣ IP salvo para esta Node
    if nodemcu_name and nodemcu_name in last_ips:
        ip = last_ips[nodemcu_name]
        print(f"Tentando IP salvo para {nodemcu_name}: {ip}")
        if _validate_and_test_ip(ip, expected_schema, mac_address):
            return ip

    # 2️⃣ Busca via ARP
    ip = _find_ip_from_arp(mac_address, expected_schema)
    if ip:
        _update_cache(last_ips, nodemcu_name, ip)
        return ip

    # 3️⃣ Busca por faixa de IP
    ip = _find_ip_by_range(mac_address, expected_schema)
    if ip:
        _update_cache(last_ips, nodemcu_name, ip)
        return ip

    return None

def _update_cache(last_ips, nodemcu_name, ip):
    if not nodemcu_name:
        return

    # Remove IP duplicado de outras nodes
    for k, v in list(last_ips.items()):
        if v == ip and k != nodemcu_name:
            del last_ips[k]

    last_ips[nodemcu_name] = ip
    save_last_known_ips(last_ips)
