let brightnessLvl = 0.25;
const flashlightBtn = document.getElementById("flashlight-button");

flashlightBtn.addEventListener("click", function () {
    brightnessLvl = brightnessLvl + 0.25
    if (brightnessLvl > 1) brightnessLvl = 0.25
    const args = { brightness: brightnessLvl };
    request("flashlight", args);

    flashlightBtn.innerHTML = 'FLASHLIGHT (' + brightnessLvl*100 + '%)'
});