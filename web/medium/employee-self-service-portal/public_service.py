from flask import Flask, request, jsonify, render_template_string
import requests
import re  # <-- Importing regex module to help with filtering

app = Flask(__name__)

# Define a basic, vanilla WAF function to filter suspicious calls
# TO DO: implement a real WAF, this is a temporary solution!
def is_url_blocked(url):
    # Block access to private IP ranges and localhost
    private_ips = [
        r'localhost',           # localhost
        r'127\.\d+\.\d+\.\d+',  # 127.x.x.x
        r'10\.\d+\.\d+\.\d+',   # 10.x.x.x
        r'172\.(1[6-9]|2\d|3[0-1])\.\d+\.\d+',  # 172.16.x.x - 172.31.x.x
        r'192\.168\.\d+\.\d+'   # 192.168.x.x
    ]

    # Block certain protocols such as file://, ftp://, etc.
    blocked_protocols = [
        r'^file://',
        r'^ftp://',
        r'^gopher://',
        r'^dict://',
        r'^ldap://',
        r'^mailto:'
    ]

    # Combine all blocked patterns into a single regex
    blocked_patterns = private_ips + blocked_protocols
    for pattern in blocked_patterns:
        if re.search(pattern, url):
            return True
    return False

@app.route('/fetch', methods=['GET'])
def fetch_url():
    url = request.args.get('url')
    
    # Check if the URL parameter is missing
    if not url:
        return jsonify({"status": "error", "message": "Missing 'url' parameter"})

    try:
        # Check if the URL is blocked by our WAF
        if is_url_blocked(url):
            return jsonify({"status": "error", "message": "WAF identified possible SSRF"})
        
        # Vulnerable to SSRF (if not blocked)
        response = requests.get(url)
        return jsonify({"status": "success", "content": response.text})
    except Exception as e:
        return jsonify({"status": "error", "message": str(e)})

@app.route('/')
def index():
    return render_template_string('''
    <!DOCTYPE html>
    <html lang="en">
    <head>
        <meta charset="UTF-8">
        <meta name="viewport" content="width=device-width, initial-scale=1.0">
        <title>Employee Self Service</title>
        <link rel="stylesheet" type="text/css" href="{{ url_for('static', filename='css/style.css') }}">
    </head>
    <body>
        <div class="jumbotron">
            <div class="container">
                <h1 style="text-align: center;"">
                    Employee Self Service Portal
                </h1>
                <br>
                <h2>Coming Soon</h2>
                <p>We are working for ya!</p>
            </div>
        </div>
    </body>
    </html>
    ''')

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=1042)
