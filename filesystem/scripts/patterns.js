
const submitBtn = document.getElementById("submit")
const textInput = document.getElementById("text")

submitBtn.addEventListener("click", function () {
    if (!textInput.value) return;

    submitBtn.style.borderColor = "#0ad826"
    textInput.value = ""
    
    const characters = document.getElementById("text").value.toUpperCase().split("");
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
            animType: 0,
            animSpeed: 120,
            delay: 0,
            start: i===0 ? true : false,
        });
    }

    json.push({
        to: characters[0],
        color: {
            R: 50,
            G: 0,
            B: 0
        },
        animType: 0,
        animSpeed: 120,
        delay: 0
    })
    
    request("changePattern", json)

    setTimeout(() => {
        submitBtn.style.borderColor = ""
    }, 1000);
});


textInput.addEventListener('keydown', function (event) {
    if (event.key === "Enter") submitBtn.click();
});