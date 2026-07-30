import { createRoot } from 'react-dom/client'
import { Provider } from 'react-redux'
import App from './App.tsx'
import './global.css'
import GlobalWebViewEbent from './event/GlobalWebViewEbent.ts'
import { store } from './store/store.ts'

document.documentElement.classList.add("dark")
GlobalWebViewEbent.start()
createRoot(document.getElementById('root')!).render(
  <Provider store={store}>
    <App />
  </Provider>
)
