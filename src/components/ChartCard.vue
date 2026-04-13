<script setup>
import { ref, onMounted, onUnmounted, watch, nextTick } from 'vue';
import * as echarts from 'echarts';
import { LineChart } from 'lucide-vue-next';

const props = defineProps({
  data: {
    type: Object,
    required: true
  },
  title: {
    type: String,
    default: 'Biểu đồ dữ liệu'
  }
});

const chartRef = ref(null);
let myChart = null;

const initChart = () => {
  if (!chartRef.value) return;
  
  myChart = echarts.init(chartRef.value);
  const option = {
    backgroundColor: 'transparent',
    tooltip: {
      trigger: 'axis',
      backgroundColor: 'rgba(255, 255, 255, 0.9)',
      borderColor: 'rgba(0, 0, 0, 0.05)',
      textStyle: { color: '#0f172a' },
      axisPointer: {
        lineStyle: { color: 'rgba(0,0,0,0.05)' }
      }
    },
    legend: {
      data: ['Nhiệt độ', 'Độ ẩm', 'Ánh sáng'],
      textStyle: { color: '#64748b', fontSize: 12 },
      bottom: 0,
      icon: 'circle'
    },
    grid: {
      left: '3%',
      right: '4%',
      bottom: '12%',
      top: '10%',
      containLabel: true
    },
    xAxis: {
      type: 'category',
      boundaryGap: false,
      data: props.data.timestamps,
      axisLine: { lineStyle: { color: 'rgba(0,0,0,0.02)' } },
      axisLabel: { color: '#94a3b8' },
      splitLine: { show: false }
    },
    yAxis: [
      {
        type: 'value',
        name: '°C / %',
        nameTextStyle: { color: '#94a3b8' },
        axisLine: { show: false },
        axisLabel: { color: '#94a3b8' },
        splitLine: { lineStyle: { color: 'rgba(0,0,0,0.03)' } }
      },
      {
        type: 'value',
        name: 'lux',
        position: 'right',
        nameTextStyle: { color: '#94a3b8' },
        axisLine: { show: false },
        axisLabel: { color: '#94a3b8' },
        splitLine: { show: false }
      }
    ],
    series: [
      {
        name: 'Nhiệt độ',
        type: 'line',
        smooth: true,
        data: props.data.temperature,
        itemStyle: { color: '#f97316' }, // Orange
        areaStyle: {
          color: new echarts.graphic.LinearGradient(0, 0, 0, 1, [
            { offset: 0, color: 'rgba(249, 115, 22, 0.1)' },
            { offset: 1, color: 'rgba(249, 115, 22, 0)' }
          ])
        },
        symbol: 'none'
      },
      {
        name: 'Độ ẩm',
        type: 'line',
        smooth: true,
        data: props.data.humidity,
        itemStyle: { color: '#10b981' }, // Emerald
        areaStyle: {
          color: new echarts.graphic.LinearGradient(0, 0, 0, 1, [
            { offset: 0, color: 'rgba(16, 185, 129, 0.1)' },
            { offset: 1, color: 'rgba(16, 185, 129, 0)' }
          ])
        },
        symbol: 'none'
      },
      {
        name: 'Ánh sáng',
        type: 'line',
        yAxisIndex: 1,
        smooth: true,
        data: props.data.light,
        itemStyle: { color: '#f59e0b' }, // Amber
        symbol: 'none'
      }
    ]
  };
  
  myChart.setOption(option);
};


const updateChart = () => {
  if (myChart) {
    myChart.setOption({
      xAxis: { data: props.data.timestamps },
      series: [
        { data: props.data.temperature },
        { data: props.data.humidity },
        { data: props.data.light }
      ]
    });
  }
};

watch(() => props.data, () => {
  updateChart();
}, { deep: true });

onMounted(() => {
  nextTick(() => {
    initChart();
    window.addEventListener('resize', handleResize);
  });
});

onUnmounted(() => {
  window.removeEventListener('resize', handleResize);
  myChart?.dispose();
});

const handleResize = () => {
  myChart?.resize();
};
</script>

<template>
  <div class="rounded-2xl border border-white/5 bg-slate-800/40 p-6 backdrop-blur-xl transition-all duration-300">
    <div class="mb-6 flex items-center justify-between">
      <div class="flex items-center gap-3">
        <div class="rounded-lg bg-slate-700/50 p-2 text-slate-400">
          <LineChart class="h-5 w-5" />
        </div>
        <h2 class="text-xl font-bold text-white font-heading">{{ title }}</h2>
      </div>
      <div class="flex items-center gap-2">
        <span class="flex h-2 w-2 rounded-full bg-emerald-500 animate-pulse"></span>
        <span class="text-[10px] font-bold uppercase tracking-widest text-slate-500">Dữ liệu thời gian thực</span>
      </div>
    </div>
    
    <div ref="chartRef" class="h-[350px] w-full"></div>
  </div>
</template>
