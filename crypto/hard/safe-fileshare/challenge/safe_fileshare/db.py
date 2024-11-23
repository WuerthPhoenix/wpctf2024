import sqlite3
from hashlib import sha256
from .util import dict_factory
from contextlib import contextmanager

db_file = "safe_fileshare.db"

def setup_db():
    db = sqlite3.connect(db_file)
    cursor = db.cursor()
    cursor.execute("CREATE TABLE users(user_id INTEGER PRIMARY KEY ASC, name TEXT UNIQUE, password BLOB) STRICT")
    cursor.execute("""
        CREATE TABLE files(
            download_id TEXT PRIMARY KEY ASC,
            key TEXT,
            user_id INTEGER,
            filename TEXT,
            content BLOB,
            UNIQUE(user_id, filename),
            CONSTRAINT user_id_constraint
                FOREIGN KEY (user_id)
                REFERENCES users(user_id)
        ) STRICT
    """)


@contextmanager
def db_cursor():
    db = sqlite3.connect(db_file)
    db.row_factory = dict_factory
    cursor = db.cursor()
    try:
        yield cursor
        db.commit()
    except Exception as e:
        db.rollback()
        raise e
    finally:
        cursor.close()


def password_hash(password: str):
    hasher = sha256()
    hasher.update(bytes(password, "utf8"))
    return hasher.digest()


def create_user(username: str, password: str):
    with db_cursor() as cursor:
        cursor.execute("INSERT INTO users(name, password) VALUES (?, ?)", (username, password_hash(password)))


def user_exists(username: str):
    with db_cursor() as cursor:
        res = cursor.execute("SELECT * FROM users WHERE name = ?;", [username])
        return res.fetchone() is not None


def get_user(username: str, password: str):
    with db_cursor() as cursor:
        res = cursor.execute("SELECT * FROM users WHERE name = ?;", [username])
        user = res.fetchone()

    if user is None:
        return None
    if user["password"] == password_hash(password):
        del user["password"]
        return user

    return None


def save_file(r: int, s: int, user: int, filename: str, content: str):
    with db_cursor() as cursor:
        cursor.execute("""
            INSERT INTO files(download_id, key, user_id, filename, content)
                VALUES (?, ?, ?, ?, ?)
                ON CONFLICT(user_id, filename) DO UPDATE SET
                    download_id = ?,
                    key = ?
                WHERE filename = ? AND user_id = ?;
            """,
            (f"{s:x}", f"{r:x}", user, filename, content, f"{s:x}", f"{r:x}", filename, user)
        )


def get_user_files(user_id: int):
    with db_cursor() as cursor:
        res = cursor.execute("SELECT filename, download_id, key FROM files WHERE user_id = ?;", [user_id])
        return res.fetchall()


def get_file(download_id, key):
    with db_cursor() as cursor:
        res = cursor.execute("SELECT * FROM files WHERE download_id = ? AND key = ?", (download_id, key))
        return res.fetchone()
