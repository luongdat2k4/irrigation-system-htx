<script setup>
import { computed } from 'vue';

const props = defineProps({
  label: { type: String, required: true },
  value: { type: [Number, String], required: true },
  unit: { type: String, default: '' },
  icon: { type: Object, required: true },
  colorClass: { type: String, default: 'text-emerald-400' },
  bgClass: { type: String, default: 'from-emerald-500/10 to-transparent' },
  maxValue: { type: Number, default: 100 }
});

const progressWidth = computed(() => {
  const val = parseFloat(props.value);
  if (isNaN(val)) return 0;
  return Math.min((val / props.maxValue) * 100, 100);
});
</script>

<template>
  <div class="group relative overflow-hidden rounded-2xl border border-slate-200 bg-white p-5 shadow-sm transition-all duration-300 hover:-translate-y-1 hover:border-emerald-500/20 hover:shadow-xl hover:shadow-emerald-900/5">
    <!-- Decorative background gradient -->
    <div :class="['absolute -right-4 -top-4 h-24 w-24 rounded-full bg-gradient-to-br opacity-5 blur-2xl transition-opacity group-hover:opacity-10', bgClass]"></div>

    <div class="flex items-start justify-between">
      <div class="space-y-1">
        <p class="text-xs font-semibold uppercase tracking-wider text-slate-500">{{ label }}</p>
        <div class="flex items-baseline gap-1">
          <h3 class="text-3xl font-bold tracking-tight text-slate-900 font-heading">{{ value }}</h3>
          <span class="text-sm font-medium text-slate-400">{{ unit }}</span>
        </div>
      </div>
      
      <div :class="['rounded-xl bg-slate-50 p-2.5 shadow-inner transition-colors group-hover:bg-slate-100', colorClass]">
        <component :is="icon" class="h-6 w-6" />
      </div>
    </div>

    <!-- Progress bar -->
    <div class="mt-6 flex flex-col gap-2">
      <div class="h-1.5 w-full overflow-hidden rounded-full bg-slate-100">
        <div 
          :class="['h-full rounded-full transition-all duration-1000 ease-out', colorClass.replace('text-', 'bg-')]"
          :style="{ width: `${progressWidth}%` }"
        ></div>
      </div>
    </div>
  </div>
</template>
