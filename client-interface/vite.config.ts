import { defineConfig } from 'vite';
import react from '@vitejs/plugin-react'

// https://vite.dev/config/
export default defineConfig(({ mode }) => ({
  plugins: [react()],
  server: mode === 'development'
    ? {
        proxy: {
          '/api': {
            target: 'http://192.168.1.184', // Arduino NodeMCU IP address from serial output
            changeOrigin: true,
            secure: false,
          }
        }
      }
    : undefined
}))
