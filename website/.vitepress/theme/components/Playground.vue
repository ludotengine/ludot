<script setup lang="ts">
import { onMounted, onUnmounted, ref } from 'vue'
import { withBase } from 'vitepress'
import { examples } from './examples'
import CodeEditor from './CodeEditor.vue'
const props = withDefaults(
	defineProps<{ example?: keyof typeof examples; compact?: boolean }>(),
	{ example: 'Enums & matching' },
)
const engineMode = ref(false)
const codeEditor = ref<InstanceType<typeof CodeEditor>>()
const selected = ref<keyof typeof examples>(props.example)
const code = ref<string>(examples[props.example])
onMounted(() => {
	const saved = sessionStorage.getItem('ludot:playground')
	if (!saved) return
	sessionStorage.removeItem('ludot:playground')
	try {
		const snippet = JSON.parse(saved)
		if (typeof snippet.code === 'string' && snippet.code.length <= 50000) {
			code.value = snippet.code
			engineMode.value = snippet.engine === true
		}
	} catch {
		/* Ignore invalid browser storage. */
	}
})
const output = ref('Run the example to see its output.')
const busy = ref(false)
const failed = ref(false)
let worker: Worker | undefined
let timer: ReturnType<typeof setTimeout> | undefined
function stop() {
	worker?.terminate()
	worker = undefined
	clearTimeout(timer)
	busy.value = false
}
function cancelRun() {
	stop()
	output.value = 'Stopped.'
}
function selectExample() {
	stop()
	code.value = examples[selected.value]
	output.value = 'Run the example to see its output.'
	failed.value = false
}
async function checkTypes() {
	if (!engineMode.value) {
		run('check')
		return
	}
	busy.value = true
	failed.value = false
	output.value = 'Checking types…'
	try {
		const result = await codeEditor.value?.check()
		if (!result) {
			output.value = 'The editor is still loading.'
			return
		}
		failed.value = result.diagnostics.length > 0
		output.value =
			result.diagnostics
				.map(
					(item) =>
						`${item.line + 1}:${item.column + 1}: ${item.message}`,
				)
				.join('\n') || 'Type check passed.'
	} catch (error) {
		failed.value = true
		output.value = error instanceof Error ? error.message : String(error)
	} finally {
		busy.value = false
	}
}
function run(action: 'run' | 'check') {
	stop()
	if (code.value.length > 50000) {
		output.value = 'This playground accepts up to 50,000 characters.'
		failed.value = true
		return
	}
	busy.value = true
	failed.value = false
	output.value =
		action === 'check' ? 'Loading the type checker…' : 'Loading Ludot…'
	worker = new Worker(new URL('./runner.worker.ts', import.meta.url), {
		type: 'module',
	})
	worker.onmessage = ({ data }) => {
		output.value =
			(data.output || '') +
			(data.error ||
				(data.checked
					? 'Type check passed.'
					: data.output
						? ''
						: 'Finished without output.'))
		failed.value = !!data.error
		stop()
	}
	worker.onerror = () => {
		output.value =
			'The runtime could not load. Check your connection and try again.'
		failed.value = true
		stop()
	}
	timer = setTimeout(() => {
		stop()
		failed.value = true
		output.value =
			'Stopped after 10 seconds. Check for an infinite loop, or try a smaller program.'
	}, 10000)
	worker.postMessage({
		runtime: new URL(withBase('/runtime/ludot.mjs'), window.location.origin)
			.href,
		code: code.value,
		action,
	})
}
onUnmounted(stop)
</script>

<template>
	<div class="playground" :class="{ compact }">
		<div class="playground-toolbar">
			<label
				>Example
				<select v-model="selected" @change="selectExample">
					<option v-for="(_, name) in examples" :key="name">
						{{ name }}
					</option>
				</select></label
			><span class="runtime-label">Ludot · WebAssembly</span>
		</div>

		<label class="playground-context"
			>Environment
			<select v-model="engineMode">
				<option :value="false">Standalone Luau</option>
				<option :value="true">Engine API · check only</option>
			</select>
		</label>
		<CodeEditor
			ref="codeEditor"
			v-model="code"
			:engine="engineMode"
			@run="!engineMode && run('run')"
		/>
		<div class="playground-controls">
			<button
				class="primary-button"
				:disabled="busy || engineMode"
				@click="run('run')"
			>
				{{ busy ? 'Working…' : 'Run code' }}</button
			><button :disabled="busy" @click="checkTypes">Check types</button
			><button v-if="busy" @click="cancelRun">Stop</button
			><button v-else @click="selectExample">Reset</button
			><span>Ctrl / ⌘ Enter to run</span>
		</div>
		<div class="output-heading">
			Output <span>{{ failed ? 'Error' : 'Console' }}</span>
		</div>
		<pre
			class="playground-output"
			:class="{ error: failed }"
			role="status"
			aria-live="polite"
			>{{ output }}</pre
		>
		<p class="playground-note">
			Runs locally in your browser using this repository's Luau fork.
			Engine objects, tasks, signals, and file imports require the desktop
			engine. Engine mode supplies completion, hover, and inline checking
			without running a scene. Ctrl+Space opens suggestions; Escape then
			Tab leaves the editor.
		</p>
	</div>
</template>
