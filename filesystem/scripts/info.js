// Get document elements
let fvVersionDoc = document.getElementById('fvVersion')
let FSVersionDoc = document.getElementById('fsVersion')
let chipIDDoc = document.getElementById('chipID')

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

function callUpdater() {
    const xhttp = new XMLHttpRequest();
  xhttp.open("POST", `/functions/update`);
  xhttp.send();
  setTimeout(() => {
      getLEDsPattern();
  }, 100); 
}