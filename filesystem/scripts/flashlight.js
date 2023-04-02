let brightnessLvl = 0;
const flashlightBtn = document.getElementById("toggleFlashlight");
const brightnessControl = document.getElementById("brightness");
const redControl = document.getElementById("red");
const greenControl = document.getElementById("green");
const blueControl = document.getElementById("blue");

// Refresh buttons
function refreshFlashlight() {
    const xhttp = new XMLHttpRequest();
    xhttp.onreadystatechange = function () {
        if (this.readyState == 4 && this.status == 200) {
            const response = JSON.parse(this.responseText);
            const flashlightBrightness = response.flashlightBrightness;
            const flashlightColorR = response.flashlightColorR;
            const flashlightColorG = response.flashlightColorG;
            const flashlightColorB = response.flashlightColorB;

            if (flashlightBrightness > 0) {
                flashlightBtn.innerHTML = 'TURN OFF';
                flashlightBtn.style.borderColor = "#0ad826"
                flashlightBtn.setAttribute("on", true)
                brightnessControl.value = flashlightBrightness
                redControl.value = flashlightColorR
                greenControl.value = flashlightColorG
                blueControl.value = flashlightColorB
            }
            else {
                flashlightBtn.innerHTML = 'TURN ON';
                flashlightBtn.style.borderColor = ""
                flashlightBtn.removeAttribute("on")
            }
        }
    };

    xhttp.open("GET", "../functions/checkLEDs", true);
    xhttp.send();
}

function changeBrightness(brightnessLvl) {
    if (!flashlightBtn.hasAttribute("on")) return;
    const params = {
        brightness: brightnessLvl
    };
    const urlSearchParams = new URLSearchParams(params).toString();
    request("flashlight", urlSearchParams)
    refreshFlashlight()
}

function changeColor(value, color) {
    if (!flashlightBtn.hasAttribute("on")) return;
    let params
    if (color == "red")  params = {brightness: brightnessControl.value, red: value};
    else if (color == "green")  params = {brightness: brightnessControl.value, green: value};
    else if (color == "blue")   params = {brightness: brightnessControl.value, blue: value};
    const urlSearchParams = new URLSearchParams(params).toString();
    request("flashlight", urlSearchParams)
    refreshFlashlight()
}


// Flashlight functions
flashlightBtn.addEventListener("click", function () {
    if (flashlightBtn.hasAttribute("on")) {
        const params = {
            brightness: 0
        };
        const urlSearchParams = new URLSearchParams(params).toString();
        flashlightBtn.removeAttribute("on")
        request("flashlight", urlSearchParams)
    }
    else flashlightBtn.setAttribute("on", true)
    refreshFlashlight()
});