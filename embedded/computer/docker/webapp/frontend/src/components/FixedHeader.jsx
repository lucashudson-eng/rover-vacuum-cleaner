import React, { useState, useEffect } from 'react'
import { Menu, X } from 'lucide-react'
import './FixedHeader.css'

const FixedHeader = ({ activeSection, onSectionClick, isMenuOpen, onMenuToggle, onLogoClick }) => {
  const [isVisible, setIsVisible] = useState(false)
  const [roverData, setRoverData] = useState({
    battery: { percentage: 0 },
    power: { current_consumption: 0 }
  })

  // Função para buscar dados do rover
  const fetchRoverData = async () => {
    try {
      const response = await fetch('http://localhost:8000/api/rover/status')
      if (response.ok) {
        const data = await response.json()
        setRoverData(data)
      }
    } catch (error) {
      console.error('Erro ao buscar dados do rover:', error)
    }
  }

  useEffect(() => {
    const handleScroll = () => {
      const heroSection = document.querySelector('.hero-section')
      if (heroSection) {
        const heroRect = heroSection.getBoundingClientRect()
        // Show header when hero section leaves the screen
        setIsVisible(heroRect.bottom < 0)
      }
    }

    window.addEventListener('scroll', handleScroll)
    return () => window.removeEventListener('scroll', handleScroll)
  }, [])

  // Atualiza dados do rover a cada 5 segundos
  useEffect(() => {
    fetchRoverData()
    const interval = setInterval(fetchRoverData, 5000)
    return () => clearInterval(interval)
  }, [])

  const menuItems = [
    { id: 'mapa', label: 'Map', emoji: '🗺️' },
    { id: 'camera', label: 'Camera', emoji: '📷' },
    { id: 'controle', label: 'Control', emoji: '🎮' },
    { id: 'configuracoes', label: 'Settings', emoji: '⚙️' }
  ]

  const handleSectionClick = (sectionId) => {
    onSectionClick(sectionId)
    onMenuToggle(false)
  }

  const toggleMenu = () => {
    onMenuToggle(!isMenuOpen)
  }

  return (
    <>
      {/* Header fixo - só aparece em mobile e quando faz scroll */}
      <header className={`fixed-header ${isVisible ? 'visible' : ''}`}>
        <div className="header-content">
          {/* Hamburger menu on the left */}
          <button 
            className="header-menu-toggle"
            onClick={toggleMenu}
            aria-label="Open menu"
          >
            {isMenuOpen ? <X size={20} /> : <Menu size={20} />}
          </button>

          {/* Logo and title centered */}
          <div className="header-title-center" onClick={() => {
            onLogoClick()
            onMenuToggle(false) // Close menu when clicking
          }} style={{ cursor: 'pointer' }}>
            <img src="/android-chrome-192x192.png" alt="RVC" className="header-logo" />
            <span className="header-title">
              <span className="header-title-full">Rover Vacuum Cleaner</span>
              <span className="header-title-short">RVC</span>
            </span>
          </div>

          {/* Dados do rover alinhados à direita */}
          <div className="header-rover-data">
            <span className="header-rover-icon">🔋</span>
            <span className="header-rover-value">{roverData.battery.percentage}%</span>
            <span className="header-rover-icon">⚡</span>
            <span className="header-rover-value">{roverData.power.current_consumption}A</span>
          </div>
        </div>
      </header>

      {/* Overlay to close menu */}
      {isMenuOpen && (
        <div 
          className="header-menu-overlay"
          onClick={() => onMenuToggle(false)}
        />
      )}

      {/* Dropdown menu */}
      <div className={`header-menu ${isMenuOpen ? 'open' : ''}`}>
        <nav className="header-menu-nav">
          {menuItems.map((item) => {
            return (
              <button
                key={item.id}
                className={`header-nav-item ${activeSection === item.id ? 'active' : ''}`}
                onClick={() => handleSectionClick(item.id)}
              >
                <span className="header-nav-emoji">{item.emoji}</span>
                <span className="header-nav-label">{item.label}</span>
              </button>
            )
          })}
        </nav>
        
        <div className="header-menu-footer">
          <div className="header-menu-rover-data">
            <span className="header-menu-rover-icon">🔋</span>
            <span className="header-menu-rover-value">{roverData.battery.percentage}%</span>
            <span className="header-menu-rover-icon">⚡</span>
            <span className="header-menu-rover-value">{roverData.power.current_consumption}A</span>
          </div>
        </div>
      </div>
    </>
  )
}

export default FixedHeader
