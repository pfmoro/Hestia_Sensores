#!/bin/bash
echo "Iniciando backend em segundo plano..."
python backend_central.py &
echo "Iniciando aplicação Streamlit..."
streamlit run streamlit_app.py