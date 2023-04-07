const canvas = document.getElementById('canvas');
const ctx = canvas.getContext('2d');
const pixelSize = 10;
ctx.fillStyle = 'rgba(0, 0, 0, 0)';
ctx.fillRect(0, 0, canvas.width, canvas.height);
const connectionStatus = document.getElementById('connection')

const audioCtx = new AudioContext();
const analyserNode = audioCtx.createAnalyser();
analyserNode.fftSize = 512;
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

function hexToRgb(hex) {
    let r = parseInt(hex.slice(1, 3), 16);
    let g = parseInt(hex.slice(3, 5), 16);
    let b = parseInt(hex.slice(5, 7), 16);
    return { r, g, b };
}

async function sendPixelDataToArduino() {
    if(!connectionStatus.hasAttribute("Connected")) return;

    analyserNode.getByteFrequencyData(frequencyData);
    
    let colors = [];
    for (let i = 0; i < frequencyData.length; i += 4) {
        let value = (frequencyData[i] + frequencyData[i + 1] + frequencyData[i + 2] + frequencyData[i + 3]) / 4;    
        let hue = value / 255 * 360;
        let saturation_procent = 1;
        let lightness_procent = 0.7;
        let chroma = (1 - Math.abs(2 * lightness_procent - 1)) * saturation_procent;
        let hue_point;
        if (hue < 60) {
        hue_point = chroma;
        } else if (hue < 120) {
        hue_point = chroma * (1 - Math.abs((hue / 60) % 2 - 1));
        } else if (hue < 180) {
        hue_point = 0;
        } else if (hue < 240) {
        hue_point = chroma * (1 - Math.abs((hue / 60) % 2 - 1));
        } else if (hue < 300) {
        hue_point = chroma;
        } else {
        hue_point = chroma * (1 - Math.abs((hue / 60) % 2 - 1));
        }
        let m = lightness_procent - chroma / 2;
        let r = (hue_point + m) * 255;
        let g = (chroma + m) * 255;
        let b = (hue_point - chroma + m) * 255;
        r = Math.round(r);
        g = Math.round(g);
        b = Math.round(b);
        colors.push({ R: r, G: g, B: b });
    }

    let pixelData = [];
    for (let i = 0; i < 8; i++) {
        let row = [];
        for (let j = 0; j < 8; j++) {
            row.push(colors[i * 8 + j]);
        }
        pixelData.push(row);
    }

    let params = new URLSearchParams();
    for (let i = 0; i < colors.length; i++) {
        let color = colors[i];
        params.append(`colors[${i}][R]`, color.R);
        params.append(`colors[${i}][G]`, color.G);
        params.append(`colors[${i}][B]`, color.B);
    }

    let data = params.toString();
    
    await request('LEDs/visualizer', data)
}

function drawVisualization() {
    analyserNode.getByteFrequencyData(frequencyData);

    for (let i = 0; i < frequencyData.length; i++) {
        let value = frequencyData[i];
        let hue = value / 255 * 360;
        let color = `hsl(${hue}, 100%, 50%)`;
        let x = i % 8;
        let y = 7 - Math.floor(i / 8);

        ctx.fillStyle = color;
        ctx.fillRect(x * pixelSize, y * pixelSize, pixelSize, pixelSize);
    }

}


const startBtn = document.getElementById('start-btn');

let arduinoInterval, drawInterval;
startBtn.addEventListener('click', () => {

    if(startBtn.innerHTML === 'START') {
        audioCtx.resume();
    
        startBtn.innerHTML = 'STOP'
        startBtn.style.borderColor = '#00f80c'

        arduinoInterval = setInterval(sendPixelDataToArduino, 500);
        drawInterval = setInterval(drawVisualization, 500);
    }

    else {
        startBtn.innerHTML = 'START'
        startBtn.style.borderColor = ''

        clearInterval(arduinoInterval)
        clearInterval(drawInterval)

        audioCtx.suspend();

        let data = new URLSearchParams({end: true}).toString();
        request('LEDs/visualizer', data)

        ctx.fillStyle = 'rgba(0, 0, 0, 0)';
    }
});