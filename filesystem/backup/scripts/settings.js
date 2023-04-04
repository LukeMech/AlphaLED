// Get document elements
const fvVersionDoc = document.getElementById('fvVersion')
const FSVersionDoc = document.getElementById('fsVersion')
const updButton = document.getElementById('updButton')
const connectionStatus = document.getElementById('connection')

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

// Call updater
async function callUpdater() {
    if (updButton.hasAttribute("updating")) return;
    updButton.innerHTML = "Updating..."
    updButton.style.borderColor = "#0e3814"
    updButton.setAttribute("updating", true)

    const response = await fetch("../updater.json")
    const updaterSettings = await response.json()
    const currentBranch = await updaterSettings.currentBranch
    const verCtrl = await updaterSettings.versionControl
    const versionCtrlUrl = await verCtrl.replace("{branch}", await currentBranch)

    const req = await fetch(await versionCtrlUrl)

    let timeout = 0
    if (req.ok) {
        const text = await req.text()
        const lines = text.split("\n");
        const newFsVer = lines[0].replace("Filesystem: ", "").trim()
        const newFvVer = lines[1].replace("Firmware: ", "").trim()
        fsUrl = await updaterSettings.filesystemUrl
        fvUrl = await updaterSettings.firmwareUrl
        let urlSearchParams = new URLSearchParams();
        urlSearchParams.append("filesystem", await fsUrl.replace("{branch}", await currentBranch))
        FSVersionDoc.innerHTML = newFsVer + ' <i class="fa-solid fa-cloud-arrow-down"></i>'
        urlSearchParams.append("firmware", await fvUrl.replace("{branch}", await currentBranch))
        fvVersionDoc.innerHTML = newFvVer + ' <i class="fa-solid fa-cloud-arrow-down"></i>'

        request("updater/update", urlSearchParams);
    }
    
    setTimeout(() => {
        setInterval(() => {
            if (connectionStatus.hasAttribute("Connected")) {
                setTimeout(() => {
                    window.reload()
                }, 1000);
            }
        }, 100);
    }, 6000);
    
}
callUpdater();