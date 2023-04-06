
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
const outAnim = document.getElementById('addSpaceBtn')

const slidersDiv = document.getElementById('slidersDiv')
const animationDirDiv = document.getElementById('dirDiv')
const outAnimDiv = document.getElementById("addSpaceToggle")

const lastPatternsList = document.getElementById('lastPatterns')

let choosenLetter
let optionsPerAnim = [], patterns = []

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

    for (let i = 0; i < optionsPerAnim.length; i++) {
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
        else if(i===optionsPerAnim.length-1) urlSearchParams.append("filename", characters.join(""));        
        await request("LEDs/changePattern", urlSearchParams.toString())
    }

    setTimeout(() => {
        submitBtn.style.borderColor = ""
        getLastPatterns()
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

    for(let i = 0; i < charactersList.children.length; i++) if(charactersList.children.item(i).classList.contains('selected')) charactersList.children.item(i).classList.remove('selected');
    charactersList.children.item(num).classList.add('selected')

    optionsBox.style.height = `370px`
    charactersList.style.height = '360px'
    slidersDiv.style.height = `${slidersDiv.scrollHeight}px`
    animationDirDiv.style.marginTop = ''
    outAnimDiv.style.height = ''

    if(charactersList.children.item(num).getAttribute("data-value") == "undefined") {
        optionsBox.style.height = `330px`
        slidersDiv.style.height = ''
        outAnimDiv.style.height = `${outAnimDiv.scrollHeight}px`
    }
    else if(charactersList.children.item(num).getAttribute("data-value") == " ") {
        optionsBox.style.height = `240px`
        slidersDiv.style.height = ''
        charactersList.style.height = ''
        animationDirDiv.style.marginTop = '0px'
    }
    else {
        redControl.value = optionsPerAnim[num]["color[R]"]
        greenControl.value = optionsPerAnim[num]["color[G]"]
        blueControl.value = optionsPerAnim[num]["color[B]"]
    }
    if(optionsPerAnim[num].animType === 0) animDir.style.rotate=''
    else if(optionsPerAnim[num].animType === 1) animDir.style.rotate='180deg'
    else if(optionsPerAnim[num].animType === 2) animDir.style.rotate='90deg'
    else if(optionsPerAnim[num].animType === 3) animDir.style.rotate='-90deg'
    animSpeed.value = 550-optionsPerAnim[num].animSpeed
    addDelay.value = ''
    addDelay.placeholder = `${(optionsPerAnim[num].delay/1000)} sec`
    if(num === optionsPerAnim.length-1 && outAnim.innerHTML==='OFF') addDelay.placeholder = ''
    addDelay.removeAttribute('readonly')
}

optionsBtn.addEventListener("click", function () {
    if (!textInput.value) return;

    if (!optionsBox.style.height) {

        charactersList.innerHTML=''
        
        for(let i = 0; i < optionsPerAnim.length; i++) {
            const option = document.createElement("li");
            option.innerHTML = i === optionsPerAnim.length - 1 ? '<i class="fa-solid fa-right-from-bracket"></i>' : optionsPerAnim[i].to;
            option.setAttribute("data-value", optionsPerAnim[i].to)
            option.setAttribute('onclick', `changeLetter(${i})`)
            option.classList.add("list-option")
            if(i === 0) option.classList.add("selected")
            charactersList.appendChild(option)
        }

        outAnim.innerHTML = 'ON'
        outAnim.style.borderColor = ''

        changeLetter(0)
        optionsBtn.style.rotate = '180deg'
    }
    else {
        optionsBtn.style.rotate = ''
        optionsBox.style.height = ''
        addDelay.setAttribute('readonly', true)
    }
});

async function getLastPatterns() {
    const response = await fetch("../functions/LEDs/getSavedPattern")
    const files = await response.text()
    const lines = files.split("\n")

    for (let i = 0; i < lines.length; i++) {
        const filename = lines[i].substring(0, 10)
        if(filename) {
            const res = await fetch(`../functions/LEDs/getSavedPattern?filename=${filename}`)
            let responseJson = await res.json()
            responseJson.unshift(lines[i].substring(12))
            patterns.push(await responseJson)
        }
    }

    lastPatternsList.innerHTML = ''
    for (let i = 0; i < patterns.length; i++) {
        const option = document.createElement("div");
        option.innerHTML = `<div class="lastPatterns"><h1>${patterns[i][0]}</h1><a class="runPattern-btn" onclick="runSavedPattern(${i})"><i class="fa-solid fa-play"></i></a></div>`
        option.classList.add('patternOption')

        lastPatternsList.appendChild(option)
    }
}
getLastPatterns()

function runSavedPattern(num) {
    
    optionsPerAnim = patterns[num].slice(1)
    let textInputValueArray = []
    for(let i = 0; i < optionsPerAnim.length; i++) if(optionsPerAnim[i].to != 'undefined') textInputValueArray.push(optionsPerAnim[i].to)
    
    textInput.value = textInputValueArray.join("")
    optionsBox.style.height = ''
    optionsBtn.click()

    optionsPerAnim = patterns[num].slice(1)
    changeLetter(0)

    if(optionsPerAnim[0].from != 'undefined') {
        outAnim.innerHTML = 'OFF'
        outAnim.style.borderColor = '#f44336' 
    }
}

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
    if(outAnim.innerHTML === 'OFF' && outAnimDiv.style.height) return;
    optionsPerAnim[choosenLetter].animType = optionsPerAnim[choosenLetter].animType+1
    if(optionsPerAnim[choosenLetter].animType>3) optionsPerAnim[choosenLetter].animType = 0

    if(optionsPerAnim[choosenLetter].animType === 0) animDir.style.rotate=''
    else if(optionsPerAnim[choosenLetter].animType === 1) animDir.style.rotate='180deg'
    else if(optionsPerAnim[choosenLetter].animType === 2) animDir.style.rotate='90deg'
    else if(optionsPerAnim[choosenLetter].animType === 3) animDir.style.rotate='-90deg'
})
animSpeed.addEventListener("input", function () {
    if(outAnim.innerHTML === 'OFF' && outAnimDiv.style.height) return animSpeed.value=0
    optionsPerAnim[choosenLetter].animSpeed = 550-animSpeed.value
})
addDelay.addEventListener("input", function () {
    if(outAnim.innerHTML === 'OFF' && outAnimDiv.style.height) return;
    optionsPerAnim[choosenLetter].delay = addDelay.value*1000
})
outAnim.addEventListener("click", function () {
    if(outAnim.innerHTML === 'OFF') {

        outAnim.innerHTML = 'ON'
        outAnim.style.borderColor = ''

        optionsPerAnim[0].from = 'undefined'
        optionsPerAnim.push({
            from: optionsPerAnim[optionsPerAnim.length - 1].to,
            to: "undefined",
            "color[R]": 200,
            "color[G]": 0,
            "color[B]": 0,
            animType: 0,
            animSpeed: 120,
            delay: 0,
        })
        animSpeed.value=550-120;
        addDelay.placeholder='0 sec'
    }
    else {
        outAnim.innerHTML = 'OFF'
        outAnim.style.borderColor = '#f44336'
        optionsPerAnim.pop()
        optionsPerAnim[0].from = optionsPerAnim[optionsPerAnim.length-1].to
        animSpeed.value=0
        animDir.style.rotate=''
        addDelay.placeholder=''
    }
})