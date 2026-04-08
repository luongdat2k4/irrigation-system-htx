<template>
  <div class="min-h-screen p-6 md:p-12 max-w-6xl mx-auto">
    <!-- Header Section -->
    <header class="mb-12 animate-fade-in-down">
      <div class="flex items-center gap-3 mb-2">
        <div class="p-2.5 bg-primary text-white rounded-xl shadow-lg">
          <el-icon :size="24"><HomeFilled /></el-icon>
        </div>
        <h1 class="text-3xl font-extrabold text-slate-900 tracking-tight">SmartHome Dashboard</h1>
      </div>
      <p class="text-slate-500 font-medium ml-12">Chào mừng trở lại! Hệ thống của bạn đang hoạt động ổn định.</p>
    </header>

    <!-- Main Content Section -->
    <main class="space-y-12">
      <div class="grid grid-cols-1 md:grid-cols-2 lg:grid-cols-3 gap-8">
        <!-- Light Control Card -->
        <device-card
          v-model="devices.light.isOn"
          title="Đèn Phòng Khách"
          :icon="Sunny"
          on-text="Đang bật"
          off-text="Đã tắt"
          class="animate-slide-up-1"
        />

        <!-- Irrigation Control Card -->
        <device-card
          v-model="devices.irrigation.isOn"
          title="Hệ Thống Tưới"
          :icon="Drizzling"
          on-text="Đang tưới nước"
          off-text="Đã tắt"
          class="animate-slide-up-2"
        />

        <!-- Statistics or Info Card (Real-time) -->
        <el-card 
          class="info-card animate-slide-up-3"
          :body-style="{ padding: '24px' }"
        >
          <div class="flex items-center gap-4 mb-4 text-slate-400">
            <el-icon :size="20"><InfoFilled /></el-icon>
            <span class="text-sm font-bold uppercase tracking-wider">Thông tin hệ thống</span>
          </div>
          <div class="space-y-4">
            <div class="flex justify-between items-center">
              <span class="text-slate-500">Nhiệt độ</span>
              <span class="font-bold text-slate-800 text-lg">{{ sensorData.temperature }}°C</span>
            </div>
            <div class="flex justify-between items-center">
              <span class="text-slate-500">Độ ẩm</span>
              <span class="font-bold text-slate-800 text-lg">{{ sensorData.humidity }}%</span>
            </div>
            <div class="flex justify-between items-center">
              <span class="text-slate-500">Ánh sáng</span>
              <span class="font-bold text-slate-800 text-lg">{{ sensorData.light }} lx</span>
            </div>
          </div>
        </el-card>
      </div>

      <!-- Trend Chart Section -->
      <section class="animate-slide-up-3">
        <el-card class="chart-card" :body-style="{ padding: '24px' }">
          <div class="flex items-center justify-between mb-8">
            <div class="flex items-center gap-4">
              <div class="p-2 bg-blue-50 text-blue-500 rounded-lg">
                <el-icon :size="20"><TrendCharts /></el-icon>
              </div>
              <h2 class="text-xl font-bold text-slate-800">Biểu đồ xu hướng (Real-time)</h2>
            </div>
            <span class="text-xs font-bold text-slate-400 uppercase tracking-widest">Last 10 Samples</span>
          </div>
          <div ref="chartRef" class="w-full h-[400px]"></div>
        </el-card>
      </section>
    </main>

    <!-- Footer / Status Bar -->
    <footer class="mt-12 pt-8 border-t border-slate-200/60 flex justify-between items-center text-slate-400 text-sm">
      <p>© 2026 Antigravity IoT Solutions</p>
      <div class="flex items-center gap-2">
        <span 
          :class="[
            'w-2 h-2 rounded-full transition-all duration-300',
            connectionStatus === 'Connected' ? 'bg-green-500 shadow-[0_0_8px_rgba(34,197,94,0.6)]' : 
            connectionStatus === 'Connecting...' ? 'bg-yellow-500 animate-pulse' : 'bg-red-500'
          ]"
        ></span>
        <span class="font-medium tracking-wide uppercase text-[10px]">{{ connectionStatus }}</span>
      </div>
    </footer>
  </div>
</template>

<script setup>
import { reactive, onMounted, watch, ref, nextTick } from 'vue'
import mqtt from 'mqtt'
import * as echarts from 'echarts'
import { HomeFilled, Sunny, Drizzling, InfoFilled, TrendCharts } from '@element-plus/icons-vue'
import DeviceCard from './components/DeviceCard.vue'
import { sensorHistory, addDataPoint } from './utils/sensorStore'

// MQTT Configuration
const MQTT_CONFIG = {
  host: '2bc93b5857b249d3988054d99e413b9a.s1.eu.hivemq.cloud',
  port: 8884,
  protocol: 'wss',
  username: 'esp32',
  password: 'Ltdat2004@',
  path: '/mqtt'
}

const connectionStatus = ref('Disconnected')
const chartRef = ref(null)
let client = null
let myChart = null

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
  }
})

const initChart = () => {
  if (!chartRef.value) return
  
  myChart = echarts.init(chartRef.value)
  const option = {
    tooltip: { trigger: 'axis' },
    legend: { data: ['Nhiệt độ', 'Độ ẩm', 'Ánh sáng'], bottom: 0 },
    grid: { left: '3%', right: '4%', bottom: '15%', containLabel: true },
    xAxis: {
      type: 'category',
      boundaryGap: false,
      data: sensorHistory.timestamps,
      axisLine: { lineStyle: { color: '#e2e8f0' } }
    },
    yAxis: [
      {
        type: 'value',
        name: '°C / %',
        min: 0,
        max: 100,
        axisLine: { lineStyle: { color: '#64748b' } }
      },
      {
        type: 'value',
        name: 'lx',
        position: 'right',
        axisLine: { lineStyle: { color: '#64748b' } }
      }
    ],
    series: [
      {
        name: 'Nhiệt độ',
        type: 'line',
        smooth: true,
        data: sensorHistory.temperature,
        itemStyle: { color: '#f87171' },
        areaStyle: { color: new echarts.graphic.LinearGradient(0, 0, 0, 1, [{ offset: 0, color: 'rgba(248, 113, 113, 0.3)' }, { offset: 1, color: 'rgba(248, 113, 113, 0)' }]) }
      },
      {
        name: 'Độ ẩm',
        type: 'line',
        smooth: true,
        data: sensorHistory.humidity,
        itemStyle: { color: '#60a5fa' },
        areaStyle: { color: new echarts.graphic.LinearGradient(0, 0, 0, 1, [{ offset: 0, color: 'rgba(96, 165, 250, 0.3)' }, { offset: 1, color: 'rgba(96, 165, 250, 0)' }]) }
      },
      {
        name: 'Ánh sáng',
        type: 'line',
        yAxisIndex: 1,
        smooth: true,
        data: sensorHistory.light,
        itemStyle: { color: '#facc15' }
      }
    ]
  }
  myChart.setOption(option)
}

const updateChart = () => {
  if (myChart) {
    myChart.setOption({
      xAxis: { data: sensorHistory.timestamps },
      series: [
        { data: sensorHistory.temperature },
        { data: sensorHistory.humidity },
        { data: sensorHistory.light }
      ]
    })
  }
}

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
    // Subscribe to sensor topics
    client.subscribe(['esp32/nhietdo', 'esp32/doam', 'esp32/anhsang'], (err) => {
      if (!err) console.log('Subscribed to sensor topics')
    })
  })

  client.on('message', (topic, message) => {
    const val = parseFloat(message.toString())
    if (isNaN(val)) return

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
    
    updateChart()
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
      else console.log(`Published to ${topic}:`, payload)
    })
  }
}

watch(() => devices.light.isOn, (newVal) => {
  publishEvent(devices.light.topic, { [devices.light.key]: newVal })
})

watch(() => devices.irrigation.isOn, (newVal) => {
  publishEvent(devices.irrigation.topic, { [devices.irrigation.key]: newVal })
})

onMounted(() => {
  connectMQTT()
  nextTick(() => {
    initChart()
    window.addEventListener('resize', () => myChart?.resize())
  })
})
</script>

<style>
/* Custom Animations */
@keyframes fadeInDown {
  from { opacity: 0; transform: translateY(-20px); }
  to { opacity: 1; transform: translateY(0); }
}

@keyframes slideUp {
  from { opacity: 0; transform: translateY(30px); }
  to { opacity: 1; transform: translateY(0); }
}

.animate-fade-in-down {
  animation: fadeInDown 0.8s ease-out forwards;
}

.animate-slide-up-1 { animation: slideUp 0.6s ease-out 0.1s both; }
.animate-slide-up-2 { animation: slideUp 0.6s ease-out 0.2s both; }
.animate-slide-up-3 { animation: slideUp 0.6s ease-out 0.3s both; }

.info-card, .chart-card {
  border-radius: 20px;
  border: 1px solid rgba(255, 255, 255, 0.6);
  background: rgba(255, 255, 255, 0.8);
  backdrop-filter: blur(10px);
}
</style>
