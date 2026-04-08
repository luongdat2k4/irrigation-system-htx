import { reactive } from 'vue'

export const sensorHistory = reactive({
  temperature: [],
  humidity: [],
  light: [],
  timestamps: []
})

const MAX_POINTS = 10

export const addDataPoint = (type, value) => {
  const target = sensorHistory[type]
  if (!target) return

  const now = new Date().toLocaleTimeString([], { hour: '2-digit', minute: '2-digit', second: '2-digit' })

  // Add timestamp only if it's a new time slot or if we want to sync all sensors
  // For simplicity, we'll maintain timestamps separately
  if (sensorHistory.timestamps.length >= MAX_POINTS) {
    sensorHistory.timestamps.shift()
  }
  sensorHistory.timestamps.push(now)

  // Add the value
  if (target.length >= MAX_POINTS) {
    target.shift()
  }
  target.push(value)
}
