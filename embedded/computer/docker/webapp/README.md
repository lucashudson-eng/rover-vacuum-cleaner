# Rover Vacuum Cleaner - Sistema Embarcado

Sistema web embarcado para controle de rover aspirador autônomo, desenvolvido com React + Vite no frontend e Flask no backend, otimizado para recursos limitados.

## Estrutura do Projeto

```
webapp/
├── frontend/          # Aplicação React + Vite
│   ├── src/           # Código fonte React
│   │   ├── components/ # Componentes React
│   │   ├── App.jsx    # Componente principal
│   │   └── main.jsx   # Ponto de entrada
│   ├── public/        # Arquivos estáticos
│   ├── package.json   # Dependências React
│   ├── server.js     # Servidor Express para produção
│   └── vite.config.js # Configuração Vite
├── backend/           # API Flask
│   ├── requirements.txt # Dependências Python
│   └── app.py         # API Flask
├── Dockerfile         # Build multi-stage otimizado
├── start.sh           # Script de inicialização
└── README.md          # Este arquivo
```

## Como Executar

### Usando Docker (Aplicação Embarcada)

1. **Construir a imagem:**
   ```bash
   docker build -t webapp-embarcada .
   ```

2. **Executar o container:**
   ```bash
   docker run -p 3000:3000 -p 8000:8000 webapp-embarcada
   ```

3. **Executar em background:**
   ```bash
   docker run -d -p 3000:3000 -p 8000:8000 --name webapp webapp-embarcada
   ```

4. **Parar o container:**
   ```bash
   docker stop webapp
   docker rm webapp
   ```

### Acessos

- **Frontend:** http://localhost:3000
- **Backend API:** http://localhost:8000
- **Health Check Frontend:** http://localhost:3000/api/health
- **Health Check Backend:** http://localhost:8000/api/health

## Desenvolvimento

### Frontend (React + Vite)
- Framework: React 18 + Vite
- UI: Componentes responsivos com CSS moderno
- Porta: 3000
- Build otimizado para produção

### Backend (Python 3.10-slim)
- Framework: Flask
- Porta: 8000
- API REST para controle do rover
- CORS habilitado para frontend

## Funcionalidades

### Interface Web
- **Dashboard responsivo** com navegação intuitiva
- **Controle do rover** via interface web
- **Mapa de navegação** em tempo real
- **Câmera integrada** para monitoramento
- **Configurações** do sistema
- **Menu adaptativo** para desktop e mobile

### Otimizações para Embarcado
- **Build multi-stage** reduz tamanho da imagem
- **Usuário não-root** para segurança
- **Recursos mínimos** necessários
- **Inicialização rápida** com script otimizado
- **Interface responsiva** para diferentes telas

## Próximos Passos

1. Implementar comunicação serial com rover
2. Adicionar controle de motores
3. Implementar sistema de navegação
4. Adicionar sensores de obstáculos
5. Configurar telemetria em tempo real

## Vantagens da Aplicação Embarcada

✅ **Eficiência:** Uma única imagem Docker  
✅ **Simplicidade:** Um container para gerenciar  
✅ **Otimização:** Build multi-stage reduz tamanho final  
✅ **Deploy:** Mais fácil para sistemas embarcados  
✅ **Recursos:** Menor consumo de memória e CPU  

## Comandos Úteis

```bash
# Ver logs do container
docker logs webapp -f

# Executar comandos no container
docker exec -it webapp bash

# Reconstruir a imagem
docker build -t webapp-embarcada . --no-cache

# Verificar tamanho da imagem
docker images webapp-embarcada
```
