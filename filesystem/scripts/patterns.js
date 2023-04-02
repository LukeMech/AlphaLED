
const patternBtn = document.getElementById("pattern-button")

// Refresh buttons
function refreshPatternsButton() {
    const xhttp = new XMLHttpRequest();
    xhttp.onreadystatechange = function () {
        if (this.readyState == 4 && this.status == 200) {
            if (this.responseText != "0") patternBtn.innerHTML = `CHANGE (${this.responseText})`;
            else patternBtn.innerHTML = 'CHANGE';
        }
    };

    xhttp.open("GET", "../functions/checkLEDs", true);
    xhttp.send();
}

// Next pattern
patternBtn.addEventListener("click", function () {
    request('changePattern')
    refreshPatternsButton()
});

setInterval(() => {
    refreshPatternsButton()
}, 500);