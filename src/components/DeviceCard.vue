<script setup>
import { computed } from 'vue';

const props = defineProps({
  title: String,
  icon: Object,
  modelValue: Boolean,
  onText: { type: String, default: 'Đang hoạt động' },
  offText: { type: String, default: 'Đang tắt' },
  colorClass: { type: String, default: 'text-blue-400' },
  activeBgClass: { type: String, default: 'bg-blue-500' }
});

const emit = defineEmits(['update:modelValue']);

const toggle = () => {
  emit('update:modelValue', !props.modelValue);
};

const isHeater = computed(() => props.title.toLowerCase().includes('đèn') || props.title.toLowerCase().includes('sưởi'));
const isPump = computed(() => props.title.toLowerCase().includes('bơm') || props.title.toLowerCase().includes('tưới'));
</script>

<template>
  <div 
    class="relative overflow-hidden rounded-2xl border border-slate-200 bg-white p-6 shadow-sm transition-all duration-500"
    :class="modelValue ? 'shadow-lg shadow-slate-200 ring-1 ring-slate-100' : 'hover:border-slate-300'"
  >
    <!-- Background Glow for Active State -->
    <div 
      v-if="modelValue"
      :class="[
        'absolute -right-8 -top-8 h-32 w-32 rounded-full blur-3xl transition-all duration-1000 opacity-10',
        isHeater ? 'bg-orange-500' : 'bg-blue-500'
      ]"
    ></div>

    <div class="flex items-center justify-between relative z-10">
      <div class="flex items-center gap-4">
        <!-- Icon Container -->
        <div 
          :class="[
            'relative flex h-14 w-14 items-center justify-center rounded-2xl transition-all duration-500',
            modelValue ? (isHeater ? 'bg-orange-50/50 text-orange-500 shadow-sm animate-glow' : 'bg-blue-50/50 text-blue-500 shadow-sm') : 'bg-slate-50 text-slate-400'
          ]"
        >
          <!-- Ripple effect for Pump -->
          <div v-if="modelValue && isPump" class="ripple-effect h-full w-full"></div>
          
          <component :is="icon" :stroke-width="2.5" class="h-7 w-7 relative z-10" />
        </div>

        <div>
          <h3 class="text-lg font-bold text-slate-900 font-heading">{{ title }}</h3>
          <p :class="['text-sm font-medium transition-colors duration-300', modelValue ? 'text-slate-600' : 'text-slate-400']">
            {{ modelValue ? onText : offText }}
          </p>
        </div>
      </div>

      <!-- Custom Toggle -->
      <button 
        @click="toggle"
        :class="[
          'relative inline-flex h-7 w-12 items-center rounded-full transition-all duration-300 focus:outline-none',
          modelValue ? activeBgClass : 'bg-slate-200'
        ]"
      >
        <span 
          :class="[
            'inline-block h-5 w-5 transform rounded-full bg-white shadow transition-transform duration-300',
            modelValue ? 'translate-x-6' : 'translate-x-1'
          ]"
        ></span>
      </button>
    </div>

    <!-- Status Bar -->
    <div class="mt-6 flex flex-col gap-2 relative z-10">
      <div class="h-1 w-full overflow-hidden rounded-full bg-slate-100">
        <div 
          :class="[
            'h-full transition-all duration-700 ease-out rounded-full',
            modelValue ? 'w-full ' + activeBgClass : 'w-0'
          ]"
        ></div>
      </div>
    </div>
  </div>
</template>
