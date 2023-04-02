
const patternBtn = document.getElementById("pattern-button")

// Refresh buttons
function refreshPatternsButton() {
    const xhttp = new XMLHttpRequest();
    xhttp.onreadystatechange = function () {
        if (this.readyState == 4 && this.status == 200) {
            const response = JSON.parse(this.responseText);
            const pattern = response.pattern;
            if (pattern != 0) patternBtn.innerHTML = `CHANGE (${(pattern+1).toString()})`;
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