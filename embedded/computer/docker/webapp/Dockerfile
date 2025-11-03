# Multi-stage build otimizado para sistemas embarcados
# Stage 1: Build do frontend
FROM node:20-alpine AS frontend-builder

WORKDIR /app/frontend

# Copiar e instalar dependências do frontend
COPY frontend/package*.json ./
RUN npm install

# Copiar código e fazer build
COPY frontend/ ./
RUN npm run build

# Stage 2: Build do backend e runtime
FROM python:3.10-slim AS backend-builder

# Instalar dependências de runtime necessárias
RUN apt-get update && apt-get install -y \
    curl \
    libglib2.0-0 \
    libsm6 \
    libxext6 \
    libxrender-dev \
    libgomp1 \
    libjpeg-dev \
    libpng-dev \
    libtiff-dev \
    && curl -fsSL https://deb.nodesource.com/setup_20.x | bash - \
    && apt-get install -y nodejs \
    && apt-get clean \
    && rm -rf /var/lib/apt/lists/*

# Criar usuário não-root
RUN groupadd -r appuser && useradd -r -g appuser appuser

# Configurar diretórios
WORKDIR /app

# Copiar e instalar dependências Python
COPY backend/requirements.txt ./backend/
RUN pip install --no-cache-dir -r backend/requirements.txt

# Copiar código do backend
COPY backend/ ./backend/

# Copiar build do frontend do stage anterior
COPY --from=frontend-builder /app/frontend/dist ./frontend/dist
COPY --from=frontend-builder /app/frontend/server.js ./frontend/server.js
COPY --from=frontend-builder /app/frontend/package.json ./frontend/package.json
COPY --from=frontend-builder /app/frontend/node_modules ./frontend/node_modules

# Voltar para diretório raiz
WORKDIR /app

# Copiar script de inicialização
COPY start.sh ./
RUN chmod +x start.sh

# Configurar permissões
RUN chown -R appuser:appuser /app
USER appuser

# Expor portas
EXPOSE 3000 8000

# Script de inicialização
CMD ["./start.sh"]
