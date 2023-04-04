
const submitBtn = document.getElementById("submit")
const textInput = document.getElementById("text")

submitBtn.addEventListener("click", async function () {
    if (!textInput.value) return;

    const characters = await document.getElementById("text").value.toUpperCase().split("");

    submitBtn.style.borderColor = "#0ad826"    
    textInput.value = ""

    for (let i = 0; i <= characters.length; i++) {
        const params = ({
            from: characters[i-1],
            to: characters[i],
            "color[R]": 50,
            "color[G]": 0,
            "color[B]": 0,
            animType: 0,
            animSpeed: 120,
            delay: 0,
        });

        let urlSearchParams = new URLSearchParams(params);
        if(i===0) urlSearchParams.append("start", true);
        else if(i===characters.length) urlSearchParams.append("end", true);        
        await request("changePattern", urlSearchParams.toString())
    }

    setTimeout(() => {
        submitBtn.style.borderColor = ""
    }, 1000);
});


textInput.addEventListener('keydown', function (event) {
    if (event.key === "Enter") submitBtn.click();
});