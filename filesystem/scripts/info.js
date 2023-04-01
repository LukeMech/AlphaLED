// Get document elements
const fvVersionDoc = document.getElementById('fvVersion')
const FSVersionDoc = document.getElementById('fsVersion')
const chipIDDoc = document.getElementById('chipID')

let fvVer, fsVer, chipID
function getSystemInfo() {

    let xhttp = new XMLHttpRequest();
    xhttp.onreadystatechange = () => {
        if (this.readyState == 4 && this.status == 200) {
            let lines = this.responseText.split("\n");
            fsVer = lines[0].replace("Filesystem: ", "").trim();
            fvVer = lines[1].replace("Firmware: ", "").trim();
            chipID = lines[2].replace("Chip ID: ", "").trim();

            FSVersionDoc.innerHTML = fsVer
            fvVersionDoc.innerHTML = fvVer
            chipIDDoc.innerHTML = chipID
        }
    };
    xhttp.open("GET", "../functions/getSystemInfo", true);
    xhttp.send();
}
getSystemInfo();