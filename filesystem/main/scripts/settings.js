// Get document elements
const fvVersionDoc = document.getElementById('fvVersion')
const FSVersionDoc = document.getElementById('fsVersion')
const chipIDDoc = document.getElementById('chipID')
const updButton = document.getElementById('updButton')
const connectionStatus = document.getElementById('connection')
const branches = document.getElementById("branches")

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
1. ${SEARCH} Your phone will compare local and server versions.\n
2. ${ARROW_SPIN} If no updates found, operation will be aborted.\n
3. ${MAGIC_WAND} If updates found, required files will be downloaded to microcontroller displaying progress on LEDs.\n
5. ${TOOLS} Update will be applied on auto reboot.\n
6. ${BOLT} Connection will be restored, you will see 'connected' status on top of the screen.\n
${EXCLAM_MARK}${WARNING_SIGN} Make sure to NOT turn off the device during update! Refreshing the page is also not recommended, as it'll automatically reconnect to device after its reboot ${WARNING_SIGN}${EXCLAM_MARK}`

// Get system info
let fsVer, fvVer, branchName=0;
async function getSystemInfo() {
    updButton.setAttribute("updating", true)
    
    req = await request("getSystemInfo")

    if(!branches.value) {
        const response = await fetch("../updater.json")
        const updaterSettings = await response.json()

        if(!branchName) branchName = await updaterSettings.currentBranch;
        const gitRepoName = await updaterSettings.gitRepoName

        let branchesListStatus
        try {branchesListStatus = await fetch('https://api.github.com/repos/' + await gitRepoName + '/branches')}
        catch (error) {}

        async function errHandle() {
            const option = document.createElement("option");
            let displayName
            if(await updaterSettings.currentBranch == 'main') displayName = 'stable'
            else displayName = await updaterSettings.currentBranch
            option.text = displayName;
            option.value = await updaterSettings.currentBranch              
            option.classList.add("branchOption")    
            branches.appendChild(option)

            document.getElementById("warn1").classList.remove("hidden")
            document.getElementById("warn2").classList.remove("hidden")
        }

        if(branchesListStatus) {   // Github given branches list
            if(branchesListStatus.ok) {  
                const branchesList = await branchesListStatus.json()
                for(let i = await branchesList.length-1; i >= 0 ; i--) {
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
            } else errHandle(); // Github not responded with branches list
        } 
        else errHandle();  
    }
    
    const text = await req.text()
    const lines = await text.split("\n");
    fsVer = await lines[0].replace("Filesystem: ", "").replace(/[^\d.a-zA-Z-]/g, "")
    fvVer = await lines[1].replace("Firmware: ", "").replace(/[^\d.a-zA-Z-]/g, "")
    const chipID = await lines[2].replace("Chip ID: ", "").replace(/[^\d.a-zA-Z-]/g, "")

    FSVersionDoc.innerHTML = await fsVer;
    fvVersionDoc.innerHTML = await fvVer;
    chipIDDoc.innerHTML = await chipID;

    updButton.innerHTML = "Check for updates"
    updButton.removeAttribute("updating")
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

        const response = await fetch("../updater.json")
        const updaterSettings = await response.json()

        const gitRepoName = await updaterSettings.gitRepoName
        const verCtrl = await updaterSettings.versionFile
        
        const urlToVerCtrl = 'https://raw.githubusercontent.com/' + await gitRepoName + '/' + branchName + '/' + await verCtrl;
        
        let req
        try {req = await fetch(urlToVerCtrl)}
        catch (error) {}

        await getSystemInfo();
        let timeout = 0

        if(req) {
            if (req.ok) {
                const text = await req.text()
                const lines = text.split("\n");
                const newFsVer = lines[0].replace("Filesystem: ", "").replace(/[^\d.a-zA-Z-]/g, "")
                const newFvVer = lines[1].replace("Firmware: ", "").replace(/[^\d.a-zA-Z-]/g, "")
    
                fsUrl = await updaterSettings.filesystemFile
                fvUrl = await updaterSettings.firmwareFile
                let urlSearchParams = new URLSearchParams();
                if (newFsVer !== fsVer) {
                    urlSearchParams.append("filesystem", 'https://raw.githubusercontent.com/' + await gitRepoName + '/' + branchName + '/' + await fsUrl)
                    FSVersionDoc.innerHTML = newFsVer + ' <i class="fa-solid fa-cloud-arrow-down"></i>'
                }
                else {
                    urlSearchParams.append("versions", text)
                }
                
                if (newFvVer !== fvVer) {
                    urlSearchParams.append("firmware", 'https://raw.githubusercontent.com/' + await gitRepoName + '/' + branchName + '/' + await fvUrl)
                    fvVersionDoc.innerHTML = newFvVer + ' <i class="fa-solid fa-cloud-arrow-down"></i>'
                }
    
                if (newFsVer !== fsVer || newFvVer !== fvVer) {
                    request("updater/update", urlSearchParams);
                    updButton.innerHTML = "Updating, wait for reconnection..."
                    timeout = 5000
                }
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