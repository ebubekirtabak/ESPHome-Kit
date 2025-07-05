import { defineConfig } from 'vite';
import react from '@vitejs/plugin-react'
import path from 'path'

// https://vite.dev/config/
export default defineConfig(({ mode }) => ({
  plugins: [react()],
  resolve: {
    alias: {
      '@': path.resolve(__dirname, './src'),
      '@components': path.resolve(__dirname, './src/components'),
      '@utils': path.resolve(__dirname, './src/utils'),
      '@models': path.resolve(__dirname, './src/models'),
      '@assets': path.resolve(__dirname, './src/assets'),
      '@icons': path.resolve(__dirname, './src/icons'),
    },
  },
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
