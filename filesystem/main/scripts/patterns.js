
const submitBtn = document.getElementById("submit")
const textInput = document.getElementById("text")
const optionsBtn = document.getElementById('optionsBtn')
const optionsBox = document.getElementById("optionsBox")

const charactersList = document.getElementById('charList')
const redControl = document.getElementById("red");
const greenControl = document.getElementById("green");
const blueControl = document.getElementById("blue");
const animDir = document.getElementById('direction');
const animSpeed = document.getElementById('speed');
const addDelay = document.getElementById('delay')

const slidersDiv = document.getElementById('slidersDiv')
const animationDirDiv = document.getElementById('dirDiv')

let choosenLetter
let optionsPerAnim = []

textInput.addEventListener('keydown', function (event) {
    if (event.key === "Enter") {
        if(optionsBox.style.height) submitBtn.click();
        else optionsBtn.click();
    }
});

submitBtn.addEventListener("click", async function () {
    if (!textInput.value) return;

    const characters = await document.getElementById("text").value.toUpperCase().split("");
    submitBtn.style.borderColor = "#0ad826"    
    textInput.value = ""
    optionsBtn.style.rotate = ''
    optionsBox.style.height = ''
    addDelay.setAttribute('readonly', true)

    for (let i = 0; i <= characters.length; i++) {
        const params = ({
            from: optionsPerAnim[i].from,
            to: optionsPerAnim[i].to,
            "color[R]": optionsPerAnim[i]["color[R]"],
            "color[G]": optionsPerAnim[i]["color[G]"],
            "color[B]": optionsPerAnim[i]["color[B]"],
            animType: optionsPerAnim[i].animType,
            animSpeed: optionsPerAnim[i].animSpeed,
            delay: optionsPerAnim[i].delay,
        });

        const urlSearchParams = new URLSearchParams(params);
        if(i===0) urlSearchParams.append("start", true);
        else if(i===characters.length) urlSearchParams.append("end", true);        
        await request("LEDs/changePattern", urlSearchParams.toString())
    }

    setTimeout(() => {
        submitBtn.style.borderColor = ""
    }, 1000);
});

textInput.addEventListener('input', function() {
    optionsBtn.style.rotate = ''
    optionsBox.style.height = ''
    addDelay.setAttribute('readonly', true)
    
    if(textInput.value) {
        const characters = document.getElementById("text").value.toUpperCase().split("");
        optionsPerAnim=[]

        for (let i = 0; i <= characters.length; i++) {
            optionsPerAnim.push({
                from: characters[i-1],
                to: characters[i],
                "color[R]": 200,
                "color[G]": 0,
                "color[B]": 0,
                animType: 0,
                animSpeed: 120,
                delay: 0,
            })
        } 
    }
})

function changeLetter(num) {
    choosenLetter = num

    for(let i = 0; i < charactersList.children.length; i++) charactersList.children.item(i).removeAttribute('selected');
    charactersList.children.item(num).setAttribute('selected', true)

    if(charactersList.children.item(num).value == " ") {
        optionsBox.style.height = `240px`
        slidersDiv.style.height = ''
        animationDirDiv.style.marginTop = '0px'
    }
    else {
        optionsBox.style.height = `370px`
        slidersDiv.style.height = `${slidersDiv.scrollHeight}px`
        animationDirDiv.style.marginTop = ''
        redControl.value = optionsPerAnim[num]["color[R]"]
        greenControl.value = optionsPerAnim[num]["color[G]"]
        blueControl.value = optionsPerAnim[num]["color[B]"]
    }
    if(optionsPerAnim[num].animType === 0) animDir.style.rotate=''
    else if(optionsPerAnim[num].animType === 1) animDir.style.rotate='180deg'
    else if(optionsPerAnim[num].animType === 2) animDir.style.rotate='90deg'
    else if(optionsPerAnim[num].animType === 3) animDir.style.rotate='-90deg'
    animSpeed.value = 850-optionsPerAnim[num].animSpeed
    addDelay.value = ''
    addDelay.placeholder = `${(optionsPerAnim[num].delay/1000)} sec`
    addDelay.removeAttribute('readonly')
}

optionsBtn.addEventListener("click", function () {
    if (!textInput.value) return;

    if (!optionsBox.style.height) {
        charactersList.innerHTML=''
        
        for(let i = 0; i < optionsPerAnim.length-1; i++) {
            const option = document.createElement("option");
            option.innerHTML = optionsPerAnim[i].to;
            option.value = optionsPerAnim[i].to
            option.setAttribute('onclick', `changeLetter(${i})`)
            option.classList.add("list-option")
            charactersList.appendChild(option)
        }

        changeLetter(0)
        optionsBtn.style.rotate = '180deg'
    }
    else {
        optionsBtn.style.rotate = ''
        optionsBox.style.height = ''
        addDelay.setAttribute('readonly', true)
    }
});

redControl.addEventListener("input", function () {
    optionsPerAnim[choosenLetter]["color[R]"] = redControl.value
})
greenControl.addEventListener("input", function () {
    optionsPerAnim[choosenLetter]["color[G]"] = greenControl.value
})
blueControl.addEventListener("input", function () {
    optionsPerAnim[choosenLetter]["color[B]"] = blueControl.value
})
animDir.addEventListener('click', function() {
    optionsPerAnim[choosenLetter].animType = optionsPerAnim[choosenLetter].animType+1
    if(optionsPerAnim[choosenLetter].animType>3) optionsPerAnim[choosenLetter].animType = 0

    if(optionsPerAnim[choosenLetter].animType === 0) animDir.style.rotate=''
    else if(optionsPerAnim[choosenLetter].animType === 1) animDir.style.rotate='180deg'
    else if(optionsPerAnim[choosenLetter].animType === 2) animDir.style.rotate='90deg'
    else if(optionsPerAnim[choosenLetter].animType === 3) animDir.style.rotate='-90deg'
})
animSpeed.addEventListener("input", function () {
    optionsPerAnim[choosenLetter].animSpeed = 850-animSpeed.value
})
addDelay.addEventListener("input", function () {
    optionsPerAnim[choosenLetter].delay = addDelay.value*1000
})