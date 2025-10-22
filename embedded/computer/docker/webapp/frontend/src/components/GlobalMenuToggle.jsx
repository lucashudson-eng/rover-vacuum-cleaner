import React, { useState, useEffect } from 'react'
import { Menu, X } from 'lucide-react'
import './GlobalMenuToggle.css'

const GlobalMenuToggle = ({ onToggle, onMobileToggle, sidebarOpen, mobileMenuOpen }) => {
  const [isMobile, setIsMobile] = useState(false)

  useEffect(() => {
    const checkMobile = () => {
      setIsMobile(window.innerWidth <= 768)
    }
    
    checkMobile()
    window.addEventListener('resize', checkMobile)
    return () => window.removeEventListener('resize', checkMobile)
  }, [])

  const handleToggle = () => {
    if (isMobile) {
      // No mobile, alterna o menu dropdown
      onMobileToggle(!mobileMenuOpen)
    } else {
      // No desktop, alterna a sidebar
      onToggle(!sidebarOpen)
    }
  }

  // Lógica do ícone: Desktop usa estado da sidebar, Mobile usa estado do menu dropdown
  const getIcon = () => {
    if (isMobile) {
      return mobileMenuOpen ? <X size={20} /> : <Menu size={20} /> // Mobile baseado no menu dropdown
    } else {
      return sidebarOpen ? <X size={20} /> : <Menu size={20} /> // Desktop baseado na sidebar
    }
  }

  const getAriaLabel = () => {
    if (isMobile) {
      return mobileMenuOpen ? "Fechar menu" : "Abrir menu"
    } else {
      return sidebarOpen ? "Fechar menu" : "Abrir menu"
    }
  }

  return (
    <button 
      className="global-menu-toggle"
      onClick={handleToggle}
      aria-label={getAriaLabel()}
    >
      {getIcon()}
    </button>
  )
}

export default GlobalMenuToggle
