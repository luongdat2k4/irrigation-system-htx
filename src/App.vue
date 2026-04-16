<template>
  <div class="min-h-screen text-slate-700 selection:bg-emerald-500/10">
    <!-- Header / Navigation -->
    <header class="sticky top-0 z-50 border-b border-slate-200 bg-white/80 backdrop-blur-md">
      <div class="mx-auto flex max-w-7xl items-center justify-between px-6 py-4">
        <div class="flex items-center gap-3">
          <div class="flex h-10 w-10 items-center justify-center rounded-xl bg-gradient-to-br from-emerald-500 to-emerald-600 shadow-md">
            <Leaf class="h-6 w-6 text-white" />
          </div>
          <div>
            <h1 class="text-xl font-bold tracking-tight text-slate-900 font-heading">Hệ thống tưới tiêu</h1>
            <p class="text-[10px] font-bold uppercase tracking-[0.2em] text-emerald-600">Bảng điều khiển AgriTech</p>
          </div>
        </div>

        <div class="flex items-center gap-6">
          <div class="hidden md:flex items-center gap-2 px-3 py-1.5 rounded-full bg-slate-50 border border-slate-100">
            <div 
              :class="[
                'h-2 w-2 rounded-full transition-all duration-500',
                connectionStatus === 'Connected' ? 'bg-emerald-500 shadow-[0_0_8px_rgba(16,185,129,0.3)]' : 
                connectionStatus === 'Connecting...' ? 'bg-amber-500 animate-pulse' : 'bg-rose-500'
              ]"
            ></div>
            <span class="text-[10px] font-bold uppercase tracking-wider text-slate-500">
              {{ connectionStatus === 'Connected' ? 'Online' : connectionStatus === 'Connecting...' ? 'Connecting' : 'Disconnected' }}
            </span>
          </div>
          <button class="rounded-xl bg-slate-50 border border-slate-100 p-2 text-slate-500 hover:bg-slate-100 transition-colors">
            <Zap class="h-5 w-5" />
          </button>
        </div>
      </div>
    </header>

    <main class="mx-auto max-w-7xl p-6 md:p-8 space-y-8">
      <!-- Connection Status Card -->
      <div v-if="connectionStatus !== 'Connected'" class="rounded-2xl border border-rose-200 bg-rose-50 p-4 flex items-center gap-4 animate-pulse">
        <div class="bg-white p-2 rounded-lg shadow-sm">
          <WifiOff class="h-5 w-5 text-rose-500" />
        </div>
        <div>
          <p class="text-sm font-bold text-rose-700 font-heading">Lỗi kết nối hệ thống</p>
          <p class="text-xs text-rose-600/70">Đang cố gắng kết nối lại với máy chủ MQTT...</p>
        </div>
      </div>

      <!-- Welcome Summary -->
      <section class="grid grid-cols-1 lg:grid-cols-3 gap-8 items-end">
        <div class="lg:col-span-2 space-y-2">
          <h2 class="text-3xl font-bold text-slate-900 font-heading">Chào mừng trở lại!</h2>
          <p class="text-slate-500 max-w-lg">
            Hệ thống tưới của bạn đang được giám sát chặt chẽ. Độ ẩm đất lý tưởng giúp cây trồng phát triển tốt nhất.
          </p>
        </div>
        <div class="flex items-center justify-end gap-3 text-sm font-medium text-slate-400">
          <Calendar class="h-4 w-4" />
          <span>{{ currentTime }}</span>
        </div>
      </section>

      <!-- Main Layout Grid -->
      <div class="grid grid-cols-1 xl:grid-cols-3 gap-8">
        <!-- Left Side: Controls & Sensors -->
        <div class="xl:col-span-1 space-y-8">
          <!-- Control Section -->
          <div class="space-y-4">
            <div class="flex items-center justify-between mb-2">
              <div class="flex items-center gap-2 text-slate-400">
                <ToggleRight class="h-4 w-4" />
                <span class="text-xs font-bold uppercase tracking-widest text-slate-400">Thiết bị điều khiển</span>
              </div>
            </div>
            <div class="grid grid-cols-1 gap-4">
              <DeviceCard
                :model-value="devices.light.isOn"
                @update:model-value="handleDeviceToggle('light', $event)"
                title="Đèn sưởi ấm"
                :icon="Sun"
                on-text="Đang sưởi ấm"
                off-text="Sẵn sàng"
                active-bg-class="bg-orange-500"
              />
              <DeviceCard
                :model-value="devices.irrigation.isOn"
                @update:model-value="handleDeviceToggle('irrigation', $event)"
                title="Máy bơm nước"
                :icon="Droplets"
                on-text="Đang tưới nước"
                off-text="Đã dừng"
                active-bg-class="bg-blue-600"
              />
              <DeviceCard
                :model-value="devices.auto.isOn"
                @update:model-value="handleDeviceToggle('auto', $event)"
                title="Chế độ tự động"
                :icon="Cpu"
                :on-text="isAutoPaused ? 'Đang tạm dừng tự động' : 'Đang giám sát'"
                off-text="Thủ công"
                active-bg-class="bg-emerald-600"
              />
            </div>
          </div>

          <!-- Sensor Section -->
          <div class="space-y-4">
            <div class="flex items-center gap-2 text-slate-400 mb-2">
              <Activity class="h-4 w-4" />
              <span class="text-xs font-bold uppercase tracking-widest text-slate-400">Thông số cảm biến</span>
            </div>
            <div class="grid grid-cols-1 gap-4">
              <SensorCard
                label="Độ ẩm đất"
                :value="sensorData.humidity"
                unit="%"
                :icon="Droplet"
                color-class="text-emerald-500"
                bg-class="from-emerald-500/20 to-transparent"
              />
              <SensorCard
                label="Nhiệt độ môi trường"
                :value="sensorData.temperature"
                unit="°C"
                :icon="Thermometer"
                color-class="text-orange-500"
                bg-class="from-orange-500/20 to-transparent"
                :max-value="50"
              />
              <SensorCard
                label="Cường độ ánh sáng"
                :value="sensorData.light"
                unit="lx"
                :icon="Sun"
                color-class="text-amber-500"
                bg-class="from-amber-500/20 to-transparent"
                :max-value="1000"
              />
            </div>
          </div>
        </div>

        <!-- Right Side: Analytics Chart -->
        <div class="xl:col-span-2 space-y-8">
          <ChartCard 
            :data="sensorHistory" 
            title="Biểu đồ phân tích thực tế"
          />
          
          <!-- System Tips / Health -->
          <div class="rounded-2xl border border-emerald-100 bg-emerald-50 p-6">
             <div class="flex items-start gap-4">
                <div class="bg-white p-3 rounded-xl shadow-sm">
                  <Info class="h-6 w-6 text-emerald-500" />
                </div>
                <div>
                  <h4 class="font-bold text-slate-900 mb-1">Mẹo nông nghiệp</h4>
                  <p class="text-sm text-slate-600 leading-relaxed">
                    Độ ẩm đất lý tưởng cho vườn của bạn là từ <span class="text-emerald-600 font-bold">60-80%</span>. 
                    Hệ thống sẽ tự động đề xuất tưới nếu độ ẩm xuống dưới 40%.
                  </p>
                </div>
             </div>
          </div>
        </div>
      </div>
    </main>

    <!-- Footer -->
  </div>
</template>

<script setup>
import { reactive, onMounted, watch, ref } from 'vue'
import mqtt from 'mqtt'
import { 
  Leaf, Sun, Droplets, Droplet, Thermometer, Zap, 
  WifiOff, Calendar, ToggleRight, Activity, Info, Cpu 
} from 'lucide-vue-next'
import DeviceCard from './components/DeviceCard.vue'
import SensorCard from './components/SensorCard.vue'
import ChartCard from './components/ChartCard.vue'
import { sensorHistory, addDataPoint } from './utils/sensorStore'

// MQTT Configuration
const MQTT_CONFIG = {
  host: 'b47231f3e05d4693806682d3f94b7e9b.s1.eu.hivemq.cloud',
  port: 8884,
  protocol: 'wss',
  username: 'esp32',
  password: 'ESP32pass',
  path: '/mqtt'
}

const connectionStatus = ref('Disconnected')
const currentTime = ref(new Date().toLocaleString('vi-VN'))
const isAutoPaused = ref(false)
let autoPauseTimer = null
let client = null

// Real-time sensor display
const sensorData = reactive({
  temperature: 0,
  humidity: 0,
  light: 0
})

// Device state management
const devices = reactive({
  light: {
    isOn: false,
    topic: 'home/livingroom/light',
    key: 'led'
  },
  irrigation: {
    isOn: false,
    topic: 'home/garden/irrigation',
    key: 'water'
  },
  auto: {
    isOn: false,
    topic: 'home/garden/auto',
    key: 'auto'
  }
})

const connectMQTT = () => {
  connectionStatus.value = 'Connecting...'
  const url = `${MQTT_CONFIG.protocol}://${MQTT_CONFIG.host}:${MQTT_CONFIG.port}${MQTT_CONFIG.path}`
  
  client = mqtt.connect(url, {
    username: MQTT_CONFIG.username,
    password: MQTT_CONFIG.password,
    clientId: 'vue_dashboard_' + Math.random().toString(16).substring(2, 8)
  })

  client.on('connect', () => {
    connectionStatus.value = 'Connected'
    console.log('Connected to MQTT Cloud')
    client.subscribe([
      'esp32/nhietdo', 'esp32/doam', 'esp32/anhsang',
      'home/livingroom/light', 'home/garden/irrigation', 'home/garden/auto'
    ], (err) => {
      if (!err) console.log('Subscribed to all topics')
    })
  })

  client.on('message', (topic, message) => {
    const msgStr = message.toString()
    
    // Handle sensor topics first
    if (topic.startsWith('esp32/')) {
      const val = parseFloat(msgStr)
      if (!isNaN(val)) {
        if (topic === 'esp32/nhietdo') {
          sensorData.temperature = val.toFixed(1)
          addDataPoint('temperature', val)
        } else if (topic === 'esp32/doam') {
          sensorData.humidity = val.toFixed(1)
          addDataPoint('humidity', val)
        } else if (topic === 'esp32/anhsang') {
          sensorData.light = val.toFixed(1)
          addDataPoint('light', val)
        }
      }
      return
    }

    // Handle JSON payloads for state sync
    try {
      const doc = JSON.parse(msgStr)
      
      if (topic === 'home/livingroom/light' && doc.led !== undefined) {
        devices.light.isOn = doc.led
      } else if (topic === 'home/garden/irrigation' && doc.water !== undefined) {
        devices.irrigation.isOn = doc.water
      } else if (topic === 'home/garden/auto' && doc.auto !== undefined) {
        devices.auto.isOn = doc.auto
      }
    } catch (e) {
      console.error('MQTT JSON Parse Error:', e, 'Topic:', topic, 'Message:', msgStr)
    }
  })

  client.on('error', (err) => {
    connectionStatus.value = 'Error'
    console.error('MQTT Connection Error:', err)
  })

  client.on('close', () => {
    connectionStatus.value = 'Disconnected'
  })
}

const publishEvent = (topic, payload) => {
  if (client && client.connected) {
    client.publish(topic, JSON.stringify(payload), { qos: 1 }, (err) => {
      if (err) console.error(`Failed to publish to ${topic}:`, err)
    })
  }
}

const handleDeviceToggle = (deviceKey, newValue) => {
  devices[deviceKey].isOn = newValue
  publishEvent(devices[deviceKey].topic, { [devices[deviceKey].key]: newValue })
  
  // If user toggles light or irrigation while auto is on, show pause status
  if ((deviceKey === 'light' || deviceKey === 'irrigation') && devices.auto.isOn) {
    isAutoPaused.value = true
    if (autoPauseTimer) clearTimeout(autoPauseTimer)
    autoPauseTimer = setTimeout(() => {
      isAutoPaused.value = false
    }, 10000)
  }
}

onMounted(() => {
  connectMQTT()
  setInterval(() => {
    currentTime.value = new Date().toLocaleString('vi-VN')
  }, 1000)
})
</script>

