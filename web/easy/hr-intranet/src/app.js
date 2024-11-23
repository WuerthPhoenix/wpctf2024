const crypto = require('crypto');
const path = require('path');

const express = require('express');
const md5 = require("md5");
const { v1: uuid } = require('uuid');

const cookieParser = require("cookie-parser");
const bodyParser = require('body-parser');


const session_manager = require('./session.js');
const db = require('./db.js');

let app = express();
app.use(bodyParser.json());
app.use(cookieParser());
app.use(express.static('public'));

function login(username, password) {
    return new Promise((resolve, reject) => {
        // check if user exists and is enabled
        db.all("SELECT * FROM user WHERE name=? AND enabled=1", [username], (err, users)=>{
            if(!users || users.length === 0) {
                reject("No user found");
                return;
            }

            let user = users[0];

            // check if password is correct
            if(!crypto.timingSafeEqual(
                Buffer.from(md5(password)),
                Buffer.from(md5(user.password))
            )){
                reject("Wrong password");
                return;
            }

            // create session user
            const sessionUser = {
                name: user.name,
                surname: user.surname,
                permission: user.permission,
                id: user.id,
            }

            resolve(sessionUser);
        });
    });

}

function createSession(req, res) {
    let {session, id} = session_manager.get(req.cookies.session_id);
    res.cookie('session_id', id);
    req.session = session;
}

app.put('/login', async(req, res) => {

    // check if user exists
    db.all("SELECT * FROM user WHERE name=?", [req.body.username], (err, users)=>{
        if(users && users.length !== 0) {
            res.status(400).json({
                success: false,
                error: "Username already exists"
            });
            return;
        }

        // create user
        db.all("INSERT INTO user(name, password, permission, enabled) VALUES (?, ?, ?, 1)", [req.body.username, req.body.password, session_manager.PERMISSION.READ_ONLY], (err)=>{
            if (err) {
                res.status(500).json({
                    success: false,
                    error: "Error creating user"
                });
                return;
            }

            // login with the new created user
            login(req.body.username, req.body.password).then((sessionUser)=>{
                createSession(req, res);
                req.session.user = sessionUser;
                res.json({
                    success: true
                });
            }).catch((error)=>{
                res.status(401).json({
                    success: false,
                    error: error
                });
            });
        })
    });
});

app.use((req, res, next)=>{
    createSession(req, res);
    next();
})

app.get("/users", (req, res)=>{
    res.sendFile(path.join(__dirname, '/view/users.html'));
})

app.get("/", (req, res)=>{
    res.sendFile(path.join(__dirname, '/view/index.html'));
});

app.post('/login', async(req, res) => {
    login(req.body.username, req.body.password).then((sessionUser)=>{
        req.session.user = sessionUser;
        res.json({
            success: true
        });
    }).catch((error)=>{
        res.status(401).json({
            success: false,
            error: error
        });
    });
});

app.get('/account', async(req, res) => {
    res.sendFile(path.join(__dirname, '/view/account.html'));
});

app.get('/account/reset', async(req, res) => {
    res.sendFile(path.join(__dirname, '/view/reset_password.html'));
});


app.get("/api/users",async (req, res) =>{
    if (!req.session.user) {
        res.status(401).json({
            success: false,
            error: "Unauthorized"
        });
        return;
    }

    if (req.session.user.permission !== session_manager.PERMISSION.ADMIN) {
        db.all("SELECT id, name, surname, permission, '***' as password FROM user WHERE name = ?", [req.session.user.name], (err, users)=>{
            res.json({
                success: true,
                users: users
            });
        });
    } else {
        db.all("SELECT id, name, surname, permission, '***' as password FROM user", (err, users)=>{
            res.json({
                success: true,
                users: users
            });
        });
    }
})

app.get("/api/account",async (req, res) =>{
    if (!req.session.user) {
        res.status(401).json({
            success: false,
            error: "Unauthorized"
        });
        return;
    }

    res.json(req.session.user);
})

app.get("/api/account/reset",async (req, res) => {
    const token = req.query.token;
    if (!token || typeof token !== "string" || token.length !== 36) {
        res.status(400).json({
            success: false,
            error: "Invalid reset link"
        });
        return;
    }

    // Check if token is valid and get the username
    db.all("SELECT name, token FROM reset_links WHERE token=?", [token], (err, links)=>{
        if(links && links.length !== 0) {
            res.json({
                success: true,
                name: links[0].name
            });
        } else {
            res.status(400).json({
                success: false,
                error: "Invalid reset link"
            });
        }
    });
})

app.post("/api/account/reset",async (req, res) =>{
    if (!req.session.user) {
        res.status(401).json({
            success: false,
            error: "Unauthorized"
        });
        return;
    }

    // generate a new token
    const token = uuid();
    db.run("INSERT INTO reset_links(name, token) VALUES (?, ?)", [req.body.name, token], (err)=>{
        if(err){
            res.status(500).json({
                success: false,
                error: "Error generating token"
            });
            return;
        }

        // Generate a message with the reset link
        const message = `Reset your password <a href="/account/reset?token=${token}">here</a>`;
        db.run("INSERT INTO messages(name, message) VALUES (?, ?)", [req.body.name, message], (err)=>{
            if(err){
                res.status(500).json({
                    success: false,
                    error: "Error sending message"
                });
                return;
            }

            res.json({
                success: true
            });
        });
    });
})

app.put("/api/account/reset",async (req, res) =>{
    const token = req.body.token;
    const password = req.body.password;
    const confirmPassword = req.body.confirmPassword;

    // Check if all fields are present
    if (!password || !confirmPassword || !token || token.length !== 36 || typeof password !== "string" || typeof confirmPassword !== "string" || typeof token !== "string") {
        res.status(400).json({
            success: false,
            error: "All fields are required"
        });
        return;
    }

    // Check if password and confirmPassword match
    if (password !== confirmPassword) {
        res.status(400).json({
            success: false,
            error: "Passwords do not match"
        });
        return;
    }

    // Check if token is valid
    db.all("SELECT name, token FROM reset_links WHERE token=?", [token], (err, links)=>{
        if (!links || links.length === 0) {
            res.status(400).json({
                success: false,
                error: "Invalid reset link"
            });
            return;
        }

        // Update password
        db.run("UPDATE user SET password=? WHERE name=?", [password, links[0].name], (err)=>{
            if(err){
                res.status(500).json({
                    success: false,
                    error: "Error updating password"
                });
                return;
            }

            // Delete token
            db.run("DELETE FROM reset_links WHERE token=?", [token], (err)=> {
                if (err) {
                    res.status(500).json({
                        success: false,
                        error: "Error deleting token"
                    });
                    return;
                }

                res.json({
                    success: true
                });

                // Clean messages
                db.run("DELETE FROM messages WHERE name=?", [links[0].name]);
            });
        });
    });
})

app.get("/api/account/messages",async (req, res) =>{
    if (!req.session.user) {
        res.status(401).json({
            success: false,
            error: "Unauthorized"
        });
        return;
    }

    db.all("SELECT name, message FROM messages WHERE name = ?", [req.session.user.name], (err, messages)=>{
        res.json({
            success: true,
            messages: messages
        });
    });
});

app.listen(3000, () => {
    console.log(`Example app listening on port 3000`)
})
