import React, { useState, useEffect } from 'react'
import Sidebar from './components/Sidebar'
import FixedHeader from './components/FixedHeader'
import GlobalMenuToggle from './components/GlobalMenuToggle'
import Mapa from './components/Mapa'
import Camera from './components/Camera'
import Controle from './components/Controle'
import Configuracoes from './components/Configuracoes'
import './App.css'

function App() {
  const [activeSection, setActiveSection] = useState('mapa')
  const [sidebarOpen, setSidebarOpen] = useState(true) // Começar aberto por padrão
  const [mobileMenuOpen, setMobileMenuOpen] = useState(false)

  useEffect(() => {
    const handleScroll = () => {
      const sections = ['mapa', 'camera', 'controle', 'configuracoes']
      const scrollPosition = window.scrollY + 100

      for (const section of sections) {
        const element = document.getElementById(section)
        if (element) {
          const { offsetTop, offsetHeight } = element
          if (scrollPosition >= offsetTop && scrollPosition < offsetTop + offsetHeight) {
            setActiveSection(section)
            break
          }
        }
      }
    }

    window.addEventListener('scroll', handleScroll)
    return () => window.removeEventListener('scroll', handleScroll)
  }, [])

  const scrollToSection = (sectionId) => {
    const element = document.getElementById(sectionId)
    if (element) {
      element.scrollIntoView({ behavior: 'smooth' })
    }
  }

  const handleSidebarToggle = (isOpen) => {
    setSidebarOpen(isOpen)
  }

  const handleMobileMenuToggle = (isOpen) => {
    setMobileMenuOpen(isOpen)
  }

  const scrollToTop = () => {
    window.scrollTo({ top: 0, behavior: 'smooth' })
  }

  return (
    <div className={`app ${sidebarOpen ? 'sidebar-open' : ''}`}>
      <Sidebar 
        activeSection={activeSection} 
        onSectionClick={scrollToSection}
        isOpen={sidebarOpen}
        onLogoClick={scrollToTop}
      />
      <FixedHeader 
        activeSection={activeSection} 
        onSectionClick={scrollToSection}
        isMenuOpen={mobileMenuOpen}
        onMenuToggle={handleMobileMenuToggle}
        onLogoClick={scrollToTop}
      />
      <main className="main-content">
        <div className="hero-section">
          <GlobalMenuToggle 
            onToggle={handleSidebarToggle} 
            onMobileToggle={handleMobileMenuToggle}
            sidebarOpen={sidebarOpen}
            mobileMenuOpen={mobileMenuOpen}
          />
          <div className="hero-content">
            <img src="/android-chrome-192x192.png" alt="RVC" className="app-icon" />
            <div className="hero-text">
              <h1>Rover Vacuum Cleaner</h1>
              <p>Autonomous robotic rover designed for navigation and task execution, inspired by smart vacuum cleaners</p>
            </div>
          </div>
        </div>
        
        <Mapa />
        <Camera />
        <Controle />
        <Configuracoes />
        
        <footer className="app-footer">
        <div className="footer-content">
        <p>
            © 2025 Lucas Hudson&nbsp;|&nbsp;
            <a 
                href="https://github.com/lucashudson-eng/rover-vacuum-cleaner" 
                target="_blank" 
                rel="noopener noreferrer"
                className="footer-link"
            >
                <svg width="16" height="16" viewBox="0 0 24 24" fill="currentColor" style={{marginRight: '4px', verticalAlign: 'middle'}}>
                <path d="M12 0c-6.626 0-12 5.373-12 12 0 5.302 3.438 9.8 8.207 11.387.599.111.793-.261.793-.577v-2.234c-3.338.726-4.033-1.416-4.033-1.416-.546-1.387-1.333-1.756-1.333-1.756-1.089-.745.083-.729.083-.729 1.205.084 1.839 1.237 1.839 1.237 1.07 1.834 2.807 1.304 3.492.997.107-.775.418-1.305.762-1.604-2.665-.305-5.467-1.334-5.467-5.931 0-1.311.469-2.381 1.236-3.221-.124-.303-.535-1.524.117-3.176 0 0 1.008-.322 3.301 1.23.957-.266 1.983-.399 3.003-.404 1.02.005 2.047.138 3.006.404 2.291-1.552 3.297-1.23 3.297-1.23.653 1.653.242 2.874.118 3.176.77.84 1.235 1.911 1.235 3.221 0 4.609-2.807 5.624-5.479 5.921.43.372.823 1.102.823 2.222v3.293c0 .319.192.694.801.576 4.765-1.589 8.199-6.086 8.199-11.386 0-6.627-5.373-12-12-12z"/>
                </svg>
                GitHub
            </a>
            &nbsp;|&nbsp;
            <a 
                href="https://www.gnu.org/licenses/gpl-3.0.html" 
                target="_blank" 
                rel="noopener noreferrer"
                className="footer-link"
            >
                <svg width="16" height="16" viewBox="0 0 24 24" fill="currentColor" style={{marginRight: '4px', verticalAlign: 'middle'}}>
                <path d="M6 2a2 2 0 0 0-2 2v16a2 
                2 0 0 0 2 2h12a2 2 0 0 0 2-2V8l-6-6H6zm7 
                1.5L18.5 9H13V3.5zM8 13h8v2H8v-2zm0 
                4h8v2H8v-2z"/>
                </svg>
                GPL-3.0
            </a>
            </p>
        </div>
        </footer>
      </main>
    </div>
  )
}

export default App
