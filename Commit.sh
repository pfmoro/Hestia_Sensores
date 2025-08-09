#!/bin/bash


# Lembre-se de dar permissão de execução: chmod +x commit.sh


read -p "Digite sua mensagem de commit: " message


echo "Adicionando arquivos..."
git add .


echo "Efetuando commit..."
git commit -m "$message"


echo "Enviando para o repositório remoto..."
git push


echo "Processo concluído."