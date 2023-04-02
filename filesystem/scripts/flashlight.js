let brightnessLvl = 0;
const flashlightBtn = document.getElementById("flashlight-button");

flashlightBtn.addEventListener("click", function () {
    brightnessLvl = brightnessLvl + 25
    if (brightnessLvl > 100) brightnessLvl = 25
    params = "brightness=" + encodeURIComponent(brightnessLvl);
    const xhttp = new XMLHttpRequest();
    xhttp.open("POST", `/functions/flashlight`);
    xhttp.setRequestHeader("Content-type", "application/x-www-form-urlencoded");
    xhttp.send(params);
    flashlightBtn.innerHTML = 'FLASHLIGHT (' + brightnessLvl.toString() + '%)'
});