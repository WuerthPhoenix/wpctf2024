fetch("/api/users", {
    method: "GET",
    redirect: 'follow',
    cache: "no-cache",
}).then(async (response) => {
    let jsonPayload = await response.json();
    if (response.ok && jsonPayload.success === true) {
        renderUsers(jsonPayload.users);
    } else {
        showError(jsonPayload.error);
    }
}).catch(() => {
    showError("Error")
});

function renderUsers(users) {
    const usersTable = document.querySelector("table.users tbody");
    users.forEach((user) => {
        const tr = document.createElement("tr");

        const id = document.createElement("td");
        id.innerText = user.id;
        tr.appendChild(id);

        const name = document.createElement("td");
        name.innerText = user.name;
        tr.appendChild(name);

        const surname = document.createElement("td");
        surname.innerText = user.surname;
        tr.appendChild(surname);

        const permission = document.createElement("td");
        permission.innerText = permissionToString(user.permission);
        tr.appendChild(permission);

        usersTable.appendChild(tr);
    });
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
