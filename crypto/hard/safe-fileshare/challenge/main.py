from flask import Flask
from flask import send_from_directory, render_template, request, redirect, session, flash, abort, jsonify, make_response
from random import choice
from string import ascii_letters, digits
import os
from urllib.parse import quote_plus
from sqlite3 import IntegrityError

from safe_fileshare.ec import secp192k1
from safe_fileshare.rng import BlumBlumShub
from safe_fileshare.db import create_user, get_user, save_file, setup_db, get_user_files, get_file, user_exists
from safe_fileshare.secrets import rng_p, rng_q, rng_seed, private_key

chars = ascii_letters + digits
rng = BlumBlumShub(rng_p, rng_q, rng_seed)

app = Flask(__name__)
app.config['TEMPLATES_AUTO_RELOAD'] = True


def save_file_into_db(user: int, username: str, filename: str, content: str):
    total_name = bytes(f"{username}/{filename}", "utf8")
    z = secp192k1.hash_bits(total_name)
    r, s = secp192k1.sign(z, next(rng), private_key)
    save_file(r, s, user, filename, content)
    return f"/files/{s:x}?key={r:x}"


@app.route("/")
@app.route("/index.html")
def home():
    user = session.get("user")
    if user is None:
        return redirect("/auth/login.html")
    files = get_user_files(user["user_id"])
    return render_template("index.html", files=files)


@app.route('/static/<path:path>')
def static_file_server(path):
    return send_from_directory('static', path)


@app.route('/auth/login.html', methods = ["GET", "POST"])
def login_page():
    if request.method == "GET":
        return render_template('login.html')
    elif request.method == "POST":
        # Todo: do login, set session
        name = request.form.get("username")
        password = request.form.get("password")
        user = get_user(name, password)
        if user is None:
            flash("Wrong username or password.")
            return redirect("/auth/login.html")
        session["user"] = user

        return redirect("/index.html")


@app.route('/auth/register.html', methods = ["GET", "POST"])
def register_page():
    if request.method == "GET":
        return render_template('register.html')
    elif request.method == "POST":
        # Todo: do login, set session
        name = request.form.get("username")
        password = request.form.get("password")
        if user_exists(name):
            flash("User exists already")
            return redirect("/auth/register.html")
        
        create_user(name, password)
        session["user"] = get_user(name, password)

        return redirect("/index.html")
    
@app.route('/auth/logout.html')
def logout():
    if session.get("user") is not None:
        del session["user"]
    return redirect("/auth/login.html")


@app.route("/upload", methods = ["POST"])
def upload_files():
    user = session["user"]
    if user is None:
        abort(405)

    urls = []
    for _filename, file_storage in request.files.items():
        filename = file_storage.filename
        content = file_storage.stream.read()
        try:
            url = save_file_into_db(user["user_id"], user['name'], filename, content)
        except IntegrityError as e:
            jsonify({
                "state": "error",
                "message": f"{e}"
            }), 400 
        urls.append(url)
    
    return jsonify({
        "state": "success",
        "download_urls": urls,
    })

@app.route("/files/<download_id>")
def download_file(download_id: str):
    key = request.args.get("key")
    file = get_file(download_id, key)
    if file is None:
        abort(404)

    response = make_response(file["content"])
    filename = quote_plus(file["filename"])
    response.headers["Content-Disposition"] = f"attachment; filename=\"{filename}\""
    return response
    

def random_password(length: int):
    return "".join(choice(chars) for _ in range(length))


if __name__ == "__main__":
    setup_db()
    create_user("admin", random_password(32))
    create_user("intruder", random_password(32))
    create_user("Brisket", random_password(32))
    create_user("Madeline", random_password(32))
    create_user("Badeline", random_password(32))

    save_file_into_db(2, "intruder", "flag.txt", bytes(os.environ["FLAG"], 'utf8'))
    app.secret_key = os.environ["FLASK_SESSION_KEY"]
    app.run(host="0.0.0.0")