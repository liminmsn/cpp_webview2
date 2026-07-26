import { createRoot } from 'react-dom/client'
import './global.css'
import { ThemeProvider } from 'next-themes'
import { MysqlProvider } from './context/MysqlContext.tsx'
import LMysql from './page/LMysql.tsx'

createRoot(document.getElementById('root')!).render(
  // <StrictMode>
  //   </StrictMode>,
  <MysqlProvider>
    <ThemeProvider
      attribute="data-theme"
      defaultTheme="cyber-dark"
      enableSystem={false}>
      <LMysql />
    </ThemeProvider>
  </MysqlProvider>
)
