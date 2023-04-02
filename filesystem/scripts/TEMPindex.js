let brightnessLvl = 0;
const flashlightBtn = document.getElementById("flashlight-button");
const patternBtn = document.getElementById("pattern-button")

// Refresh buttons
function refreshButtons() {
    const xhttp = new XMLHttpRequest();
    xhttp.onreadystatechange = function () {
        if (this.readyState == 4 && this.status == 200) {
            if (this.responseText != "0") {
                patternBtn.innerHTML = `CHANGE (${this.responseText})`;
                flashlightBtn.innerHTML = 'FLASHLIGHT';
            }
            else {
                patternBtn.innerHTML = 'CHANGE';
                flashlightBtn.innerHTML = `FLASHLIGHT (${(brightnessLvl*100).toString()}%)`;
            }
        }
    };

    xhttp.open("GET", "../functions/checkLEDs", true);
    xhttp.send();
}

// Next pattern
patternBtn.addEventListener("click", function () {
    request('changePattern')
    refreshButtons()
});

// Flashlight functions
flashlightBtn.addEventListener("click", function () {
    brightnessLvl = brightnessLvl + 0.25
    if (brightnessLvl > 1) brightnessLvl = 0.25
    const params = {
        brightness: brightnessLvl
    };
    const urlSearchParams = new URLSearchParams(params).toString();
    request("flashlight", urlSearchParams)
    refreshButtons()
});

setInterval(() => {
    refreshButtons();
}, 500);