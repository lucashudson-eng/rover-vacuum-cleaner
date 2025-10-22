from flask import Flask, jsonify
from flask_cors import CORS
from datetime import datetime
import os

app = Flask(__name__)
CORS(app)

# Configuração
PORT = int(os.environ.get('PORT', 8000))

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

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=PORT, debug=True)
