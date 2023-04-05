// Get document elements
const fvVersionDoc = document.getElementById('fvVersion')
const FSVersionDoc = document.getElementById('fsVersion')
const chipIDDoc = document.getElementById('chipID')
const updButton = document.getElementById('updButton')
const connectionStatus = document.getElementById('connection')
const branches = document.getElementById("branches")
const updaterSettings = fetch("../updater.json").then(res => res.json())

const loadingversions = '<i class="fa-solid fa-ellipsis fa-bounce"></i>'
const updbuttonhtml = 'Check for updates'

const Q_MARK = "\u2753"             // ❓
const ZZZ = "\ud83d\ude34"          // 😴
const SEARCH = "\ud83d\udd0d"       // 🔍
const ARROW_SPIN = "\ud83d\udd04"   // 🔄️
const MAGIC_WAND = "\ud83e\ude84"   // 🪄
const TOOLS = "\ud83d\udee0"        // 🛠️
const BOLT = "\u26A1"               // ⚡
const EXCLAM_MARK = "\u2757"        // ❗
const WARNING_SIGN = "\u26A0"       // ⚠️

const confirmUpdateText = `${Q_MARK} Call updater? That's the procedure:\n
1. ${ZZZ} Server will turn off, status 'reconnecting' will be displayed\n
2. ${SEARCH} The device will check for updates\n
3. ${ARROW_SPIN} If no updates found, it'll reboot\n
4. ${MAGIC_WAND} If updates found, required files will be downloaded displaying progress on LEDs\n
5. ${TOOLS} Update will be applied on auto reboot\n
6. ${BOLT} Connection will be restored, you will see 'connected' status on top of the screen\n
${EXCLAM_MARK}${WARNING_SIGN} Make sure to NOT turn off the device during update! Refreshing the page is also not recommended, as it'll automatically reconnect to device after its reboot ${WARNING_SIGN}${EXCLAM_MARK}`

// Get system info
let fsVer, fvVer, branchName=0;
async function getSystemInfo() {

    req = await request("getSystemInfo")
    if (req.ok) {
        const text = await req.text()
        const lines = await text.split("\n");
        fsVer = await lines[0].replace(/[^\d.-]/g, "")
        fvVer = await lines[1].replace(/[^\d.-]/g, "")
        const chipID = await lines[2].replace(/[^\d.-]/g, "")

        FSVersionDoc.innerHTML = await fsVer;
        fvVersionDoc.innerHTML = await fvVer;
        chipIDDoc.innerHTML = await chipID;

        if(!branchName) branchName = await updaterSettings.currentBranch;
        const gitRepoName = await updaterSettings.gitRepoName
        const branchesList = await fetch('https://api.github.com/repos/' + await gitRepoName + '/branches').then(res => res.json())

        for(let i = 0; i < await branchesList.length; i++) {

            let displayName
            if(await branchesList[i].name == 'main') displayName = 'stable'
            else displayName = await branchesList[i].name

            const option = document.createElement("option");
            option.text = displayName;
            option.value = await branchesList[i].name
            option.classList.add("branchOption")
            if(await branchesList[i].name == await updaterSettings.currentBranch) option.selected = true

            branches.appendChild(option)
          } 
    }
}
getSystemInfo();

branches.addEventListener("change", function() {
    branchName = branches.value
})

// Call updater
async function callUpdater() {
    if (updButton.hasAttribute("updating")) return;
    if (confirm(confirmUpdateText)) {
        updButton.innerHTML = "Checking..."
        updButton.style.borderColor = "#0e3814"
        updButton.setAttribute("updating", true)
        FSVersionDoc.innerHTML = loadingversions
        fvVersionDoc.innerHTML = loadingversions

        const gitRepoName = await updaterSettings.gitRepoName
        const verCtrl = await updaterSettings.versionFile
        
        const urlToVerCtrl = 'https://raw.githubusercontent.com/' + await gitRepoName + '/' + await branchName + '/' + await verCtrl;
        const req = await fetch(urlToVerCtrl)
        await getSystemInfo();

        let timeout = 0
        if (req.ok) {
            const text = await req.text()
            const lines = text.split("\n");
            const newFsVer = lines[0].replace(/[^\d.-]/g, "")
            const newFvVer = lines[1].replace(/[^\d.-]/g, "")

            fsUrl = await updaterSettings.filesystemFile
            fvUrl = await updaterSettings.firmwareFile
            let urlSearchParams = new URLSearchParams();
            if (newFsVer !== fsVer) {
                urlSearchParams.append("filesystem", 'https://raw.githubusercontent.com/' + await gitRepoName + '/' + await branchName + '/' + await fsUrl)
                FSVersionDoc.innerHTML = newFsVer + ' <i class="fa-solid fa-cloud-arrow-down"></i>'
            }
            else {
                urlSearchParams.append("versions", text)
            }
            
            if (newFvVer !== fvVer) {
                urlSearchParams.append("firmware", 'https://raw.githubusercontent.com/' + await gitRepoName + '/' + await branchName + '/' + await fvUrl)
                fvVersionDoc.innerHTML = newFvVer + ' <i class="fa-solid fa-cloud-arrow-down"></i>'
            }

            if (newFsVer !== fsVer || newFvVer !== fvVer) {
                request("updater/update", urlSearchParams);
                updButton.innerHTML = "Updating, wait for reconnection..."
                timeout = 6000
            }
        }
        
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
        }, timeout);
    }
}