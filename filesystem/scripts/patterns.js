
const submitBtn = document.getElementById("submit")
const textInput = document.getElementById("text")

submitBtn.addEventListener("click", async function () {
    if (!textInput.value) return;

    submitBtn.style.borderColor = "#0ad826"
    textInput.value = ""
    
    const characters = document.getElementById("text").value.toUpperCase().split("");

    for (let i = 0; i < characters.length; i++) {
        const params = ({
            from: characters[i],
            to: characters[i + 1],
            "color[R]": 50,
            "color[G]": 0,
            "color[B]": 0,
            animType: 0,
            animSpeed: 120,
            delay: 0,
        });

        if(i===0) params.append("start", true)

        const urlSearchParams = new URLSearchParams(params).toString();
        await request("changePattern", urlSearchParams)
    }

    const params = ({
        to: characters[0],
        "color[R]": 50,
        "color[G]": 0,
        "color[B]": 0,
        animType: 0,
        animSpeed: 120,
        delay: 0,
        end: true
    })

    const urlSearchParams = new URLSearchParams(params).toString();
    await request("changePattern", urlSearchParams)

    setTimeout(() => {
        submitBtn.style.borderColor = ""
    }, 1000);
});


textInput.addEventListener('keydown', function (event) {
    if (event.key === "Enter") submitBtn.click();
});