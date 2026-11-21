<script setup lang="ts">
import { onMounted, onUnmounted, ref, watch } from 'vue'
import { withBase } from 'vitepress'
import './home.css'

const github = 'https://github.com/ludotengine/ludot'

// Each scene pairs a world in the live demo with the code block from index.md
// at the same position. The still shows until the live demo has loaded.
const scenes = [
	{
		file: 'GasGiant.lushader',
		focus: 'Giant',
		image: '/home/vesper-giant.webp',
		alt: 'A banded gas giant with dusty rings',
	},
	{
		file: 'Clouds.lushader',
		focus: 'Earth',
		image: '/home/vesper-earth.webp',
		alt: 'An ocean planet with white cloud bands and a small moon',
	},
	{
		file: 'OrbitBody.luau',
		focus: 'Sun',
		image: '/home/vesper-orbits.webp',
		alt: 'The whole solar system from above, with each world on its orbit around the sun',
	},
]
const shown = ref(0)
const live = ref(false)
const ready = ref(false)
const visible = ref(false)
const frame = ref<HTMLIFrameElement>()
const viewport = ref<HTMLElement>()
// Set once, when the frame is created, so the demo builds this world first.
const startFocus = ref(scenes[0].focus)

// Vesper's Flight script reads these every frame on the web. While the demo is off
// screen it drops to 2 frames per second, so it does not slow down the page.
const sendState = () => {
	const target = frame.value?.contentWindow as
		| (Window & { vesperFocus?: string; vesperActive?: boolean })
		| null
		| undefined
	if (target) {
		target.vesperFocus = scenes[shown.value].focus
		target.vesperActive = visible.value
	}
}

watch([shown, visible], sendState)

// The demo posts this once the world the page asked for is on screen. Until then the
// still stays on top, so the loading screen and the half-built system never show.
const onMessage = (event: MessageEvent) => {
	if (event.source === frame.value?.contentWindow && event.data === 'vesper-ready') {
		ready.value = true
	}
}

let observer: IntersectionObserver | undefined
let idle = 0
onMounted(() => {
	window.addEventListener('message', onMessage)
	observer = new IntersectionObserver((entries) => {
		visible.value = entries.some((entry) => entry.isIntersecting)
	})
	observer.observe(viewport.value!)
	// Start the download, compile, and world building as soon as the page is idle, so
	// they run while the visitor reads the hero.
	const start = () => {
		startFocus.value = scenes[shown.value].focus
		live.value = true
	}
	idle = window.requestIdleCallback
		? window.requestIdleCallback(start, { timeout: 1500 })
		: window.setTimeout(start, 300)
})
onUnmounted(() => {
	window.removeEventListener('message', onMessage)
	observer?.disconnect()
	if (window.cancelIdleCallback) {
		window.cancelIdleCallback(idle)
	} else {
		window.clearTimeout(idle)
	}
})

const facts = [
	{
		title: 'Luau classes',
		text: 'Brace classes extend engine nodes. Fields marked export show up in the Inspector, and onready fields run before _ready.',
		link: '/docs/luau/classes.html',
	},
	{
		title: 'Imports',
		text: 'import "@sim/Clock" as Clock resolves through .luaurc aliases. Plain require still works.',
		link: '/docs/luau/imports.html',
	},
	{
		title: 'Engine API in Luau casing',
		text: 'self:getNode("Camera"), Enum.MouseMode.Captured, typed Signal objects, and a task scheduler with task.wait.',
		link: '/docs/engine/naming.html',
	},
	{
		title: 'Hot reload',
		text: 'Saving a script updates running instances. They keep their field values, and connected callbacks run the new code.',
		link: '/docs/engine/reload.html',
	},
	{
		title: 'lushader',
		text: 'A typed Luau subset that compiles to canvas, spatial, and particle shaders. GPU errors point back to your source lines.',
		link: '/docs/reference/shaders.html',
	},
	{
		title: 'Editor and LSP',
		text: 'Type checking, completion, hover, a debugger, and a profiler in the editor. A standalone LSP serves other editors.',
		link: '/docs/reference/status.html',
	},
]
</script>

<template>
	<section class="landing-hero">
		<img
			class="landing-hero-image"
			:src="withBase('/home/vesper-desert.webp')"
			alt="A desert planet under a large sun, with a ringed planet in the sky, rendered in the Vesper demo"
			width="1600"
			height="900"
		/>
		<div class="landing-hero-text">
			<h1>A game engine you script in Luau.</h1>
			<p class="landing-lead">
				Make 2D and 3D games for Linux, Windows, and macOS.
			</p>
			<div class="landing-actions">
				<a
					class="landing-button primary"
					:href="withBase('/docs/guide/installation.html')"
					>Install</a
				>
				<a class="landing-button" :href="withBase('/docs/')"
					>Documentation</a
				>
			</div>
			<p class="landing-note">
				Ludot is experimental. Expect breaking changes.
			</p>
		</div>
	</section>

	<section class="landing-demo">
		<div class="landing-heading">
			<h2>The Vesper demo</h2>
			<a :href="`${github}/tree/main/demos/vesper`">Source</a>
		</div>
		<div class="landing-tabs" role="tablist" aria-label="Demo scenes">
			<button
				v-for="(scene, index) in scenes"
				:key="scene.file"
				role="tab"
				:aria-selected="shown === index"
				@click="shown = index"
			>
				{{ scene.file }}
			</button>
		</div>
		<div class="landing-scene">
			<div ref="viewport" class="landing-viewport">
				<img
					:src="withBase(scenes[shown].image)"
					:alt="scenes[shown].alt"
					width="960"
					height="540"
				/>
				<iframe
					v-if="live"
					ref="frame"
					:class="{ ready }"
					:src="withBase(`/vesper/play.htm#${startFocus}`)"
					title="Vesper demo, running live. Drag to look around and scroll to zoom."
					@load="sendState"
				></iframe>
			</div>
			<div class="landing-files" :data-file="shown">
				<slot />
			</div>
		</div>
	</section>

	<section class="landing-facts">
		<div class="landing-heading">
			<h2>What works now</h2>
			<a :href="withBase('/docs/reference/status.html')">Status</a>
		</div>
		<div class="landing-fact-grid">
			<a
				v-for="fact in facts"
				:key="fact.title"
				:href="withBase(fact.link)"
			>
				<h3>{{ fact.title }}</h3>
				<p>{{ fact.text }}</p>
			</a>
		</div>
	</section>

	<section class="landing-playground">
		<div class="landing-heading">
			<h2>Try the language</h2>
			<a :href="withBase('/docs/playground.html')">Full playground</a>
		</div>
		<Playground compact />
	</section>

	<footer class="landing-footer">
		<a class="brand" :href="withBase('/')"
			><img :src="withBase('/logo.svg')" alt="" />Ludot</a
		>
		<a :href="withBase('/docs/guide/licenses.html')">Licenses</a>
		<a :href="github">GitHub</a>
	</footer>
</template>
