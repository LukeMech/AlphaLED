// Get document elements
const loading = document.getElementById('loading')
const main = document.getElementById('mainSection')

// Change animation style if it's main page
if(loading.classList.contains('mainPageLoading')) {
    loading.style.lineHeight = '80vh'
    loading.style.height = '100vh'
}

// Run animation
setTimeout(() => {
  changeAnim()
  if (loading.classList.contains('mainPageLoading')) loading.style.animation = 'mainPageLoading 2s infinite'
}, 300);

// Remove loading screen, show content
window.onload = function() {
  setTimeout(() => {
    loading.style.display = 'none'
    main.style.display = 'block'
  }, 600);
}