#!/bin/bash
# Ativa o ambiente virtual
source ../.venv/bin/activate
echo "Iniciando backend em segundo plano..."
python backend_central.py &
echo "Iniciando aplicação Streamlit..."
streamlit run streamlit_app.py