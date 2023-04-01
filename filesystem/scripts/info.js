// Get document elements
let fvVersionDoc = document.getElementById('fvVersion')
let FSVersionDoc = document.getElementById('fsVersion')
let chipIDDoc = document.getElementById('chipID')

let fvVer, fsVer, chipID, xhhtp
async function getSystemInfo() {

    xhttp = new XMLHttpRequest();
    xhttp.onreadystatechange = () => {
        if (this.status == 200) {
            let lines = this.responseText.split("\n");
            fsVer = lines[0].replace("Filesystem: ", "").trim();
            fvVer = lines[1].replace("Firmware: ", "").trim();
            chipID = lines[2].replace("Chip ID: ", "").trim();

            FSVersionDoc.innerHTML = fsVer;
            fvVersionDoc.innerHTML = fvVer;
            chipIDDoc.innerHTML = chipID;
        }
    };
    xhttp.open("GET", "../functions/getSystemInfo", true);
    xhttp.send();
}

getSystemInfo();