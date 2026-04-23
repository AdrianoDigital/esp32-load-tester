import { defineConfig } from 'vite'
import react, { reactCompilerPreset } from '@vitejs/plugin-react'
import babel from '@rolldown/plugin-babel'

// https://vite.dev/config/
export default defineConfig({
  plugins: [
    react(),
    babel({ presets: [reactCompilerPreset()] })
  ],
  server: {
    proxy: {
      '/stream': {
        target: 'http://192.168.4.1',
        changeOrigin: true,
      },
      '/api': {
        target: 'http://192.168.4.1',
        changeOrigin: true,
      },
    },
  },
})
