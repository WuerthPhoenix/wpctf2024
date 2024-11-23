const urlParams = new URLSearchParams(window.location.search);
const token = urlParams.get('token');

fetch(`/api/account/reset?token=${token}`, {
    method: "GET",
    redirect: 'follow',
    cache: "no-cache",
}).then(async (response) => {
    let jsonPayload = await response.json();
    if (response.ok) {
        document.querySelector("[name='username']").value = jsonPayload.name;
    } else {
        showError(jsonPayload.error);
    }
}).catch(() => {
    showError("Error")
});

document.querySelector('#_submit').addEventListener("click", resetPassword);

function showError(error) {
    const errorContainer = document.querySelector(".error");
    errorContainer.innerHTML = error;
    errorContainer.style.display = "block";
}

async function resetPassword() {
    const password = document.querySelector("[name='password']").value;
    const confirmPassword = document.querySelector("[name='confirm-password']").value;
    const data = {
        password,
        confirmPassword,
        token
    };

    const response = await fetch("/api/account/reset", {
        method: "PUT",
        redirect: 'follow',
        cache: "no-cache",
        headers: {
            "Content-Type": "application/json",
        },
        body: JSON.stringify(data)
    });

    let jsonPayload = await response.json();
    if (response.ok) {
        window.location.href = `/`;
    } else {
        showError(jsonPayload.error);
    }
}
