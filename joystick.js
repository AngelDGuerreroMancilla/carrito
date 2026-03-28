const canvas = document.getElementById('joystickCanvas');
const ctx = canvas.getContext('2d');
const coordX = document.getElementById('valorX');
const coordY = document.getElementById('valorY');

// Configuraciones del joystick
const radioBase = canvas.width / 2;
const radioStick = 40;
let centroX = canvas.width / 2;
let centroY = canvas.height / 2;
let dibujando = false;

// Coordenadas actuales del stick
let stickX = centroX;
let stickY = centroY;

// Función para dibujar el joystick
function dibujar() {
    ctx.clearRect(0, 0, canvas.width, canvas.height); // Limpiar lienzo

    // Dibujar el stick (círculo interior)
    ctx.beginPath();
    ctx.arc(stickX, stickY, radioStick, 0, Math.PI * 2);
    ctx.fillStyle = '#3498db'; // Color azul
    ctx.fill();
    ctx.closePath();
}

// Calcular la posición y limitar al borde del círculo
function actualizarPosicion(clientX, clientY) {
    const rect = canvas.getBoundingClientRect();
    const x = clientX - rect.left;
    const y = clientY - rect.top;

    // Matemáticas para calcular el ángulo y la distancia
    const dx = x - centroX;
    const dy = y - centroY;
    const distancia = Math.min(Math.hypot(dx, dy), radioBase - radioStick);
    const angulo = Math.atan2(dy, dx);

    // Nuevas coordenadas del stick
    stickX = centroX + distancia * Math.cos(angulo);
    stickY = centroY + distancia * Math.sin(angulo);

    // Calcular valores de -100 a 100 (Invertimos Y para que Arriba sea positivo)
    let valX = Math.round((stickX - centroX) / (radioBase - radioStick) * 100);
    let valY = Math.round((stickY - centroY) / (radioBase - radioStick) * -100); 

    coordX.innerText = valX;
    coordY.innerText = valY;

    dibujar();
    let envio = `${valX.toString()},${valY.toString()}`;
    
    envDatos("joystick", envio )
    
    // AQUÍ ES DONDE ENVIARÍAS POR MQTT
    // Ejemplo: enviarDatosMQTT(valX, valY);
}

// Eventos de Mouse
canvas.addEventListener('mousedown', () => dibujando = true);
canvas.addEventListener('mousemove', (e) => { if (dibujando) actualizarPosicion(e.clientX, e.clientY); });
canvas.addEventListener('mouseup', soltar);
canvas.addEventListener('mouseleave', soltar);

// Eventos de Pantalla Táctil (Celular)
canvas.addEventListener('touchstart', (e) => { dibujando = true; actualizarPosicion(e.touches[0].clientX, e.touches[0].clientY); e.preventDefault(); }, {passive: false});
canvas.addEventListener('touchmove', (e) => { if (dibujando) actualizarPosicion(e.touches[0].clientX, e.touches[0].clientY); e.preventDefault(); }, {passive: false});
canvas.addEventListener('touchend', soltar);

// Función para cuando soltamos el joystick (regresa al centro)
function soltar() {
    dibujando = false;
    stickX = centroX;
    stickY = centroY;
    coordX.innerText = "0";
    coordY.innerText = "0";
    dibujar();

    let envioCeros= "0,0";
    envDatos("joystick", envioCeros );
    // Enviar señal de parar al ESP32
    // Ejemplo: enviarDatosMQTT(0, 0);
}

// Dibujar por primera vez al cargar
dibujar();