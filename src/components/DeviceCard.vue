<template>
  <el-card 
    class="device-card transition-all duration-300 transform hover:-translate-y-1 hover:shadow-xl"
    :body-style="{ padding: '24px' }"
  >
    <div class="flex items-center justify-between mb-6">
      <div class="flex items-center gap-4">
        <!-- Icon Container with Dynamic Color -->
        <div 
          :class="[
            'relative p-3 rounded-2xl transition-all duration-500 z-10',
            modelValue ? 'bg-primary-light text-white shadow-lg shadow-primary-light/30' : 'bg-slate-100 text-slate-400'
          ]"
        >
          <!-- Light Glow Animation -->
          <div 
            v-if="modelValue && title.toLowerCase().includes('đèn')"
            class="absolute inset-0 bg-yellow-400/30 rounded-2xl blur-xl animate-pulse -z-10"
          ></div>
          
          <!-- Water Ripple / Spray Animation -->
          <div 
            v-if="modelValue && title.toLowerCase().includes('tưới')"
            class="absolute inset-0 -z-10"
          >
            <div class="water-drop drop-1"></div>
            <div class="water-drop drop-2"></div>
            <div class="water-drop drop-3"></div>
          </div>

          <el-icon :size="28">
            <component :is="icon" />
          </el-icon>
        </div>
        
        <div class="z-10">
          <h3 class="text-lg font-bold text-slate-800 m-0">{{ title }}</h3>
          <p 
            :class="[
              'text-sm font-medium mt-1 transition-colors duration-300',
              modelValue ? 'text-primary' : 'text-slate-400'
            ]"
          >
            {{ modelValue ? onText : offText }}
          </p>
        </div>
      </div>

      <!-- Control Switch -->
      <el-switch
        :model-value="modelValue"
        @update:model-value="$emit('update:modelValue', $event)"
        class="custom-switch"
        style="--el-switch-on-color: #4f46e5"
      />
    </div>

    <!-- Status Indicator Bar -->
    <div class="mt-4 h-1.5 w-full bg-slate-100 rounded-full overflow-hidden">
      <div 
        :class="[
          'h-full transition-all duration-500 ease-out rounded-full',
          modelValue ? 'w-full bg-primary' : 'w-0 bg-slate-300'
        ]"
      />
    </div>
  </el-card>
</template>

<script setup>
defineProps({
  title: String,
  icon: Object,
  modelValue: Boolean,
  onText: {
    type: String,
    default: 'Đang bật'
  },
  offText: {
    type: String,
    default: 'Đã tắt'
  }
})

defineEmits(['update:modelValue'])
</script>

<style scoped>
.device-card {
  border-radius: 20px;
  border: 1px solid rgba(255, 255, 255, 0.6);
  background: rgba(255, 255, 255, 0.8);
  backdrop-filter: blur(10px);
}

.custom-switch :deep(.el-switch__core) {
  height: 24px;
  border-radius: 12px;
}

.custom-switch :deep(.el-switch__action) {
  width: 20px;
  height: 20px;
}

/* Water Spray Animation */
.water-drop {
  position: absolute;
  top: 50%;
  left: 50%;
  width: 6px;
  height: 6px;
  background: #3b82f6;
  border-radius: 50%;
  opacity: 0;
  transform: translate(-50%, -50%);
}

.drop-1 { animation: spray 1.5s infinite; }
.drop-2 { animation: spray 1.5s infinite 0.5s; }
.drop-3 { animation: spray 1.5s infinite 1s; }

@keyframes spray {
  0% { transform: translate(-50%, -50%) scale(0); opacity: 0; }
  50% { opacity: 0.8; }
  100% { transform: translate(calc(-50% + (var(--x, 0) * 40px)), calc(-50% + (var(--y, 0) * 40px))) scale(2); opacity: 0; }
}

.drop-1 { --x: 1; --y: -1; }
.drop-2 { --x: -1; --y: -1; }
.drop-3 { --x: 0; --y: -1.5; }
</style>
