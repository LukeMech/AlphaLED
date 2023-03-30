// Get LEDs pattern
function getLEDsPattern() {

  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("patternNum").innerHTML = 'CHANGE (' + this.responseText + ')';
    }
  };
  xhttp.open("GET", "/getledspattern", true);
  xhttp.send();
}
getLEDsPattern()
setInterval(getLEDsPattern, 5000)

// Check version number
async function versionNum() {
    var versionNum = document.getElementById('version')
    try {
      const response = await fetch('version.txt')
      const text = await response.text();
      const lines = text.split('\n');
      for (const line of lines) {
        if (line.startsWith('Server: ')) {
          const version = line.substring('Server: '.length);
          versionNum.innerHTML = versionNum.innerHTML + ` v${version}`
          break;
        }
      }  
    }
    catch (error) {
      versionNum.innerHTML = versionNum.innerHTML + ' Testing env'
      console.error(error)
    }
  }
  versionNum()
  
  // Here paste announcments
  const globalAnnouncment = '<i class="fa-solid fa-wrench"></i> Work in progress'
  
  const announcment = document.getElementById('announcment')
  announcment.innerHTML =  globalAnnouncment
  
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
    { a: 'solid', b: 'sync' },
    { a: 'solid', b: 'circle-notch' },
    { a: 'solid', b: 'spinner' },
    { a: 'solid', b: 'gear' },
    { a: 'solid', b: 'compact-disc' }
  ]
  let lastAnim, anim
  function changeAnim() {
    while(lastAnim == anim) anim = anims[Math.floor(Math.random()*anims.length)];
    const loading = document.getElementById('loading')
    loading.innerHTML = `<i class="fa-${anim.a} fa-${anim.b}" id="emojiInLoading"></i> Loading`
    const emojiInLoading = document.getElementById('emojiInLoading')
    emojiInLoading.style.animation = 'spinner 0.5s infinite'
    loading.style.animation = 'loading 2s infinite'
    lastAnim = anim
  }