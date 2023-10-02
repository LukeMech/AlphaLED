let brightnessLvl = 0;
const flashlightBtn = document.getElementById("toggleFlashlight");
const brightnessControl = document.getElementById("brightness");
const redControl = document.getElementById("red");
const greenControl = document.getElementById("green");
const blueControl = document.getElementById("blue");

// Refresh buttons
async function refreshFlashlight() {
    const req = await request("LEDs/checkFlashlight")
    if(req.ok) {
        const response = new URLSearchParams(await req.text());
        const flashlightBrightness = response.get("brightness");
        const flashlightColorR = parseInt(response.get("color[R]"));
        const flashlightColorG = parseInt(response.get("color[G]"));
        const flashlightColorB = parseInt(response.get("color[B]"));

        if (flashlightBrightness > 0) {
            flashlightBtn.innerHTML = 'TURN OFF';
            flashlightBtn.style.borderColor = "#0ad826"
            flashlightBtn.setAttribute("on", true)
        }
        else {
            flashlightBtn.innerHTML = 'TURN ON';
            flashlightBtn.style.borderColor = ""
            flashlightBtn.removeAttribute("on")
        }

        redControl.value = flashlightColorR
        greenControl.value = flashlightColorG
        blueControl.value = flashlightColorB
    }
};
refreshFlashlight()

// Flashlight functions
function changeBrightnessAndColor() {
    if (!flashlightBtn.hasAttribute("on")) return;
    const params = {
        brightness: brightnessControl.value,
        "color[R]": redControl.value,
        "color[G]": greenControl.value,
        "color[B]": blueControl.value
    };
    const urlSearchParams = new URLSearchParams(params).toString();
    request("LEDs/flashlight", urlSearchParams)
}

flashlightBtn.addEventListener("click", function () {
    let urlSearchParams
    if (flashlightBtn.hasAttribute("on")) {
        const params = {
            brightness: 0
        };
        urlSearchParams = new URLSearchParams(params).toString();
    }
    else {
        const params = {
            brightness: brightnessControl.value,
            "color[R]": redControl.value,
            "color[G]": greenControl.value,
            "color[B]": blueControl.value
        };
        urlSearchParams = new URLSearchParams(params).toString();
    }
    request("LEDs/flashlight", urlSearchParams)
    refreshFlashlight()
});

setInterval(() => {
    changeBrightnessAndColor()
}, 1000);