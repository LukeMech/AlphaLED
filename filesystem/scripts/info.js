// Get document elements
const fvVersionDoc = document.getElementById('fvVersion')
const FSVersionDoc = document.getElementById('fsVersion')
const chipIDDoc = document.getElementById('chipID')
const updButton = document.getElementById('updButton')
const connectionStatus = document.getElementById('connection')

// Get system info
function getSystemInfo() {
    const xhttp = new XMLHttpRequest();
    xhttp.onreadystatechange = function () {
        if (this.readyState == 4 && this.status == 200) {
            let lines = this.responseText.split("\n");
            const fsVer = lines[0].replace("Filesystem: ", "").trim();
            const fvVer = lines[1].replace("Firmware: ", "").trim();
            const chipID = lines[2].replace("Chip ID: ", "").trim();

            FSVersionDoc.innerHTML = fsVer;
            fvVersionDoc.innerHTML = fvVer;
            chipIDDoc.innerHTML = chipID;
        }
    };
    xhttp.open("GET", "../functions/getSystemInfo", true);
    xhttp.send();
}
getSystemInfo();

// Call updater
function callUpdater() {
    if (updButton.hasAttribute("updating")) return;
    if (confirm("Call updater? It'll stop the device for a while and then restart it")) {
        const xhttp = new XMLHttpRequest();
        xhttp.open("POST", `/functions/update`);
        xhttp.send();
        updButton.innerHTML = "Checking..."
        updButton.style.borderColor = "#0e3814"
        updButton.setAttribute("updating", true)
        setInterval(() => {
            if (connectionStatus.hasAttribute("Connected")) {
                setTimeout(() => {
                    reload()
                }, 1000);
            }
        }, 100);
    }
}