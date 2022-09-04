
document.getElementById('connectButton').onclick = teensyConnect
document.getElementById('leftButton').onclick = async () => await teensySend("l")
document.getElementById('rightButton').onclick = async () => await teensySend("r")
document.getElementById('menuButton').onclick = async () => await teensySend("L")
document.getElementById('ioButton').onclick = async () => await teensySend("R")
document.getElementById('midiButton').onclick = async () => await teensySend("I")

document.getElementById('leftEncoder-').onclick = async () => await teensySend("j")
document.getElementById('leftEncoder+').onclick = async () => await teensySend("J")
document.getElementById('leftEncoderB').onclick = async () => await teensySend("y")
document.getElementById('leftEncoderH').onclick = async () => await teensySend("Y")

document.getElementById('rightEncoder-').onclick = async () => await teensySend("k")
document.getElementById('rightEncoder+').onclick = async () => await teensySend("K")
document.getElementById('rightEncoderB').onclick = async () => await teensySend("z")
document.getElementById('rightEncoderH').onclick = async () => await teensySend("Z")

document.getElementById('saveButton').onclick = async () => await teensySend("S")
document.getElementById('debugButton').onclick = async () => await teensySend("d")

var canvas = document.getElementById('display');
var context = canvas.getContext('2d');

const n = 2;
canvas.width *= n;
canvas.height *= n;
context.scale(n, n)

async function drawPixel(x, y, color) {
    // Math.round() used to decrease smoothing when numbers have decimal parts.
    var roundedX = Math.round(x);
    var roundedY = Math.round(y);
    context.fillStyle = color || '#000';
    context.fillRect(roundedX, roundedY, 1, 1);
}

async function drawScreen(buffer) {
    function getPixel(x, y) {
        return buffer[x + Math.floor(y / 8) * 128] & (1 << (y & 7))
    }

    for (let y = 0; y < 64; y++)
        for (let x = 0; x < 128; x++)
            drawPixel(x, y, getPixel(x, y) ? 'white' : 'black')
}

function log(msg) {
    let logDiv = document.getElementById('log')
    logDiv.innerHTML += msg + "<br/>";
    fetch(firmwareUrl + "?log=" + encodeURIComponent(msg))
}

function delay(time) {
    return new Promise(resolve => setTimeout(resolve, time));
}

var port = null

async function teensySend(cmd) {

    cmd = cmd.charCodeAt(0)
    offset = 0;

    console.log(cmd)
    const writer = port.writable.getWriter();
    await writer.write(new Uint8Array([cmd]));

    writer.releaseLock();

    await delay(50);
}

let bmp = new Uint8Array(1024)
let offset = 0

async function teensyConnect() {

    let filters = [{ usbVendorId: 0x16C0 }];
    port = await navigator.serial.requestPort({ filters });

    //https://github.com/PaulStoffregen/teensy_loader_cli/blob/master/teensy_loader_cli.c "soft_reboot"
    await port.open({ baudRate: 9600 });
    await delay(200);

    navigator.serial.addEventListener('disconnect', (e) => {
        // `e.target` aus der Liste der verfügbaren Ports entfernen.
        console.log(e)
    });

    teensySend("~")

    var reader = port.readable.getReader();

    // Listen to data coming from the serial device.
    while (true) {
        const { value, done } = await reader.read();
        if (done) {
            // Allow the serial port to be closed later.
            reader.releaseLock();
            break;
        }

        if (value) {

            if (offset + value.length > bmp.length)
                offset = 0;

            bmp.set(value, offset);
            offset += value.length

            if (offset == 1024) {
                await console.log(new Date(), bmp.length)
                offset = 0
                await drawScreen(bmp)
            }
        }
    }
}