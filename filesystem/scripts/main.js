// Get LEDs pattern
async function getLEDsPattern() {

    let xhttp = new XMLHttpRequest();
    xhttp.onreadystatechange = () => {
        if (this.status == 200) {
            if (this.responseText != "0" && this.responseText != "-1" && this.responseText != "-2" && this.responseText != "-3") {
                document.getElementById("patternNum").innerHTML = 'CHANGE (' + this.responseText + ')';
                document.getElementById("flashlight").innerHTML = 'FLASHLIGHT';
            }
            else {
                document.getElementById("patternNum").innerHTML = 'CHANGE';
                let text;
                switch (this.responseText) {
                    case '0': {
                        text = '1'
                        break
                    }
                    case '-1': {
                        text = '2'
                        break
                    }
                    case '-2': {
                        text = '3'
                        break
                    }
                    case '-3': {
                        text = '4 - max'
                        break
                    }
                }
                document.getElementById("flashlight").innerHTML = 'FLASHLIGHT (' + text + ')';
            }
        }
    };
    xhttp.open("GET", "../functions/getLedsPattern", true);
    xhttp.send();
}
getLEDsPattern();
setInterval(getLEDsPattern, 500);

function request(cmd) {
    let xhttp = new XMLHttpRequest();
    xhttp.open("POST", `../functions/${cmd}`);
    xhttp.send();
}