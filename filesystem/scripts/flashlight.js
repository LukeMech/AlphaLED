let brightnessLvl = 0;
const flashlightBtn = document.getElementById("flashlight-button");

// Refresh buttons
function refreshFlashlightButton() {
    const xhttp = new XMLHttpRequest();
    xhttp.onreadystatechange = function () {
        if (this.readyState == 4 && this.status == 200) {
            if (this.responseText != "0") flashlightBtn.innerHTML = 'FLASHLIGHT';
            else flashlightBtn.innerHTML = `FLASHLIGHT (${(brightnessLvl*100).toString()}%)`;
        }
    };

    xhttp.open("GET", "../functions/checkLEDs", true);
    xhttp.send();
}

// Flashlight functions
flashlightBtn.addEventListener("click", function () {
    brightnessLvl = brightnessLvl + 0.25
    if (brightnessLvl > 1) brightnessLvl = 0.25
    const params = {
        brightness: brightnessLvl
    };
    const urlSearchParams = new URLSearchParams(params).toString();
    request("flashlight", urlSearchParams)
    refreshFlashlightButton()
});

setInterval(() => {
    refreshFlashlightButton()
}, 500);