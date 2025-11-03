import React, { useState } from 'react'
import './Section.css'

const Configuracoes = () => {
  const [settings, setSettings] = useState({
    autoMode: true,
    cleaningIntensity: 'medium',
    scheduleEnabled: false,
    scheduleTime: '09:00',
    soundEnabled: true,
    ledBrightness: 80,
    maxSpeed: 75,
    obstacleDetection: true
  })

  const handleSettingChange = (key, value) => {
    setSettings(prev => ({
      ...prev,
      [key]: value
    }))
  }

  const resetSettings = () => {
    setSettings({
      autoMode: true,
      cleaningIntensity: 'medium',
      scheduleEnabled: false,
      scheduleTime: '09:00',
      soundEnabled: true,
      ledBrightness: 80,
      maxSpeed: 75,
      obstacleDetection: true
    })
  }

  const saveSettings = () => {
    // Here you would save the settings to the backend
    console.log('Settings saved:', settings)
    alert('Settings saved successfully!')
  }

  return (
    <section id="configuracoes" className="section">
      <div className="section-header">
        <h2><span className="section-emoji">⚙️</span> Settings</h2>
      </div>
      
      <div className="configuracoes-container">
        {/* Empty section - will be configured later */}
      </div>
    </section>
  )
}

export default Configuracoes
