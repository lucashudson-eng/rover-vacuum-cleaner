// Configuração central do backend
// Usa VITE_BACKEND_URL se definida; caso contrário, tenta inferir a origem atual
const defaultHost = typeof window !== 'undefined' ? window.location.hostname : 'localhost'
const defaultPort = 8000

export const BACKEND_URL =
  import.meta?.env?.VITE_BACKEND_URL?.trim() || `http://${defaultHost}:${defaultPort}`


