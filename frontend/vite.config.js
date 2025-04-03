import { defineConfig } from 'vite'
import react from '@vitejs/plugin-react'

export default defineConfig({
  plugins: [react()],
  server: {
    host: '0.0.0.0', // Permite acceso desde otros dispositivos
    port: 5173,      // Puedes especificar el puerto que prefieras
  }
})
