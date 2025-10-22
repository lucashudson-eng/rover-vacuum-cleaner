import express from 'express';
import cors from 'cors';
import path from 'path';
import { fileURLToPath } from 'url';

const __filename = fileURLToPath(import.meta.url);
const __dirname = path.dirname(__filename);

const app = express();
const PORT = process.env.PORT || 3000;

// Middleware
app.use(cors());
app.use(express.json());

// Servir arquivos estáticos da build do React
app.use(express.static(path.join(__dirname, 'dist')));

// Rota de API para testar conexão
app.get('/api/health', (req, res) => {
  res.json({ 
    status: 'OK', 
    message: 'Frontend React funcionando!',
    timestamp: new Date().toISOString()
  });
});

// Rota para servir a aplicação React (SPA)
app.get('*', (req, res) => {
  res.sendFile(path.join(__dirname, 'dist', 'index.html'));
});

app.listen(PORT, '0.0.0.0', () => {
  console.log(`Frontend React rodando na porta ${PORT}`);
});
