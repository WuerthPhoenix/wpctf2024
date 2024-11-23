from flask import Flask, request, redirect, url_for, session, render_template_string
import subprocess

app = Flask(__name__)
app.secret_key = 'supersecretkey'  # Necessary for session management

# Hardcoded credentials
USERNAME = 'operator'
PASSWORD = 'kevin'

# Before every request, check if the user is logged in, except for the login route
@app.before_request
def require_login():
    if 'logged_in' not in session and request.endpoint not in ['login', 'static']:
        return redirect(url_for('login'))

# Login page
@app.route('/login', methods=['GET', 'POST'])
def login():
    error = ""

    if request.method == 'POST':
        username = request.form['username']
        password = request.form['password']
        if username == USERNAME and password == PASSWORD:
            session['logged_in'] = True
            return redirect(url_for('index'))
        else:
            error = "<div class='error alert alert-danger alert-dismissible text-center'>Invalid credentials. Please try again.</div>"

    return render_template_string(f'''
        <html>
        <head>
            <link rel="stylesheet" type="text/css" href="{{{{ url_for('static', filename='css/style.css') }}}}">
        </head>
        <body>
             <div class="jumbotron">
                <div class="container">
                    <h1 class="max-500px">
                        Operator User Login
                    </h1>
                </div>
            </div>
            <div class="container">
                <div class="row">
                    { error }
                    <div class="col-md-8 col offset-md-2 col-lg-6 offset-lg-3">
                        <form autocomplete="off" class="center bg-form" method="post" action="/login">                    
                            <div class="mb-3">
                                <label class="form-label" for="name">Username</label>
                                <input autofocus="" class="form-control" required id="name" name="username" placeholder=" " required="" type="text" value="">
                            </div>
    
                            <div class="mb-3">
                                <label class="form-label" for="password">Password</label>
                                <input class="form-control" required id="password" name="password" placeholder=" " required="" type="password" value="">
                            </div>
    
                            <div class="row pt-3">
                                <div class="col-6"></div>
    
                                <div class="col-6">
                                    <input class="btn btn-block btn-primary w-100" type="submit" id="_submit" name="_submit" value="Login">
                                </div>
                            </div>
                        </form>
                    </div>
                </div>
            </div>
        </body>
        </html>
    ''')

# Main command executor page, requires login
@app.route('/')
def index():
    return render_template_string('''
        <html>
        <head>
            <link rel="stylesheet" type="text/css" href="{{ url_for('static', filename='css/style.css') }}">
        </head>
        <body>
             <div class="jumbotron">
                <div class="container">
                    <h1 class="max-500px">
                        Welcome Back operator!
                    </h1>
                </div>
            </div>
            
            <div class="container">
                <div class="row">
                    <div class="col-md-8 col offset-md-2 col-lg-6 offset-lg-3">
                    <form autocomplete="off" class="center bg-form" method="post" action="/execute">    
                        <div class="mb-3">
                            <label class="form-label" for="name">Enter command</label>
                            <input autofocus="" class="form-control" required id="name" name="command" placeholder=" " required="" type="text" value="">
                        </div>
                        <div class="row pt-3">
                            <div class="col-6"></div>

                            <div class="col-6">
                                <input class="btn btn-block btn-primary w-100" type="submit" id="_submit" name="_submit" value="Execute">
                            </div>
                        </div>
                    </form>
            </div>
        </body>
        </html>
    ''')

# Execute command route, requires login
@app.route('/execute', methods=['POST'])
def execute():
    command = request.form['command']
    
    try:
        # Use subprocess.run to capture both stdout and stderr
        result = subprocess.run(command, shell=True, capture_output=True, text=True)
        output = result.stdout if result.stdout else result.stderr
    except Exception as e:
        output = f"Error executing command: {str(e)}"

    return render_template_string(f'''
        <html>
        <head>
            <link rel="stylesheet" type="text/css" href="{{{{ url_for('static', filename='css/style.css') }}}}">
        </head>
        <body>
            <div class="jumbotron">
                <div class="container">
                    <h1 class="max-500px">
                        Executed: {command}
                    </h1>
                </div>
            </div>
            <div class="container">
                <div class="row">
                    <div class="col-md-8 col offset-md-2 col-lg-6 offset-lg-3">
                        <div class="output">
                            <pre>{output}</pre>
                        </div>
                        <a href="/">Go Back</a>
                    </div>
                </div>
            </div>
        </body>
        </html>
    ''')

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5023)