import mqtt from 'mqtt'

const MQTT_CONFIG = {
  host: '2bc93b5857b249d3988054d99e413b9a.s1.eu.hivemq.cloud',
  port: 8883,
  protocol: 'mqtts',
  username: 'esp32',
  password: 'Ltdat2004@'
}

const url = `${MQTT_CONFIG.protocol}://${MQTT_CONFIG.host}:${MQTT_CONFIG.port}`
const client = mqtt.connect(url, {
  username: MQTT_CONFIG.username,
  password: MQTT_CONFIG.password
})

client.on('connect', () => {
  console.log('Simulation connected to HiveMQ')
  setInterval(() => {
    const temp = (20 + Math.random() * 15).toFixed(1)
    const hum = (40 + Math.random() * 40).toFixed(1)
    const light = (100 + Math.random() * 500).toFixed(1)

    client.publish('esp32/nhietdo', temp)
    client.publish('esp32/doam', hum)
    client.publish('esp32/anhsang', light)

    console.log(`Published: T:${temp} H:${hum} L:${light}`)
  }, 3000)
})
