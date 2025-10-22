import React, { useState, useEffect } from 'react'
import { Map, Camera, Gamepad2, Settings, Menu, X } from 'lucide-react'
import './FixedHeader.css'

const FixedHeader = ({ activeSection, onSectionClick, isMenuOpen, onMenuToggle, onLogoClick }) => {
  const [isVisible, setIsVisible] = useState(false)

  useEffect(() => {
    const handleScroll = () => {
      const heroSection = document.querySelector('.hero-section')
      if (heroSection) {
        const heroRect = heroSection.getBoundingClientRect()
        // Mostrar header quando a seção hero sair da tela
        setIsVisible(heroRect.bottom < 0)
      }
    }

    window.addEventListener('scroll', handleScroll)
    return () => window.removeEventListener('scroll', handleScroll)
  }, [])

  const menuItems = [
    { id: 'mapa', label: 'Mapa', icon: Map },
    { id: 'camera', label: 'Câmera', icon: Camera },
    { id: 'controle', label: 'Controle', icon: Gamepad2 },
    { id: 'configuracoes', label: 'Configurações', icon: Settings }
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
          {/* Menu hambúrguer à esquerda */}
          <button 
            className="header-menu-toggle"
            onClick={toggleMenu}
            aria-label="Abrir menu"
          >
            {isMenuOpen ? <X size={20} /> : <Menu size={20} />}
          </button>

          {/* Logo e título centralizados */}
          <div className="header-title-center" onClick={() => {
            onLogoClick()
            onMenuToggle(false) // Fechar menu ao clicar
          }} style={{ cursor: 'pointer' }}>
            <img src="/android-chrome-192x192.png" alt="RVC" className="header-logo" />
            <span className="header-title">Rover Vacuum Cleaner</span>
          </div>

          {/* Espaço vazio à direita para balancear */}
          <div className="header-spacer"></div>
        </div>
      </header>

      {/* Overlay para fechar o menu */}
      {isMenuOpen && (
        <div 
          className="header-menu-overlay"
          onClick={() => onMenuToggle(false)}
        />
      )}

      {/* Menu dropdown */}
      <div className={`header-menu ${isMenuOpen ? 'open' : ''}`}>
        <nav className="header-menu-nav">
          {menuItems.map((item) => {
            const IconComponent = item.icon
            return (
              <button
                key={item.id}
                className={`header-nav-item ${activeSection === item.id ? 'active' : ''}`}
                onClick={() => handleSectionClick(item.id)}
              >
                <IconComponent size={18} className="header-nav-icon" />
                <span className="header-nav-label">{item.label}</span>
              </button>
            )
          })}
        </nav>
        
        <div className="header-menu-footer">
          <div className="header-status-indicator">
            <div className="header-status-dot online"></div>
            <span>Sistema Online</span>
          </div>
        </div>
      </div>
    </>
  )
}

export default FixedHeader
