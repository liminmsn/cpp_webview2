// import { StrictMode } from 'react'
import { createRoot } from 'react-dom/client'
import GlobalWebViewEbent from './event/GlobalWebViewEbent.ts'
import App from './App.tsx'
import './index.css'

document.documentElement.classList.add("dark")
GlobalWebViewEbent.start()
createRoot(document.getElementById('root')!).render(
  <App />
  // <StrictMode>
  // </StrictMode>,
)
