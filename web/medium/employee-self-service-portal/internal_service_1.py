# internal_service_1.py
from flask import Flask

app = Flask(__name__)

@app.route('/')
def index():
    return "Welcome to the printing service"

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=1805)