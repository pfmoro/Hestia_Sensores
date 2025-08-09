@echo off
echo Criando ambiente virtual...
python -m venv .venv


echo Ativando ambiente virtual...
call .venv\Scripts\activate


echo Instalando dependências...
pip install -r requirements.txt


echo Configuração concluída.
pause