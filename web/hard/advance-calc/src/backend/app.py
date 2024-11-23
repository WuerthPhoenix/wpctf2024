import hashlib
from math import factorial

from flask import Flask, request, render_template, redirect, url_for, g

from arithmetic_eval import evaluate
import time
import re

 
def calculate(expression: str, replace_rule=None):

    def sub(match):
        fn = match.group(1)
        if fn not in replace_rule:
            raise ValueError(f"Function {fn} not found in replace_rule")
        args = match.group(2).split(',') if match.group(2) else []

        return str(replace_rule[fn](*args))
        


    replace_rule = replace_rule or {}
    expression = re.sub(r"\$\{([A-Z_]+)(?:,([A-Za-z0-9]+(?:,[A-Za-z0-9]+)*))?\}", sub, expression)

    try:
        return evaluate(expression), expression
    except Exception as e:
        return -1, expression

def random():
    # blum blum shub
    xn = time.time_ns()
    while True:
        xn = (xn * xn) % (30319 * 32299)
        yield xn


rng = random()

app = Flask(__name__)

calc_sessions = {
}


@app.before_request
def manage_session():
    g.calc_session_id =  request.cookies.get('calc_session',str(next(rng)))
    calc_sessions[g.calc_session_id] = calc_sessions.get(g.calc_session_id, {
        'expressions': {},
        'last_result': 0
    })


@app.after_request
def add_csp(response):

    # https://chatgpt.com/share/672bd659-4d74-800d-a33d-e7c5b842e612
    # we only have remove nonce for laziness, script-src 'self' it's fine. Literally i don't read this csp, i totally trust chatgpt.com
    response.headers['Content-Security-Policy'] = "default-src 'self'; script-src 'self' ; style-src 'self' 'unsafe-inline'; img-src 'self' data: https:; font-src 'self' https:; connect-src 'self' https:; frame-ancestors 'self'; object-src 'none'; base-uri 'self'; form-action 'self';"

    response.set_cookie('calc_session', g.calc_session_id)
    return response


@app.route('/', methods=['GET', 'POST'])
def index():
    if request.method == 'GET':
        return render_template('index.html', calc_session_id=g.calc_session_id, last_result=calc_sessions[g.calc_session_id]['last_result'])
    elif request.method == 'POST':
        expression = request.form['expression']
        
        result,original = calculate(expression, replace_rule={
                'RANDOM': lambda: next(rng),
                'INTDIV': lambda x,y: x // y,
                'INTMOD': lambda x,y: x % y,
                'FACT': lambda x:  factorial(x),
                'RES': lambda: calc_sessions[g.calc_session_id]['last_result']
                }
        )
        
        _id = f"{original}{g.calc_session_id}{str(next(rng))}"
        expression_id = hashlib.sha256(_id.encode()).hexdigest()

        calc_sessions[g.calc_session_id]['expressions'][expression_id] = {
            'original': original,
            'result': result
        }
        calc_sessions[g.calc_session_id]['last_result'] = result
        return redirect(url_for('view', expression_id=expression_id))

        

@app.route("/reset")
def reset():
    g.calc_session_id = str(next(rng))
    calc_sessions[g.calc_session_id] = {
          'expressions': {},
          'last_result': 0
    }
    return redirect(url_for('index'))

@app.route("/view/<string:expression_id>")
def view(expression_id):
    if expression_id not in calc_sessions[g.calc_session_id]['expressions']:
        return "[SYSTEM ERROR] Expression id not found. Retry."
    return render_template('view.html', expression_id=expression_id, calc_session_id=g.calc_session_id)


@app.route("/embed/<string:expression_id>")
def embed(expression_id):    
    if expression_id not in calc_sessions[g.calc_session_id]['expressions']:
        return "[SYSTEM ERROR] Expression id not found. Retry."
    
    return render_template('embed.html', original=calc_sessions[g.calc_session_id]['expressions'][expression_id]['original'], result=calc_sessions[g.calc_session_id]['expressions'][expression_id]['result'])

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5000,debug=True)


