// Here paste announcments
const globalAnnouncment = '<i class="fa-brands fa-searchengin"></i> Backup page - restoring your device'

const announcment = document.getElementById('announcment')
announcment.innerHTML = globalAnnouncment

// Navigator, on show show, on hide hide
const navLinks = document.getElementById('navLinks')
function showMenu() {
  navLinks.style.right = '0'
}
function hideMenu() {
  navLinks.style.right = '-140px'
}

// On click reload
function reload() {
  window.location = window.location
}

// Change anim style
const anims = [
  'sync',
  'circle-notch',
  'spinner',
  'gear',
  'compact-disc',
  'cog'
]
let lastAnim, anim
function changeAnim() {
  while (lastAnim == anim) anim = anims[Math.floor(Math.random() * anims.length)];
  const loading = document.getElementById('loading')
  loading.innerHTML = `<i class="fas fa-${anim}" id="emojiInLoading"></i> Loading`
  const emojiInLoading = document.getElementById('emojiInLoading')
  emojiInLoading.style.animation = 'spinner 0.5s infinite'
  loading.style.animation = 'loading 2s infinite'
  lastAnim = anim
}

// Get document elements
const loading = document.getElementById('loading')
const main = document.getElementById('mainSection')

// Change animation style if it's main page
if (loading.classList.contains('mainPageLoading')) {
  loading.style.lineHeight = '80vh'
  loading.style.height = '100vh'
}

// Run animation
setTimeout(() => {
  changeAnim()
  if (loading.classList.contains('mainPageLoading')) loading.style.animation = 'mainPageLoading 2s infinite'
}, 300);

// Remove loading screen, show content
window.onload = () => {
  loading.style.display = 'none'
  main.style.display = 'block'
}

// Check connection state
const connErrMsg = '<i class="fa-solid fa-arrow-rotate-right fa-spin" style="color: #dc0909"></i> Reconnecting...';
function connectionState() {
  const connectionStatus = document.getElementById('connection')
  const xhttp = new XMLHttpRequest();
  xhttp.open("HEAD", "../functions/connCheck", true);
  xhttp.timeout = 3000;
  xhttp.send();
  function connError() {
    connectionStatus.removeAttribute("Connected")
    if (connectionStatus.innerHTML != connErrMsg) connectionStatus.innerHTML = connErrMsg;
  }

  xhttp.onreadystatechange = function () {
    console.log(this.status)
    if (this.readyState == 4 && this.status == 200) {
      // Connection good
      connectionStatus.setAttribute("Connected", true)
      connectionStatus.innerHTML = '<i class="fa-solid fa-check" style="color: #0cdf4b;"></i> Connected';

      // Connection error
    } else connError();
  };
  xhttp.onerror = () => connError();
  xhttp.ontimeout = () => connError();
}
connectionState()
setInterval(connectionState, 500);

// Request cmd
async function request(cmd, params=null) {
  return await fetch(`../functions/${cmd}`, {
    method: params ? 'POST' : 'GET',
    headers: params ? {'Content-Type': 'application/x-www-form-urlencoded'} : {},
    body: params
  })
}