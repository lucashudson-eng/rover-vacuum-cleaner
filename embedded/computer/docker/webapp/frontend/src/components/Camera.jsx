import React, { useState, useEffect } from 'react'
import { Camera as CameraIcon } from 'lucide-react'
import './Section.css'

const Camera = () => {
  const [isStreaming, setIsStreaming] = useState(false)
  const [cameraAngle, setCameraAngle] = useState(0)
  const [zoom, setZoom] = useState(1)

  const toggleStream = () => {
    setIsStreaming(!isStreaming)
  }

  const rotateCamera = (direction) => {
    setCameraAngle(prev => prev + (direction === 'left' ? -15 : 15))
  }

  const adjustZoom = (direction) => {
    setZoom(prev => {
      const newZoom = direction === 'in' ? prev * 1.2 : prev / 1.2
      return Math.max(0.5, Math.min(3, newZoom))
    })
  }

  return (
    <section id="camera" className="section">
      <div className="section-header">
        <h2><CameraIcon size={24} className="section-icon" /> Câmera de Visão</h2>
      </div>
      
      <div className="camera-container">
        <div className="camera-view">
          <div className="camera-feed">
            {isStreaming ? (
              <div className="stream-placeholder">
                <div className="stream-content">
                  <div className="rover-view" style={{ transform: `rotate(${cameraAngle}deg) scale(${zoom})` }}>
                    🚗
                  </div>
                  <div className="stream-overlay">
                    <div className="stream-info">
                      <span>Ângulo: {cameraAngle}°</span>
                      <span>Zoom: {zoom.toFixed(1)}x</span>
                    </div>
                  </div>
                </div>
              </div>
            ) : (
              <div className="stream-off">
                <div className="off-icon">📷</div>
                <p>Câmera Desligada</p>
              </div>
            )}
          </div>
        </div>
      </div>
    </section>
  )
}

export default Camera
