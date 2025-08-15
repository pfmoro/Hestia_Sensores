import subprocess
import re
import socket
import sys
import requests

def get_ip_from_mac(mac_address):
    """
    Tenta encontrar o IP de um dispositivo a partir do seu endereço MAC.
    Primeiro, tenta buscar na tabela ARP do sistema. Se falhar, faz uma busca
    por faixa de IPs comuns para encontrar a NodeMCU.
    """
    print(f"Tentando encontrar o IP para o MAC: {mac_address}...")

    # Tenta o método padrão de busca na tabela ARP
    ip_encontrado = _find_ip_from_arp(mac_address)
    
    if ip_encontrado:
        print(f"IP encontrado via tabela ARP: {ip_encontrado}")
        return ip_encontrado
    
    print("Falha ao encontrar IP na tabela ARP. Tentando busca por faixa de IPs...")
    
    # Se o método ARP falhar, tenta o fallback com a faixa de IPs
    ip_encontrado_fallback = _find_ip_by_range(mac_address)

    if ip_encontrado_fallback:
        print(f"IP encontrado via busca por faixa de IPs: {ip_encontrado_fallback}")
        return ip_encontrado_fallback
    
    print(f"Não foi possível encontrar o IP para o MAC {mac_address}.")
    return None

def _find_ip_from_arp(mac_address):
    """
    Busca o IP na tabela ARP do sistema. Pode falhar em sistemas sem acesso root.
    """
    try:
        if sys.platform.startswith('linux') or sys.platform.startswith('darwin'):
            # Comando para Linux e macOS
            output = subprocess.check_output(['arp', '-a'], timeout=5).decode('utf-8')
        elif sys.platform.startswith('win'):
            # Comando para Windows
            output = subprocess.check_output(['arp', '-a'], timeout=5).decode('utf-8')
        else:
            return None # Não suportado
        
        lines = output.splitlines()
        for line in lines:
            if mac_address.lower() in line.lower():
                ip_match = re.search(r'(\d{1,3}\.\d{1,3}\.\d{1,3}\.\d{1,3})', line)
                if ip_match:
                    return ip_match.group(1)
        return None
    except (subprocess.CalledProcessError, subprocess.TimeoutExpired, FileNotFoundError):
        return None

def _find_ip_by_range(mac_address):
    """
    Faz uma busca de força bruta em uma faixa de IPs comuns para encontrar o MAC.
    """
    # Apenas como exemplo. O ideal é que o endereço da NodeMCU seja um valor fixo.
    base_ip = "192.168.0."
    for i in range(200, 300):
        ip = base_ip + str(i)
        
        # Tenta enviar uma requisição e verificar a resposta
        try:
            response = requests.get(f"http://{ip}", timeout=1)
            # Verifica se o MAC da resposta HTTP corresponde ao MAC procurado
            if response.headers.get('X-ESP8266-MAC', '').lower() == mac_address.lower():
                return ip
        except requests.exceptions.RequestException:
            continue
    return None