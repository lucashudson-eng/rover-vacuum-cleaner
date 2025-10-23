import React, { useState, useEffect } from 'react'
import './Section.css'

const Mapa = () => {
  const [roverPosition, setRoverPosition] = useState({ x: 50, y: 50 })
  const [isCleaning, setIsCleaning] = useState(false)
  const [cleanedAreas, setCleanedAreas] = useState([])

  useEffect(() => {
    const interval = setInterval(() => {
      if (isCleaning) {
        setRoverPosition(prev => ({
          x: Math.random() * 100,
          y: Math.random() * 100
        }))
        
        setCleanedAreas(prev => [
          ...prev,
          { x: Math.random() * 100, y: Math.random() * 100, id: Date.now() }
        ])
      }
    }, 2000)

    return () => clearInterval(interval)
  }, [isCleaning])

  const toggleCleaning = () => {
    setIsCleaning(!isCleaning)
  }

  return (
    <section id="mapa" className="section">
      <div className="section-header">
        <h2><span className="section-emoji">🗺️</span> Navigation Map</h2>
      </div>
      
      <div className="mapa-container">
        <div className="mapa-grid">
          {Array.from({ length: 100 }, (_, i) => (
            <div key={i} className="grid-cell"></div>
          ))}
          
          {/* Already cleaned areas */}
          {cleanedAreas.map(area => (
            <div
              key={area.id}
              className="cleaned-area"
              style={{
                left: `${area.x}%`,
                top: `${area.y}%`
              }}
            />
          ))}
          
          {/* Rover */}
          <div
            className={`rover ${isCleaning ? 'cleaning' : ''}`}
            style={{
              left: `${roverPosition.x}%`,
              top: `${roverPosition.y}%`
            }}
          >
            🐢
          </div>
        </div>
      </div>
    </section>
  )
}

export default Mapa
