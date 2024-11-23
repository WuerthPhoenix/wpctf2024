const sqlite3 = require('sqlite3');
const crypto = require('crypto');
const {PERMISSION} = require("./session");


const DBSOURCE = process.env.DBSOURCE || "db.sqlite"
const FLAG = process.env.FLAG || "flag{test_flag}"


let db = new sqlite3.Database(DBSOURCE, (err) => {
    if (err) {
      // Cannot open database
      console.error(err.message)
      throw err
    }else{
        console.log('Connected to the SQLite database.')
        db.run(`CREATE TABLE user (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            name text UNIQUE,
            surname text, 
            password text,
            enabled boolean,
            permission integer,
            CONSTRAINT name UNIQUE (name)
            )`,
        (err) => {
            if (!err) {
                // Table just created, creating some rows
                const insert = 'INSERT INTO user (name, surname, password, enabled, permission) VALUES (?,?,?,?,?)';
                db.run(insert, ["admin","admin",crypto.randomBytes(30).toString('hex'), 1, PERMISSION.ADMIN])
                db.run(insert, ["Justin", "Case",crypto.randomBytes(30).toString('hex'), 0, PERMISSION.READ_ONLY])
                db.run(insert, ["Sal", "Monella",crypto.randomBytes(30).toString('hex'), 0, PERMISSION.READ_ONLY])
                db.run(insert, ["flag",FLAG,crypto.randomBytes(30).toString('hex'), 0, PERMISSION.READ_ONLY])
                db.run(insert, ["Al", "Beback",crypto.randomBytes(30).toString('hex'), 0, PERMISSION.READ_ONLY])
                db.run(insert, ["Sue", "Permann",crypto.randomBytes(30).toString('hex'), 0, PERMISSION.READ_ONLY])
            }
        });

        db.run(`CREATE TABLE IF NOT EXISTS reset_links (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            name text,
            token text
        )`);

        db.run(`CREATE TABLE IF NOT EXISTS messages (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            name text,
            message text
        )`);
    }
});


module.exports = db

