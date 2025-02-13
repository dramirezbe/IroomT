import { defineConfig } from 'vite';
import react from '@vitejs/plugin-react-swc';

// https://vite.dev/config/
export default defineConfig({
  plugins: [react()],
  
  // Server configuration
  server: {
    host: true, // Listen on all addresses, including 0.0.0.0
    port: 3000, // Optionally set a specific port
  },

  // Build configuration
  build: {
    rollupOptions: {
      // Customize Rollup options if needed
    },
  },

  // Optimize dependencies with esbuild
  optimizeDeps: {
    esbuildOptions: {
      loader: {
        '.js': 'jsx', // Treat .js files as JSX
      },
    },
  },
});