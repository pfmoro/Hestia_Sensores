#!/bin/bash
echo "Iniciando backend em segundo plano..."
python backend_central.py &
echo "Backend iniciado. Verifique o log para a saída."