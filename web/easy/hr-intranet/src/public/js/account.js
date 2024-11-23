let user;
fetch("/api/account", {
    method: "GET",
    redirect: 'follow',
    cache: "no-cache",
}).then(async (response) => {
    let jsonPayload = await response.json();
    if (response.ok) {
        user = jsonPayload;
        renderUser(user);
    } else {
        showError(jsonPayload.error);
    }
}).catch(() => {
    showError("Error")
});

fetchMessages();
setInterval(fetchMessages, 5000);

function renderUser(user) {
    document.querySelector("table.account td.username").innerText = user.name;
    document.querySelector("table.account td.permission").innerText = permissionToString(user.permission);
    document.querySelector("button.reset-password").addEventListener("click", resetPassword);
}

function permissionToString(permission) {
    switch(permission) {
        case 0:
            return "Admin";
        case 1:
        default:
            return "Read only";
    }
}

function showError(error) {
    const errorContainer = document.querySelector(".error");
    errorContainer.innerHTML = error;
    errorContainer.style.display = "block";
}

async function resetPassword() {
    const response = await fetch("/api/account/reset", {
        method: "POST",
        redirect: 'follow',
        cache: "no-cache",
        headers: {
            "Content-Type": "application/json",
        },
        body: JSON.stringify(user)
    });

    let jsonPayload = await response.json();
    if (response.ok) {
        showError("A message with a reset link has been sent");
        await fetchMessages();
    } else {
        showError(jsonPayload.error);
    }
}

async function fetchMessages() {
    const response = await fetch(`/api/account/messages`, {
        method: "GET",
        redirect: 'follow',
        cache: "no-cache"
    });

    let jsonPayload = await response.json();
    if (response.ok) {
        renderMessages(jsonPayload.messages);
    } else {
        showError(jsonPayload.error);
    }
}

function renderMessages(messages) {
    const messagesContainer = document.querySelector("table.messages tbody");
    messagesContainer.innerHTML = "";
    for (const message of messages) {
        const row = document.createElement("tr");
        const messageCell = document.createElement("td");
        messageCell.innerHTML = message.message;
        row.appendChild(messageCell);
        messagesContainer.appendChild(row);
    }
}
