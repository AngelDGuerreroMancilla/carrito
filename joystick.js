const canvas = document.getElementById('joystickCanvas');
const ctx = canvas.getContext('2d');
const coordX = document.getElementById('valorX');
const coordY = document.getElementById('valorY');
// Asegúrate de tener referenciado tu botón
// const btnModMan = document.getElementById('btnModMan'); 

// Configuraciones del joystick
const radioBase = canvas.width / 2;
const radioStick = 40;
const centroX = canvas.width / 2;
const centroY = canvas.height / 2;

// Variables de estado
let dibujando = false;
let stickX = centroX;
let stickY = centroY;
let ultimoEnvioMQTT = 0; // Control de tiempo para no saturar el broker

// Función para dibujar el joystick
function dibujar() {
    ctx.clearRect(0, 0, canvas.width, canvas.height);

    // Opcional: Dibujar la base del joystick (círculo exterior gris claro)
    ctx.beginPath();
    ctx.arc(centroX, centroY, radioBase, 0, Math.PI * 2);
    ctx.fillStyle = '#f0f0f0'; 
    ctx.fill();
    ctx.closePath();

    // Dibujar el stick (círculo interior azul)
    ctx.beginPath();
    ctx.arc(stickX, stickY, radioStick, 0, Math.PI * 2);
    ctx.fillStyle = '#3498db';
    ctx.fill();
    ctx.closePath();
}

// Calcular la posición y limitar al borde del círculo
function actualizarPosicion(clientX, clientY) {
    // CONDICIÓN PRINCIPAL: Solo funciona si el modo manual está activado
    if (btnModMan.textContent !== "Activado") return;

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

    // Calcular valores de -500 a 500
    let valX = Math.round((stickX - centroX) / (radioBase - radioStick) * 500);
    let valY = Math.round((stickY - centroY) / (radioBase - radioStick) * -500); 

    coordX.innerText = valX;
    coordY.innerText = valY;

    dibujar();

    // OPTIMIZACIÓN MQTT: Solo enviamos datos cada 100 milisegundos (10 veces por segundo)
    let ahora = Date.now();
    if (ahora - ultimoEnvioMQTT > 100) {
        let envio = `${valX},${valY}`;
        envDatos("joystick", envio);
        console.log(`Enviando: ${envio}`);
        ultimoEnvioMQTT = ahora;
    }
}

// Función para cuando soltamos el joystick (regresa al centro)
function soltar() {
    if (!dibujando) return; // Evitar que se ejecute si ya estaba suelto

    dibujando = false;
    stickX = centroX;
    stickY = centroY;
    coordX.innerText = "0";
    coordY.innerText = "0";
    dibujar();
    
    // Al soltar, enviamos el freno INMEDIATAMENTE (ignoramos el temporizador)
    let envioCeros = "0,0";
    envDatos("joystick", envioCeros);
    console.log(`Freno enviado: ${envioCeros}`);
}

// --- EVENTOS DE MOUSE ---
canvas.addEventListener('mousedown', (e) => { 
    if (btnModMan.textContent === "Activado") {
        dibujando = true;
        actualizarPosicion(e.clientX, e.clientY);
    }
});
canvas.addEventListener('mousemove', (e) => { 
    if (dibujando) actualizarPosicion(e.clientX, e.clientY); 
});
canvas.addEventListener('mouseup', soltar);
canvas.addEventListener('mouseleave', soltar);

// --- EVENTOS TÁCTILES (CELULAR) ---
canvas.addEventListener('touchstart', (e) => { 
    if (btnModMan.textContent === "Activado") {
        dibujando = true; 
        actualizarPosicion(e.touches[0].clientX, e.touches[0].clientY); 
        e.preventDefault(); 
    }
}, {passive: false});
canvas.addEventListener('touchmove', (e) => { 
    if (dibujando) {
        actualizarPosicion(e.touches[0].clientX, e.touches[0].clientY); 
        e.preventDefault(); 
    }
}, {passive: false});
canvas.addEventListener('touchend', soltar);

// Dibujar por primera vez al cargar la página
dibujar();