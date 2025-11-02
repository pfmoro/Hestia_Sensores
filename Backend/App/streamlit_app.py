import streamlit as st
import json
import os
import datetime
import requests
import pytz
import math
# Importa as funções auxiliares do novo arquivo
from utils import get_geolocation, obter_meridiano_fuso_local, get_sideral_time, calcular_hora_solar_local



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
        hora_solar = calcular_hora_solar_local(lon)


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
    st.write(f"**Hora Local Oficial:** {local_time.strftime('%Y-%m-%d %H:%M:%S')}")
    st.write(f"**Hora Solar Local Real:** {hora_solar.strftime('%Y-%m-%d %H:%M:%S')}")
    st.write(f"**Hora Sideral Local:** {sideral_time}")
    st.write(f"**Latitude:** {lat}")
    st.write(f"**Longitude:** {lon}")


    # Botão de refresh para reexecutar o script
    if st.button("Atualizar Dados"):
        st.rerun()


if __name__ == "__main__":
    main()