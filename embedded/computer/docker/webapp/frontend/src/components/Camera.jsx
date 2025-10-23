import React, { useState, useEffect } from 'react'
import './Section.css'

const Camera = () => {
  const [isStreaming, setIsStreaming] = useState(false)
  const [cameraAngle, setCameraAngle] = useState(0)
  const [cameraStatus, setCameraStatus] = useState('checking')
  const [error, setError] = useState(null)

  // Check camera status when component loads
  useEffect(() => {
    checkCameraStatus()
  }, [])

  const checkCameraStatus = async () => {
    try {
      const response = await fetch('http://192.168.100.134:8000/api/camera/status')
      const data = await response.json()
      
      if (data.status === 'ok') {
        setCameraStatus('available')
        setError(null)
      } else {
        setCameraStatus('error')
        setError(data.message)
      }
    } catch (err) {
      setCameraStatus('error')
      setError('Error connecting to backend')
    }
  }

  const toggleStream = () => {
    if (cameraStatus === 'available') {
      setIsStreaming(!isStreaming)
    }
  }

  return (
    <section id="camera" className="section">
      <div className="section-header">
        <h2><span className="section-emoji">📷</span> Surveillance Camera</h2>
      </div>
      
      <div className="camera-container">
        <div className="camera-view">
          <div className="camera-feed">
            {cameraStatus === 'checking' ? (
              <div className="stream-off">
                <div className="off-icon">⏳</div>
                <p>Checking camera...</p>
              </div>
            ) : cameraStatus === 'error' ? (
              <div className="stream-off">
                <div className="off-icon">❌</div>
                <p>Error: {error}</p>
                <button onClick={checkCameraStatus} className="retry-btn">
                  Try Again
                </button>
              </div>
            ) : isStreaming ? (
              <div className="stream-container">
                <img 
                  src="http://192.168.100.134:8000/video_feed" 
                  alt="Camera Stream"
                  className="video-stream"
                  style={{ 
                    transform: `rotate(${cameraAngle}deg)`,
                    width: '100%',
                    height: '100%',
                    objectFit: 'contain'
                  }}
                />
              </div>
            ) : (
              <div className="stream-off">
                <div className="off-icon">📷</div>
                <p>Camera Ready</p>
                <p className="camera-info">Device: /dev/video0</p>
              </div>
            )}
          </div>
        </div>
        
        <div className="camera-controls">
          <button 
            onClick={toggleStream}
            className={`stream-btn ${isStreaming ? 'stop' : 'start'}`}
            disabled={cameraStatus !== 'available'}
          >
            {isStreaming ? 'Stop Stream' : 'Start Stream'}
          </button>
        </div>
      </div>
    </section>
  )
}

export default Camera