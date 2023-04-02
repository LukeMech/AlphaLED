// Get document elements
const fvVersionDoc = document.getElementById('fvVersion')
const FSVersionDoc = document.getElementById('fsVersion')
const chipIDDoc = document.getElementById('chipID')
const updButton = document.getElementById('updButton')
const connectionStatus = document.getElementById('connection')

const loadingversions='<i class="fa-solid fa-ellipsis fa-bounce"></i>'
const updbuttonhtml='Check for updates'

const confirmUpdateText = "❓ Call updater? That's the procedure:\n1. 😴 Server will turn off, status 'reconnecting will' be displayed\n2. 🔎 The device will check for updates\n3. 🔄️ If no updates found, it'll reboot\n4. 🪄 If updates found, required files will be downloaded displaying progress on LEDs\n5. 🛠️ Update will be applied on auto reboot\n6. ⚡ Connection will be restored, you will see 'connected' status on top of the screen\n❗⚠️ Make sure to NOT turn off the device during update! Refreshing the page is also not recommended, as it'll automatically reconnect to device after its reboot ⚠️❗"

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
    if (confirm(confirmUpdateText)) {
        const xhttp = new XMLHttpRequest();
        xhttp.open("POST", `/functions/update`);
        xhttp.send();
        updButton.innerHTML = "Updating..."
        updButton.style.borderColor = "#0e3814"
        updButton.setAttribute("updating", true)
        FSVersionDoc.innerHTML=loadingversions
        fvVersionDoc.innerHTML=loadingversions
        setTimeout(() => {
            const tempinterval = setInterval(() => {
                if (connectionStatus.hasAttribute("Connected")) {
                    setTimeout(() => {
                        updButton.innerHTML = updbuttonhtml
                        updButton.style.borderColor = ""
                        updButton.removeAttribute("updating")
                        getSystemInfo()
                        clearInterval(tempinterval)
                    }, 1000);
                }
            }, 100);
        }, 2000);
    }
}