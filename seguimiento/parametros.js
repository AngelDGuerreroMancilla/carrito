let btnCalib=document.getElementById("btnCalib");
let btnSegLin= document.getElementById("btnSegLin");
let rangoKp = document.getElementById("rangoKp");
let rangoKd = document.getElementById("rangoKd");
let rangoKi = document.getElementById("rangoKi");
let rangoVelBas= document.getElementById("rangoVelBas");
let rangoVelMax= document.getElementById("rangoVelMax");


rangoKp.addEventListener("change", () => {
        envDatos("kp", rangoKp.value.toString());
        console.log(`enviado kp ${rangoKp.value.toString()}`)
    })
    rangoKd.addEventListener("change", () => {
        envDatos("kd", rangoKd.value.toString());
        console.log(`enviado kd ${rangoKd.value.toString()}`)
    })
    rangoKi.addEventListener("change", () => {
        envDatos("ki", rangoKi.value.toString());
        console.log(`enviado ki ${rangoKi.value.toString()}`)
    })

    rangoVelBas.addEventListener("change",()=>{
        envDatos("velBas",rangoVelBas.value.toString());
        console.log(`enviado velBas ${rangoVelBas.value.toString()}`)
    })

    rangoVelMax.addEventListener("change",()=>{
        envDatos("velMax",rangoVelMax.value.toString());
        console.log(`enviado velMax ${rangoVelMax.value.toString()}`)
    })        

    /*btnModMan.addEventListener("click",() =>{
        if(btnModMan.textContent== "Desactivado"){
            btnModMan.innerText="Activado";
            envDatos("modMan", "1");
            console.log("modo Manual activado");
        }else if(btnModMan.textContent=="Activado"){    
            btnModMan.innerText="Desactivado";
            envDatos("modMan", "0");
            console.log("modo manual desactivado")
        }
})*/

btnCalib.addEventListener("click", () =>{
        envDatos("calibracion","1");
        console.log("calibrar env 1");
    
})

/*btnSegLin.addEventListener("click",() =>{

    if(btnSegLin.textContent =="Desactivado"){
        btnSegLin.innerText="Activado";
        envDatos("seguidorLineas", "1");
        console.log("seguidor de lineas ACTIVADO");
    }
    else if(btnSegLin.textContent== "Activado"){
        btnSegLin.innerText="Desactivado";
        envDatos("seguidorLineas", "0");
    console.log("seguidor de lineas DESACTIVADO");
    }
})*/