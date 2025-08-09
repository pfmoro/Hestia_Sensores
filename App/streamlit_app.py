import streamlit as st
import json
import os
import datetime
import requests
import pytz


# --- Funções Auxiliares ---


def get_geolocation():
    """
    Obtém a latitude e longitude da rede atual usando a API ip-api.com.
    """
    try:
        response = requests.get("http://ip-api.com/json")
        response.raise_for_status()
        data = response.json()
        if data['status'] == 'success':
            return data['lat'], data['lon']
        else:
            return None, None
    except requests.exceptions.RequestException:
        return None, None


def get_sideral_time(longitude):
    """
    Calcula a Hora Sideral Local (LST) a partir da hora e longitude.
    """
    now = datetime.datetime.utcnow().replace(tzinfo=pytz.utc)
    
    # 1. Calcular a data Juliana
    jd = now.toordinal() + 1721424.5 + now.hour/24 + now.minute/1440 + now.second/86400
    
    # 2. Calcular a Hora Sideral Média de Greenwich (GMST) em horas
    gmst_hours = (6.697374558 + 0.06570982441908 * (jd - 2451545.0) + now.hour + now.minute/60 + now.second/3600) % 24
    
    # 3. Calcular a Hora Sideral Local (LST)
    lst_hours = (gmst_hours + longitude / 15) % 24
    
    return lst_hours


# --- Lógica Principal do Streamlit ---


def main():
    st.title("Monitoramento de Sensores e Dados Astronômicos")


    # 1. Ler o arquivo de dados
    try:
        with open("dados_completos.json", "r") as f:
            data = json.load(f)
        
        last_updated_timestamp = os.path.getmtime("dados_completos.json")
        last_updated_time = datetime.datetime.fromtimestamp(last_updated_timestamp)
        
    except FileNotFoundError:
        st.error("Arquivo 'dados_completos.json' não encontrado. Certifique-se de que o backend está em execução.")
        data = {}
        last_updated_time = "N/A"
    
    # 2. Obter informações adicionais
    local_time = datetime.datetime.now()
    lat, lon = get_geolocation()
    
    sideral_time = "N/A"
    if lon is not None:
        sideral_hours = get_sideral_time(lon)
        sideral_time_h = int(sideral_hours)
        sideral_time_m = int((sideral_hours - sideral_time_h) * 60)
        sideral_time_s = int(((sideral_hours - sideral_time_h) * 60 - sideral_time_m) * 60)
        sideral_time = f"{sideral_time_h:02d}:{sideral_time_m:02d}:{sideral_time_s:02d}"


    # 3. Exibir os dados
    st.markdown("---")
    st.subheader("Dados Coletados")
    
    col1, col2, col3 = st.columns(3)
    
    # Dados da NodeMCU #1
    col1.metric("Temperatura", f"{data.get('temperatura', 'N/A')} °C")
    col2.metric("Umidade", f"{data.get('umidade', 'N/A')} %")
    col3.metric("Luminosidade", f"{data.get('luminosidade', 'N/A')}")
    
    # Dados da NodeMCU #2
    col1.metric("Umidade do Solo", f"{data.get('umidade_solo', 'N/A')}")
    col2.metric("Estado da Chuva", f"{'Chuva detectada' if data.get('estado_chuva') == 1 else 'Sem chuva'}")


    st.markdown("---")
    st.subheader("Metadados")
    st.write(f"**Última Atualização:** {last_updated_time.strftime('%Y-%m-%d %H:%M:%S')}")
    st.write(f"**Hora Local:** {local_time.strftime('%Y-%m-%d %H:%M:%S')}")
    st.write(f"**Hora Sideral Local:** {sideral_time}")
    st.write(f"**Latitude:** {lat}")
    st.write(f"**Longitude:** {lon}")


    # Botão de refresh para reexecutar o script
    if st.button("Atualizar Dados"):
        st.rerun()


if __name__ == "__main__":
    main()