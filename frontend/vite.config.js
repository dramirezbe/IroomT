// frontend/vite.config.js
import { defineConfig, loadEnv } from 'vite'
import react from '@vitejs/plugin-react'
import path from 'path'
import dns from 'dns'

// (Optional) keep "localhost" printing consistently under Node ≥17:
dns.setDefaultResultOrder('verbatim')

export default defineConfig(({ mode }) => {
  const envRoot = path.resolve(__dirname, '../')
  const env = loadEnv(mode, envRoot, '')

  return {
    envDir: envRoot,
    plugins: [react()],

    server: {
      // listen on ALL interfaces (LAN, localhost, docker, WSL…)
      host: true,           // or host: '0.0.0.0' :contentReference[oaicite:0]{index=0}

      // pick up your port from .env:
      port: Number(env.VITE_DEV_PORT) || 5173,

      // allow requests to any Host header (localhost, IPs, custom domains…)
      allowedHosts: true,   // [] by default, which blocks unknown hosts :contentReference[oaicite:1]{index=1}
    },
  }
})
