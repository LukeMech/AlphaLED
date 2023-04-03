
const submitBtn = document.getElementById("submit")
const textInput = document.getElementById("text")

submitBtn.addEventListener("click", function () {
    if (!textInput.value) return;

    const characters = document.getElementById("text").value.split("");
    let json = []
    for (let i = 0; i < characters.length; i++) {
        json.push({
            from: characters[i],
            to: characters[i + 1],
            color: {
                R: 50,
                G: 0,
                B: 0
            },
            animType: 2,
            animSpeed: 120,
            delay: 0
        });
    }

    request("changePattern", json, true)

    submitBtn.style.borderColor = "#0ad826"
    textInput.value = ""
    setTimeout(() => {
        submitBtn.style.borderColor = ""
    }, 1000);
});


textInput.addEventListener('keydown', function (event) {
    if (event.key === "Enter") submitBtn.click();
});