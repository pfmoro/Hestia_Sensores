@echo off
set /p message="Digite sua mensagem de commit: "
echo.
git add .
git commit -m "%message%"
git push
pause