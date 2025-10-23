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
├── Dockerfile         # Build otimizado para Ubuntu 24
├── start.sh          # Script de inicialização interno
└── README.md         # Este arquivo
```

## Como Executar

### Comandos Docker

```bash
# Construir imagem
docker build -t rover-webapp .

# Executar container
docker run -d --name rover-webapp \
  -p 0.0.0.0:3000:3000 \
  -p 0.0.0.0:8000:8000 \
  --device=/dev/video0:/dev/video0 \
  --privileged \
  rover-webapp

# Parar container
docker stop rover-webapp && docker rm rover-webapp
```

### Acessos

- **Frontend:** http://localhost:3000
- **Backend API:** http://localhost:8000
- **Health Check Frontend:** http://localhost:3000/api/health
- **Health Check Backend:** http://localhost:8000/api/health
- **Stream da Câmera:** http://localhost:8000/video_feed
- **Status da Câmera:** http://localhost:8000/api/camera/status

### Acesso Externo (Celular/Outros Dispositivos)

Para acessar de outros dispositivos na rede local:

1. **Descobrir o IP da máquina:**
   ```bash
   hostname -I
   ```

2. **Parar e recriar o container com o IP correto:**
   ```bash
   docker stop rover-webapp && docker rm rover-webapp
   docker run -d --name rover-webapp \
     -p 0.0.0.0:3000:3000 \
     -p 0.0.0.0:8000:8000 \
     --device=/dev/video0:/dev/video0 \
     --privileged \
     rover-webapp
   ```

4. **Acessar do celular:**
   - URL: `http://SEU_IP:3000`
   - Exemplo: `http://192.168.1.100:3000`

## Configuração da Câmera

### Pré-requisitos
- Câmera USB conectada ao sistema
- Dispositivo `/dev/video0` disponível
- Permissões adequadas para acesso à câmera

### Configuração Docker
O `docker-compose.yml` já está configurado para:
- Mapear o dispositivo `/dev/video0` para o container
- Executar com privilégios necessários para acesso à câmera
- Configurar variáveis de ambiente para display

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
- **Câmera integrada** para monitoramento com streaming MJPEG
- **Configurações** do sistema
- **Menu adaptativo** para desktop e mobile

### Sistema de Câmera
- **Streaming MJPEG** em tempo real da câmera `/dev/video0`
- **Controles de câmera** (rotação, zoom)
- **Status da câmera** com verificação automática
- **Interface responsiva** para visualização do vídeo
- **Qualidade otimizada** para sistemas embarcados

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
docker logs rover-webapp -f

# Executar comandos no container
docker exec -it rover-webapp bash

# Reconstruir a imagem
docker build -t rover-webapp . --no-cache

# Verificar tamanho da imagem
docker images rover-webapp

# Limpar containers parados
docker container prune

# Limpar imagens não utilizadas
docker image prune
```

## Troubleshooting

### Problemas Comuns

1. **Erro "Câmera não disponível":**
   ```bash
   # Verificar se a câmera está conectada
   ls -la /dev/video*
   
   # Testar acesso à câmera
   docker exec -it rover-webapp python3 -c "import cv2; cap = cv2.VideoCapture(0); print('OK' if cap.isOpened() else 'Erro')"
   ```

2. **Erro de permissão na câmera:**
   ```bash
   # Adicionar usuário ao grupo video
   sudo usermod -a -G video $USER
   # Reiniciar o sistema
   ```

3. **Container não inicia:**
   ```bash
   # Ver logs detalhados
   docker logs rover-webapp -f
   
   # Verificar se as portas estão livres
   sudo netstat -tulpn | grep :3000
   sudo netstat -tulpn | grep :8000
   ```

4. **Acesso externo não funciona:**
   ```bash
   # Verificar firewall
   sudo ufw status
   
   # Verificar se o container está escutando em 0.0.0.0
   docker port rover-webapp
   ```
