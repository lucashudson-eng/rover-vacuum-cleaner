import React, { useState, useEffect } from 'react'
import './Sidebar.css'

const Sidebar = ({ activeSection, onSectionClick, isOpen, onLogoClick }) => {
  const [roverData, setRoverData] = useState({
    battery: { percentage: 0 },
    power: { current_consumption: 0 }
  })

  const menuItems = [
    { id: 'mapa', label: 'Map', emoji: '🗺️' },
    { id: 'camera', label: 'Camera', emoji: '📷' },
    { id: 'controle', label: 'Control', emoji: '🎮' },
    { id: 'configuracoes', label: 'Settings', emoji: '⚙️' }
  ]

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

  // Atualiza dados do rover a cada 5 segundos
  useEffect(() => {
    fetchRoverData()
    const interval = setInterval(fetchRoverData, 5000)
    return () => clearInterval(interval)
  }, [])

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
        <div className="rover-data">
          <span className="rover-icon">🔋</span>
          <span className="rover-value">{roverData.battery.percentage}%</span>
          <span className="rover-icon">⚡</span>
          <span className="rover-value">{roverData.power.current_consumption}A</span>
        </div>
      </div>
    </aside>
  )
}

export default Sidebar
