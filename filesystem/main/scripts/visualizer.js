const canvas = document.getElementById('canvas');
const ctx = canvas.getContext('2d');
const pixelSize = 10;
ctx.fillStyle = 'rgb(26, 25, 25)';
ctx.fillRect(0, 0, canvas.width, canvas.height);
const connectionStatus = document.getElementById('connection')

const audioCtx = new AudioContext();
const analyserNode = audioCtx.createAnalyser();
analyserNode.fftSize = 128;
const frequencyData = new Uint8Array(analyserNode.frequencyBinCount);
const gainNode = audioCtx.createGain();
gainNode.gain.value = 0;
gainNode.connect(audioCtx.destination);

navigator.mediaDevices.getUserMedia({ audio: true })
    .then(stream => {
        let sourceNode = audioCtx.createMediaStreamSource(stream);
        sourceNode.connect(analyserNode);
        analyserNode.connect(gainNode);
    })
    .catch(error => {
        console.error('Error getting audio', error);
    });

async function sendPixelDataToArduino() {
    if(!connectionStatus.hasAttribute("Connected")) return;

    analyserNode.getByteFrequencyData(frequencyData);
    
    colors = []
    for (let y = 0; y < 8; y++) {
        barHeight = 1.2 * (((frequencyData[i] + frequencyData[i+1] + frequencyData[i+2] + frequencyData[i+3] + frequencyData[i+4] + frequencyData[i+5] + frequencyData[i+6] + frequencyData[i+7]) / 8) / 255)
        const pixelsToTurnOn = barHeight*8
        colors[y] = []
        for (let i = 0; i < 8; i++) {
            if(i > pixelsToTurnOn) colors[y].push({R: 0, G: 0, B: 0});
            colors[y].push({R: Math.round(i*255/7), G: Math.round(7-i*255/7), B: 0});
        }
    }
    
    let params = new URLSearchParams();
    for (let y = 0; y < 8; y++) {
        for (let i = 0; i < 8; i++) {
            let color = colors[y][i];
            params.append(`rows[${y}][${i}][R]`, color.R);
            params.append(`rows[${y}][${i}][G]`, color.G);
            params.append(`rows[${y}][${i}][B]`, color.B);
        }
    }

    let data = params.toString();
    await request('LEDs/visualizer', data)
}

let visualizerStopped = true;
function draw() {
    if(visualizerStopped) {
        ctx.fillStyle = 'rgb(26, 25, 25)';
        ctx.fillRect(0, 0, canvas.width, canvas.height);
        return;
    }
    requestAnimationFrame(draw)
    analyserNode.getByteFrequencyData(frequencyData);
    ctx.fillStyle = "rgb(26, 25, 25)";
    ctx.fillRect(0, 0, canvas.width, canvas.height);
    const barWidth = canvas.width / 8;
    let barHeight;
    for (let i = 0; i < 8; i++) {
        const gradient = ctx.createLinearGradient(0, canvas.height, 0, 0);
        gradient.addColorStop(0, 'green');
        gradient.addColorStop(Math.random() * 0.03 + 0.4, 'yellow');
        gradient.addColorStop(1, 'red');
        barHeight = 1.2* canvas.height * (((frequencyData[i] + frequencyData[i+1] + frequencyData[i+2] + frequencyData[i+3] + frequencyData[i+4] + frequencyData[i+5] + frequencyData[i+6] + frequencyData[i+7]) / 8) / 255)
        ctx.fillStyle = gradient;
        ctx.fillRect(i * barWidth, canvas.height - barHeight, barWidth, barHeight);    
    }
}

const startBtn = document.getElementById('start-btn');
let arduinoInterval, drawInterval;
startBtn.addEventListener('click', () => {

    if(startBtn.innerHTML === 'START') {
        audioCtx.resume();
    
        startBtn.innerHTML = 'STOP'
        startBtn.style.borderColor = '#00f80c'

        visualizerStopped = false;
        draw();
        arduinoInterval = setInterval(sendPixelDataToArduino, 800);
    }

    else {
        startBtn.innerHTML = 'START'
        startBtn.style.borderColor = ''

        clearInterval(arduinoInterval)
        visualizerStopped=true;
        audioCtx.suspend();

        let data = new URLSearchParams({end: true}).toString();
        request('LEDs/visualizer', data)
    }
});