const mqtt = require("mqtt");
const client = mqtt.connect("mqtt://test.mosquitto.org");

client.on("connect", () => {
  client.subscribe("TeamRC/#", (err) => {
//     if (!err) {
//       client.publish('TeamRC/#', "prueba 123");
//     }
   });
});

client.on("message", (topic, message) => {
  // message is Buffer
  console.log(topic + " - " + message.toString());
  
 // client.end();
});