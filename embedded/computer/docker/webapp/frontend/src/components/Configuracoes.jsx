import React, { useState } from 'react'
import { Settings } from 'lucide-react'
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
    // Aqui você salvaria as configurações no backend
    console.log('Configurações salvas:', settings)
    alert('Configurações salvas com sucesso!')
  }

  return (
    <section id="configuracoes" className="section">
      <div className="section-header">
        <h2><Settings size={24} className="section-icon" /> Configurações</h2>
      </div>
      
      <div className="configuracoes-container">
        {/* Seção vazia - será configurada posteriormente */}
      </div>
    </section>
  )
}

export default Configuracoes
