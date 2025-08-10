@echo off

rem Ativa o ambiente virtual

call ..\.venv\Scripts\activate.bat

start "Streamlit App" streamlit run streamlit_app.py