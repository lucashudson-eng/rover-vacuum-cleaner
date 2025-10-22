# Multi-stage build para aplicação embarcada
# Stage 1: Build do Frontend React (Node.js 20)
FROM node:20-alpine AS frontend-builder

WORKDIR /app/frontend

# Copiar package.json e instalar dependências
COPY frontend/package*.json ./
RUN npm install

# Copiar código fonte do frontend
COPY frontend/ ./

# Build da aplicação React
RUN npm run build

# Stage 2: Build do Backend (Python 3.10-slim)
FROM python:3.10-slim AS backend-builder

WORKDIR /app/backend

# Instalar dependências do sistema
RUN apt-get update && apt-get install -y \
    gcc \
    && rm -rf /var/lib/apt/lists/*

# Copiar requirements e instalar dependências Python
COPY backend/requirements.txt ./
RUN pip install --no-cache-dir -r requirements.txt

# Copiar código fonte do backend
COPY backend/ ./

# Stage 3: Imagem final otimizada
FROM python:3.10-slim

# Instalar Node.js 20 no sistema final
RUN apt-get update && apt-get install -y \
    curl \
    && curl -fsSL https://deb.nodesource.com/setup_20.x | bash - \
    && apt-get install -y nodejs \
    && apt-get clean \
    && rm -rf /var/lib/apt/lists/*

# Criar usuário não-root para segurança
RUN groupadd -r appuser && useradd -r -g appuser appuser

# Configurar diretórios de trabalho
WORKDIR /app

# Copiar frontend buildado do stage 1
COPY --from=frontend-builder /app/frontend/dist ./frontend/dist
COPY --from=frontend-builder /app/frontend/server.js ./frontend/
COPY --from=frontend-builder /app/frontend/package.json ./frontend/
COPY --from=frontend-builder /app/frontend/node_modules ./frontend/node_modules

# Copiar backend do stage 2
COPY --from=backend-builder /usr/local/lib/python3.10/site-packages /usr/local/lib/python3.10/site-packages
COPY --from=backend-builder /usr/local/bin /usr/local/bin
COPY backend/ ./backend

# Criar script de inicialização
COPY start.sh ./
RUN chmod +x start.sh

# Configurar permissões
RUN chown -R appuser:appuser /app
USER appuser

# Expor portas
EXPOSE 3000 8000

# Script de inicialização
CMD ["./start.sh"]