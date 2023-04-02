// Get LEDs pattern
function getLEDsPattern() {

    const xhttp = new XMLHttpRequest();
    xhttp.onreadystatechange = function () {
        if (this.readyState == 4 && this.status == 200) {
            if (this.responseText != "0") {
                document.getElementById("patternNum").innerHTML = 'CHANGE (' + this.responseText + ')';
                document.getElementById("flashlight-button").innerHTML = 'FLASHLIGHT';
            }
            else document.getElementById("patternNum").innerHTML = 'CHANGE';
        }
    };
    xhttp.open("GET", "../functions/getLedsPattern", true);
    xhttp.send();
}
getLEDsPattern();
setInterval(getLEDsPattern, 500);