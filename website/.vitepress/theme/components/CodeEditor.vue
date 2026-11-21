<script setup lang="ts">
import { onMounted, onUnmounted, ref, watch } from 'vue'
import { useData, withBase } from 'vitepress'
const props = defineProps<{ modelValue: string; engine: boolean }>()
const emit = defineEmits(['update:modelValue', 'run'])
const { isDark } = useData()
const parent = ref<HTMLElement>()
const status = ref('Loading editor…')
let editor: ReturnType<typeof import('./code-editor').mountEditor> | undefined
let disposed = false
onMounted(async () => {
	try {
		const { mountEditor } = await import('./code-editor')
		if (disposed) return
		editor = mountEditor(parent.value!, {
			code: props.modelValue,
			dark: isDark.value,
			engine: props.engine,
			runtime: new URL(withBase('/runtime/ludot.mjs'), location.origin)
				.href,
			api: new URL(withBase('/engine.api'), location.origin).href,
			change: (code) => emit('update:modelValue', code),
			run: () => emit('run'),
			status: (message) => {
				status.value = message
			},
		})
		status.value = 'Ctrl+Space for suggestions · Hover for types'
	} catch {
		status.value = 'Editor could not load. Reload the page to try again.'
	}
})
watch(
	() => props.modelValue,
	(code) => editor?.setCode(code),
)
watch(
	() => props.engine,
	(value) => editor?.setEngine(value),
)
watch(isDark, (value) => editor?.setDark(value))
defineExpose({ check: () => editor?.check() })
onUnmounted(() => {
	disposed = true
	editor?.destroy()
})
</script>

<template>
	<div class="intellisense-editor">
		<div ref="parent" class="codemirror-host"></div>
		<div class="editor-service-status" role="status">{{ status }}</div>
	</div>
</template>
