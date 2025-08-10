import datetime
import math
import requests
import pytz

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

def obter_meridiano_fuso_local():
    """
    Determina o meridiano central do fuso horário local do sistema.
    Exemplo: Para o fuso horário 'America/Sao_Paulo' (UTC-03:00), o meridiano é -45.
    
    Returns:
        float: O meridiano central do fuso horário local em graus.
    """
    # Obtém o fuso horário local do sistema operacional
    tz_local = datetime.datetime.now(pytz.utc).astimezone().tzinfo
    
    # Calcula o offset do fuso horário em segundos
    offset_segundos = tz_local.utcoffset(datetime.datetime.now()).total_seconds()
    
    # Converte o offset de segundos para graus de longitude (15 graus por hora)
    meridiano = (offset_segundos / 3600) * 15
    
    return meridiano

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

def calcular_hora_solar_local(longitude_local):
    """
    Calcula a hora solar local real (hora civil) com base na Equação do Tempo.
    
    Args:
        longitude_local (float): A longitude do local em graus.
        
    Returns:
        datetime.datetime: A hora solar local real.
    """
    
    # Fuso horário oficial local, obtido dinamicamente
    meridiano_fuso = obter_meridiano_fuso_local()

    # 1. Obter a data e hora local do sistema
    agora_local = datetime.datetime.now()
    
    # 2. Calcular o dia juliano (Julian Day)
    # A data local agora é usada para o cálculo do Dia Juliano
    jd = agora_local.toordinal() + 1721424.5 + agora_local.hour/24.0 + agora_local.minute/1440.0 + agora_local.second/86400.0
    
    # 3. Calcular a anomalia média do Sol (M) em graus
    M = (357.5291 + 0.98560028 * (jd - 2451545.0)) % 360
    
    # 4. Calcular a Equação do Tempo (C) em minutos
    C = (1.9148 * math.sin(math.radians(M)) + 0.02 * math.sin(math.radians(2*M)) + 0.0003 * math.sin(math.radians(3*M)))
    
    # 5. Calcular a correção da longitude
    # A correção é a diferença entre a longitude local e o meridiano do fuso.
    # 1 grau de longitude = 4 minutos de tempo (15 graus por hora)
    correcao_longitude = (longitude_local - meridiano_fuso) * 4 
    
    # 6. Aplica as correções à hora local
    hora_solar_real = agora_local + datetime.timedelta(minutes=C + correcao_longitude)
    
    return hora_solar_real