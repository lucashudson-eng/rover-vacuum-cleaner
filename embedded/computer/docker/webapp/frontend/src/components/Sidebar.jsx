import React from 'react'
import { Map, Camera, Gamepad2, Settings } from 'lucide-react'
import './Sidebar.css'

const Sidebar = ({ activeSection, onSectionClick, isOpen, onLogoClick }) => {
  const menuItems = [
    { id: 'mapa', label: 'Mapa', icon: Map },
    { id: 'camera', label: 'Câmera', icon: Camera },
    { id: 'controle', label: 'Controle', icon: Gamepad2 },
    { id: 'configuracoes', label: 'Configurações', icon: Settings }
  ]

  return (
    <aside className={`sidebar ${isOpen ? 'open' : ''}`}>
      <div className="sidebar-header">
        <div className="sidebar-header-content" onClick={onLogoClick} style={{ cursor: 'pointer' }}>
          <img src="/android-chrome-192x192.png" alt="RVC" className="sidebar-app-icon" />
          <h2>RVC</h2>
        </div>
      </div>
      
      <nav className="sidebar-nav">
        {menuItems.map((item) => {
          const IconComponent = item.icon
          return (
            <button
              key={item.id}
              className={`nav-item ${activeSection === item.id ? 'active' : ''}`}
              onClick={() => onSectionClick(item.id)}
            >
              <IconComponent size={20} className="nav-icon" />
              <span className="nav-label">{item.label}</span>
            </button>
          )
        })}
      </nav>
      
      <div className="sidebar-footer">
        <div className="status-indicator">
          <div className="status-dot online"></div>
          <span>Sistema Online</span>
        </div>
      </div>
    </aside>
  )
}

export default Sidebar
