#!/bin/bash

# Script de inicialização para aplicação embarcada
# Inicia frontend e backend simultaneamente

echo "🚀 Iniciando aplicação embarcada..."

# Função para capturar sinais de parada
cleanup() {
    echo "🛑 Parando aplicação..."
    kill $FRONTEND_PID $BACKEND_PID 2>/dev/null
    exit 0
}

# Configurar captura de sinais
trap cleanup SIGTERM SIGINT

# Iniciar backend em background
echo "🐍 Iniciando backend (Python) na porta 8000..."
cd /app/backend
python app.py &
BACKEND_PID=$!

# Aguardar backend inicializar
sleep 2

# Iniciar frontend em background
echo "🟢 Iniciando frontend (Node.js) na porta 3000..."
cd /app/frontend
node server.js &
FRONTEND_PID=$!

# Aguardar frontend inicializar
sleep 2

echo "✅ Aplicação embarcada rodando!"
echo "   Frontend: http://localhost:3000"
echo "   Backend:  http://localhost:8000"

# Aguardar qualquer processo terminar
wait
