const canvas = document.getElementById('joystickCanvas'); //obtener el canvas
const ctx = canvas.getContext('2d'); //obtener el contexto para dibujar
const coordsDiv = document.getElementById('coords');  //div para mostrar coordenadas

const radioBase = canvas.width / 2; //radio del circulo base del joystick
const radioStick = 35;  //radio del stick del joystick
const centroX = canvas.width / 2;   //coordenada X del centro del joystick
const centroY = canvas.height / 2;  //coordenada Y del centro del joystick

let activo = false; //indica si el joystick se esta moviendo o no
let stickX = centroX;   //coordenada X actual del stick
let stickY = centroY;   //coordenada Y actual del stick
let ultimoEnvio = 0;    //control de tiempo para evitar saturar el broker MQTT con muchos mensajes

function dibujar() {    //limpiar el canvas y dibujar el joystick
    ctx.clearRect(0, 0, canvas.width, canvas.height);   //limpiar el canvas

    ctx.beginPath(); //dibujar la base del joystick
    ctx.arc(centroX, centroY, radioBase, 0, Math.PI * 2);   //círculo base
    ctx.fillStyle = '#FFE4E8'; //color de fondo del joystick
    ctx.fill();    //esto rellena el círculo base
    ctx.strokeStyle = '#F5A9B8';    //color de borde
    ctx.lineWidth = 2;  //grosor del borde
    ctx.stroke();   //esto dibuja el borde del circulo

    ctx.beginPath();    //esto dibuja el stick del joystick
    ctx.arc(stickX, stickY, radioStick, 0, Math.PI * 2); //círculo del stick
    ctx.fillStyle = activo ? '#FFB0C3' : '#FFC0D0'; //color del stick y se pone más oscuro si está activo
    ctx.fill();  //se rellena el circulo del stick
    ctx.strokeStyle = '#E88DA6'; //color del borde del stick
    ctx.lineWidth = 2; //grosor del borde
    ctx.stroke(); //se dibuja el borde 
}

function mover(e) { //funcion que se llama cuando se mueve el canvas 
    const rect = canvas.getBoundingClientRect();  //se obtienen las medidad del canvas para calcular la posición
    const scaleX = canvas.width / rect.width;   //se escala la posicion de x para que coincida con el tamaño del canvas
    const scaleY = canvas.height / rect.height; // tambien se escala la posicion de y para que coincida con el tamaño del canvas
    
    let x = ((e.clientX || e.touches[0].clientX) - rect.left) * scaleX; //se calcula la posicion de x relativa al canvas y se escala

    let y = ((e.clientY || e.touches[0].clientY) - rect.top) * scaleY;  //se calcula la posicion de y relativa al canvas y se escala
    
    x = Math.min(Math.max(x, 0), canvas.width); //se limita la paosición de x para que no salga del canvas
    y = Math.min(Math.max(y, 0), canvas.height); //se limita la posicion de Y para que no salga del canvas
    
    const dx = x - centroX; //operacion que calcula la distancia de x desde el centro del joystick
    const dy = y - centroY; //lo mismo pero con y
    const dist = Math.hypot(dx, dy);    //operacion que calcula la distancia desde el centro del joystick hasta la posicion actual
    const maxDist = radioBase - radioStick;    //operacion que calcula la distancia maxima que puede moverse el stick dentro del circulo base
    
    if (dist > maxDist) {   //condicion que limita el movimiento del stick para quee no salga del circulo
        const ang = Math.atan2(dy, dx); //operacion que calcula el angulo desde el centro del joystick hasta la posicion actual
        stickX = centroX + maxDist * Math.cos(ang); //
        stickY = centroY + maxDist * Math.sin(ang);
    } else {
        stickX = x;
        stickY = y;
    }
    
    const valX = Math.round(((stickX - centroX) / maxDist) * 500);
    const valY = Math.round(((centroY - stickY) / maxDist) * 500);
    
    coordsDiv.innerText = `X: ${valX} | Y: ${valY}`;
    dibujar();
    
    const ahora = Date.now();
    if (ahora - ultimoEnvio > 300 && typeof envDatos === 'function') {
        envDatos("joystick", `${valX},${valY}`);
        ultimoEnvio = ahora;
    }
}

function soltar() {
    if (!activo) return;
    activo = false;
    
    const startX = stickX, startY = stickY;
    let paso = 0;
    
    const anim = setInterval(() => {
        paso++;
        const p = paso / 10;
        stickX = startX + (centroX - startX) * p;
        stickY = startY + (centroY - startY) * p;
        dibujar();
        
        if (paso >= 10) {
            clearInterval(anim);
            stickX = centroX;
            stickY = centroY;
            coordsDiv.innerText = "X: 0 | Y: 0";
            dibujar();
            if (typeof envDatos === 'function') envDatos("joystick", "0,0");
        }
    }, 10);
}

// Eventos
canvas.onmousedown = (e) => { activo = true; mover(e); };
window.onmousemove = (e) => { if (activo) mover(e); };
window.onmouseup = soltar;

canvas.ontouchstart = (e) => { e.preventDefault(); activo = true; mover(e); };
window.ontouchmove = (e) => { if (activo) { e.preventDefault(); mover(e); } };
window.ontouchend = soltar;

dibujar();