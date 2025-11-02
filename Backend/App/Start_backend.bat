@echo off
rem Ativa o ambiente virtual
call ..\.venv\Scripts\activate.bat
start "Backend Central" python backend_central.py