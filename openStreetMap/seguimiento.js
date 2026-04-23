const map = L.map("mapa").setView([19.260666, -103.710552],16);
let marcDest= null;
let latDest= null
let lngDest= null;
let btnActGps= document.getElementById("btnActGps");

const iconDest= new L.Icon({
    iconUrl: 'https://raw.githubusercontent.com/pointhi/leaflet-color-markers/master/img/marker-icon-2x-red.png',
    shadowUrl: 'https://cdnjs.cloudflare.com/ajax/libs/leaflet/0.7.7/images/marker-shadow.png',
    iconSize: [25, 41],
    iconAnchor: [12, 41],
    popupAnchor: [1, -34],
    shadowSize: [41, 41]
})
map.on("click", function(e){
    console.log("mapa")
    latDest= e.latlng.lat;
    lngDest= e.latlng.lng;
    document.getElementById("txt-lat-dest").innerHTML= latDest;
    document.getElementById("txt-lng-dest").innerHTML= lngDest;
    
    if(marcDest!==null){
        marcDest.setLatLng(e.latlng);
    }else{
       marcDest = L.marker(e.latlng, { icon: iconDest }).addTo(map);
    }
    if (latDest!==null && lngDest!==null){
        let cord= latDest.toFixed(6) + "," + lngDest.toFixed(6);
        console.log(`coordenadas enviadas ${cord}`)
        envDatos("dest", cord)
    }
})

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


btnActGps.addEventListener("click", ()=>{
    if(btnActGps.textContent== "GPS Activado"){
        envDatos("actGps","0")
        console.log("env gps Desactivado");
        btnActGps.textContent= "GPS Desactivado";
    }else{
        envDatos("actGps","1");
        console.log("env gps Activado");
        btnActGps.textContent= "GPS Activado";
    }
})