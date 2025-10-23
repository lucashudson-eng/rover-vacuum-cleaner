import React from 'react'
import './Sidebar.css'

const Sidebar = ({ activeSection, onSectionClick, isOpen, onLogoClick }) => {
  const menuItems = [
    { id: 'mapa', label: 'Map', emoji: '🗺️' },
    { id: 'camera', label: 'Camera', emoji: '📷' },
    { id: 'controle', label: 'Control', emoji: '🎮' },
    { id: 'configuracoes', label: 'Settings', emoji: '⚙️' }
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
          return (
            <button
              key={item.id}
              className={`nav-item ${activeSection === item.id ? 'active' : ''}`}
              onClick={() => onSectionClick(item.id)}
            >
              <span className="nav-emoji">{item.emoji}</span>
              <span className="nav-label">{item.label}</span>
            </button>
          )
        })}
      </nav>
      
      <div className="sidebar-footer">
        <div className="status-indicator">
          <div className="status-dot online"></div>
          <span>System Online</span>
        </div>
      </div>
    </aside>
  )
}

export default Sidebar
