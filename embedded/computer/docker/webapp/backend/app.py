from flask import Flask, jsonify, Response
from flask_cors import CORS
from datetime import datetime
import os
import cv2
import threading
import time

app = Flask(__name__)
CORS(app)

# Configuração
PORT = int(os.environ.get('PORT', 8000))

# Configuração da câmera
CAMERA_DEVICE = '/dev/video0'
camera = None
camera_lock = threading.Lock()

def get_camera():
    """Inicializa e retorna a câmera"""
    global camera
    with camera_lock:
        if camera is None:
            try:
                camera = cv2.VideoCapture(CAMERA_DEVICE)
                camera.set(cv2.CAP_PROP_FRAME_WIDTH, 1280)
                camera.set(cv2.CAP_PROP_FRAME_HEIGHT, 720)
                camera.set(cv2.CAP_PROP_FPS, 30)
                print(f"Câmera inicializada: {CAMERA_DEVICE}")
            except Exception as e:
                print(f"Erro ao inicializar câmera: {e}")
                return None
        return camera

def generate_frames():
    """Gera frames da câmera para streaming MJPEG"""
    cam = get_camera()
    if cam is None:
        return
    
    while True:
        try:
            success, frame = cam.read()
            if not success:
                print("Erro ao capturar frame da câmera")
                break
            
            # Redimensiona o frame se necessário
            frame = cv2.resize(frame, (1280, 720))
            
            # Codifica o frame como JPEG
            ret, buffer = cv2.imencode('.jpg', frame, [cv2.IMWRITE_JPEG_QUALITY, 85])
            if not ret:
                continue
            
            # Converte para bytes
            frame_bytes = buffer.tobytes()
            
            # Envia o frame no formato MJPEG
            yield (b'--frame\r\n'
                   b'Content-Type: image/jpeg\r\n\r\n' + frame_bytes + b'\r\n')
            
            # Pequena pausa para controlar FPS
            time.sleep(1/30)
            
        except Exception as e:
            print(f"Erro no streaming: {e}")
            break

@app.route('/')
def home():
    return jsonify({
        'message': 'Backend da aplicação web funcionando!',
        'status': 'OK',
        'timestamp': datetime.now().isoformat()
    })

@app.route('/api/health', methods=['GET'])
def health_check():
    return jsonify({
        'status': 'OK',
        'message': 'Backend funcionando!',
        'timestamp': datetime.now().isoformat(),
        'version': '1.0.0'
    })

@app.route('/api/data', methods=['GET'])
def get_data():
    return jsonify({
        'data': [
            {'id': 1, 'name': 'Item 1', 'description': 'Primeiro item'},
            {'id': 2, 'name': 'Item 2', 'description': 'Segundo item'},
            {'id': 3, 'name': 'Item 3', 'description': 'Terceiro item'}
        ],
        'total': 3
    })

@app.route('/video_feed')
def video_feed():
    """Endpoint para streaming MJPEG da câmera"""
    return Response(generate_frames(),
                    mimetype='multipart/x-mixed-replace; boundary=frame')

@app.route('/api/camera/status', methods=['GET'])
def camera_status():
    """Verifica o status da câmera"""
    cam = get_camera()
    if cam is None:
        return jsonify({
            'status': 'error',
            'message': 'Câmera não disponível',
            'device': CAMERA_DEVICE
        })
    
    # Testa se a câmera está funcionando
    success, frame = cam.read()
    if success:
        return jsonify({
            'status': 'ok',
            'message': 'Câmera funcionando',
            'device': CAMERA_DEVICE,
            'resolution': f"{int(cam.get(cv2.CAP_PROP_FRAME_WIDTH))}x{int(cam.get(cv2.CAP_PROP_FRAME_HEIGHT))}"
        })
    else:
        return jsonify({
            'status': 'error',
            'message': 'Erro ao capturar frame da câmera',
            'device': CAMERA_DEVICE
        })

@app.route('/api/rover/status', methods=['GET'])
def rover_status():
    """Retorna dados do rover incluindo bateria e corrente"""
    # Dados fixos - serão substituídos por dados reais posteriormente
    return jsonify({
        'battery': {
            'percentage': 0
        },
        'power': {
            'current_consumption': 0
        }
    })

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=PORT, debug=True)
