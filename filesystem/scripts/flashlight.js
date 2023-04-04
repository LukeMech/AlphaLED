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
            const flashlightBrightness = response.brightness;
            const flashlightColorR = response.color.R;
            const flashlightColorG = response.color.G;
            const flashlightColorB = response.color.B;

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

    xhttp.open("GET", "../functions/checkFlashlight", true);
    xhttp.send();
}
refreshFlashlight()

// Flashlight functions
function changeBrightnessAndColor() {
    if (!flashlightBtn.hasAttribute("on")) return;
    const params = [{
        brightness: brightnessControl.value,
        color: {
            R: redControl.value,
            G: greenControl.value,
            B: blueControl.value
        }
    }];
    request("flashlight", params)
}

flashlightBtn.addEventListener("click", function () {
    let params
    if (flashlightBtn.hasAttribute("on")) {
        params = [{
            brightness: 0
        }];
    }
    else {
        params = [{
            brightness: brightnessControl.value,
            red: redControl.value,
            green: greenControl.value,
            blue: blueControl.value
        }];
    }
    request("flashlight", params)
    refreshFlashlight()
});

setInterval(() => {
    changeBrightnessAndColor()
}, 1000);