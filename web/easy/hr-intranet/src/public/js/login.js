document.querySelector('#_submit').addEventListener("click", () => {
    doLogin()
    .then(async (response) => {
        let jsonPayload = await response.json();
        if (response.ok && jsonPayload.success === true) {
            window.location.href = "users";
        } else {
            showError(jsonPayload.error);
        }
    })
    .catch(() => {
        showError("Error")
     });
});

document.querySelector('.create').addEventListener("click", () => {
    createAccount()
        .then(async (response) => {
            let jsonPayload = await response.json();
            if (response.ok && jsonPayload.success === true) {
                window.location.href = "users";
            } else {
                showError(jsonPayload.error);
            }
        })
        .catch(() => {
            showError("Error")
        });
});

async function doLogin() {
    hideError();

    const data = {
        username: document.querySelector("[name='username']").value,
        password: document.querySelector("[name='password']").value
    };

    return await fetch("/login", {
        method: "POST",
        redirect: 'follow',
        cache: "no-cache",
        headers: {
            "Content-Type": "application/json",
        },
        body: JSON.stringify(data)
    });
}

async function createAccount() {
    hideError();

    const data = {
        username: document.querySelector("[name='username']").value,
        password: document.querySelector("[name='password']").value
    };

    return await fetch("/login", {
        method: "PUT",
        redirect: 'follow',
        cache: "no-cache",
        headers: {
            "Content-Type": "application/json",
        },
        body: JSON.stringify(data)
    });
}

function showError(error) {
    const errorContainer = document.querySelector(".error");
    errorContainer.innerHTML = error;
    errorContainer.style.display = "block";
}

function hideError() {
    const errorContainer = document.querySelector(".error");
    errorContainer.style.display = "none";
}


