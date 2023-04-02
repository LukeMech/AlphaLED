let brightnessLvl = 0;
const flashlightBtn = document.getElementById("flashlight-button");

flashlightBtn.addEventListener("click", function () {
    brightnessLvl = brightnessLvl + 25
    if (brightnessLvl > 100) brightnessLvl = 25
    const args = { brightness: brightnessLvl };
    request("flashlight", args);
    flashlightBtn.innerHTML = 'FLASHLIGHT (' + brightnessLvl.toString() + '%)'
});