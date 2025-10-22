import React, { useState } from 'react'
import { Gamepad2 } from 'lucide-react'
import './Section.css'

const Controle = () => {
  const [speed, setSpeed] = useState(50)
  const [direction, setDirection] = useState('stop')
  const [battery, setBattery] = useState(85)
  const [isConnected, setIsConnected] = useState(true)

  const handleDirectionChange = (newDirection) => {
    setDirection(newDirection)
  }

  const handleSpeedChange = (newSpeed) => {
    setSpeed(newSpeed)
  }

  const emergencyStop = () => {
    setDirection('stop')
    setSpeed(0)
  }

  return (
    <section id="controle" className="section">
      <div className="section-header">
        <h2><Gamepad2 size={24} className="section-icon" /> Controle Manual</h2>
      </div>
      
      <div className="controle-container">
        {/* Seção vazia - será configurada posteriormente */}
      </div>
    </section>
  )
}

export default Controle
