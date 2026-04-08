const map = L.map("mapa").setView([19.260666, -103.710552],16);

L.tileLayer('https://{s}.tile.openstreetmap.org/{z}/{x}/{y}.png', {
    maxZoom: 19,
    attribution: '© OpenStreetMap contributors'
}).addTo(map);

let marcador = L.marker ([19.260666, -103.710552]).addTo(map);

const clientGps = mqtt.connect('ws://broker.hivemq.com:8000/mqtt');
let latRecibida = null;
let lngRecibida = null;

clientGps.on("connect", ()=>{
    console.log("conectado a mqtt para ubicacion");
    document.getElementById("estado").innerText= "esperando GPS..."

    clientGps.subscribe("mi_carrito/web/lat")
    clientGps.subscribe("mi_carrito/web/lng")
    clientGps.subscribe("mi_carrito/web/satel")
    
});
clientGps.on("message",(topic,message)=>{
    let mensaje = parseFloat(message)//el mensaje ya es llega en string siempre
    
    if (topic== "mi_carrito/web/lat"){
        latRecibida= mensaje;
        document.getElementById("txt-lat").innerText=latRecibida;
    }
    if (topic== "mi_carrito/web/lng"){
        lngRecibida= mensaje;
        document.getElementById("txt-lng").innerText=lngRecibida;
    }
    
    if(latRecibida!== null && lngRecibida!== null){
        document.getElementById("estado").innerText="Ubicacion actualizada"
        let nuevaPos= [latRecibida,lngRecibida];
        marcador.setLatLng(nuevaPos);
        map.panTo(nuevaPos)
    }if(topic=="mi_carrito/web/satel"){
        document.getElementById("satel").innerText=mensaje;
    }
})